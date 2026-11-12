import { common } from '@kit.AbilityKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { util } from '@kit.ArkTS';
import { BusinessError } from '@kit.BasicServicesKit';
import { fileIo, fileUri } from '@kit.CoreFileKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { systemShare } from '@kit.ShareKit';
import nativeClient, { ShareRequest } from 'libmain.so';

const RETENTION_SECONDS: number = 7 * 24 * 60 * 60;
let cleanedRoot: string = '';

enum ShareResult {
  DISPATCHED = 1,
  CANCELLED = 2,
  FAILED = 3
}

function errorText(error: BusinessError): string {
  const message = error.code === undefined ? error.message : `${error.code}: ${error.message}`;
  return message.slice(0, 512);
}

export class FileShare {
  private timer: number = -1;
  private revision: number = 0;
  private activeId: number = 0;
  private resultSent: boolean = false;
  private cache?: Promise<void>;
  private controller?: systemShare.ShareController;
  private dismissCallback?: () => void;
  private completedCallback?: (result: systemShare.ShareOperationResult) => void;

  start(context: common.UIAbilityContext): void {
    if (this.timer >= 0) {
      return;
    }
    if (!canIUse('SystemCapability.Collaboration.SystemShare')) {
      nativeClient.setShareAvailable(false);
      return;
    }
    const revision = ++this.revision;
    this.timer = setInterval(() => { this.tick(context, revision); }, 50);
    nativeClient.setShareAvailable(true);
  }

  stop(): void {
    ++this.revision;
    if (this.timer >= 0) {
      clearInterval(this.timer);
      this.timer = -1;
    }
    nativeClient.setShareAvailable(false);
    this.clearController();
    this.activeId = 0;
  }

  private tick(context: common.UIAbilityContext, revision: number): void {
    if (revision !== this.revision || this.activeId !== 0) {
      return;
    }
    try {
      const request = nativeClient.pollShare();
      if (request) {
        this.activeId = request.id;
        this.resultSent = false;
        this.share(request, context, revision);
      }
    } catch (error) {
      hilog.error(0x0000, 'SHCShare', 'Unable to poll exports: %{public}s', errorText(error as BusinessError));
    }
  }

  private isCurrent(id: number, revision: number): boolean {
    return this.activeId === id && this.revision === revision;
  }

  private complete(id: number, result: ShareResult, error: string = ''): void {
    if (this.activeId !== id || this.resultSent) {
      return;
    }
    this.resultSent = true;
    nativeClient.completeShare(id, result, error);
  }

  private async share(request: ShareRequest, context: common.UIAbilityContext, revision: number): Promise<void> {
    const id = request.id;
    const root = `${context.cacheDir}/exports`;
    let directory: string = '';
    let handedOff: boolean = false;
    try {
      if (!this.cache) {
        this.cache = this.prepareCache(root).catch((error: BusinessError) => {
          this.cache = undefined;
          throw new Error(errorText(error as BusinessError));
        });
      }
      await this.cache;
      if (!this.isCurrent(id, revision)) {
        return;
      }
      const candidate = `${root}/export-${util.generateRandomUUID()}`;
      await fileIo.mkdir(candidate);
      directory = candidate;
      const path = `${directory}/${request.filename}`;
      await this.writeFile(path, request.data);
      request.data = new ArrayBuffer(0);
      if (!this.isCurrent(id, revision)) {
        return;
      }
      const type = request.mimeType === 'application/zip' ? utd.UniformDataType.ZIP_ARCHIVE :
        request.mimeType === 'application/octet-stream' ? utd.UniformDataType.FILE :
          utd.getUniformDataTypeByMIMEType(request.mimeType);
      const data = new systemShare.SharedData({
        utd: type,
        title: request.filename,
        uri: fileUri.getUriFromPath(path)
      });
      const controller = new systemShare.ShareController(data);
      this.controller = controller;
      this.dismissCallback = () => {
        if (!this.isCurrent(id, revision)) {
          return;
        }
        this.complete(id, ShareResult.DISPATCHED);
        this.clearController();
        this.activeId = 0;
      };
      this.completedCallback = (_result: systemShare.ShareOperationResult) => {
        if (this.isCurrent(id, revision)) {
          this.complete(id, ShareResult.DISPATCHED);
        }
      };
      controller.on('dismiss', this.dismissCallback);
      controller.on('shareCompleted', this.completedCallback);
      handedOff = true;
      try {
        await controller.show(context, {
          anchor: 'SDL_XComponent',
          selectionMode: systemShare.SelectionMode.SINGLE,
          previewMode: systemShare.SharePreviewMode.DEFAULT
        });
      } catch (error) {
        handedOff = false;
        throw new Error(errorText(error as BusinessError));
      }
    } catch (error) {
      if (this.isCurrent(id, revision)) {
        this.complete(id, ShareResult.FAILED, errorText(error as BusinessError));
        this.clearController();
        this.activeId = 0;
      }
    } finally {
      if (directory && !handedOff) {
        await this.removeDirectory(directory);
      }
    }
  }

  private async writeFile(path: string, data: ArrayBuffer): Promise<void> {
    const file = await fileIo.open(path, fileIo.OpenMode.WRITE_ONLY | fileIo.OpenMode.CREATE | fileIo.OpenMode.TRUNC);
    try {
      let offset = 0;
      while (offset < data.byteLength) {
        const chunk = data.slice(offset, Math.min(offset + 1024 * 1024, data.byteLength));
        const written = await fileIo.write(file.fd, chunk);
        if (written <= 0 || written > chunk.byteLength) {
          throw new Error('Incomplete export write');
        }
        offset += written;
      }
    } finally {
      await fileIo.close(file);
    }
  }

  private async prepareCache(root: string): Promise<void> {
    try {
      await fileIo.mkdir(root, true);
    } catch (error) {
      const info = await fileIo.lstat(root);
      if (!info.isDirectory()) {
        throw new Error(errorText(error as BusinessError));
      }
    }
    if (cleanedRoot === root) {
      return;
    }
    cleanedRoot = root;
    try {
      const cutoff = Date.now() / 1000 - RETENTION_SECONDS;
      for (const name of await fileIo.listFile(root)) {
        if (!/^export-[0-9a-f-]+$/i.test(name)) {
          continue;
        }
        const path = `${root}/${name}`;
        const info = await fileIo.lstat(path);
        if (info.isDirectory() && info.mtime < cutoff) {
          await this.removeDirectory(path);
        }
      }
    } catch (error) {
      hilog.warn(0x0000, 'SHCShare', 'Unable to clean export cache: %{public}s', errorText(error as BusinessError));
    }
  }

  private async removeDirectory(path: string): Promise<void> {
    try {
      await fileIo.rmdir(path);
    } catch (error) {
      hilog.warn(0x0000, 'SHCShare', 'Unable to remove export cache: %{public}s', errorText(error as BusinessError));
    }
  }

  private clearController(): void {
    const controller = this.controller;
    this.controller = undefined;
    if (controller) {
      try {
        if (this.dismissCallback) {
          controller.off('dismiss', this.dismissCallback);
        }
        if (this.completedCallback) {
          controller.off('shareCompleted', this.completedCallback);
        }
      } catch (error) {
        hilog.warn(0x0000, 'SHCShare', 'Unable to detach share callbacks: %{public}s', errorText(error as BusinessError));
      }
    }
    this.dismissCallback = undefined;
    this.completedCallback = undefined;
  }
}
