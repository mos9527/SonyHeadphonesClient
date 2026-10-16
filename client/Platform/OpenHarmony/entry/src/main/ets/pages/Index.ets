import { common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import nativeClient from 'libmain.so';
import { ClientTransport } from '../bluetooth/ClientTransport';
import { FileShare } from '../sharing/FileShare';

@Entry
@Component
struct Index {
  private context?: common.UIAbilityContext;
  private fileShare: FileShare = new FileShare();
  private transport: ClientTransport = new ClientTransport((message: string) => {
    hilog.info(0x0000, 'SHCClient', '%{public}s', message);
  });

  aboutToAppear(): void {
    this.context = this.getUIContext().getHostContext() as common.UIAbilityContext;
    nativeClient.setScale(vp2px(1));
  }

  onPageShow(): void {
    this.resume();
  }

  aboutToDisappear(): void {
    this.fileShare.stop();
    this.transport.stop();
  }

  private resume(): void {
    nativeClient.setScale(vp2px(1));
    if (this.context) {
      this.fileShare.start(this.context);
      this.transport.start(this.context);
    }
  }

  build() {
    Stack() {
      XComponent({ id: 'SDL_XComponent', type: XComponentType.SURFACE, libraryname: 'SDL3' })
        .id('SDL_XComponent')
        .width('100%')
        .height('100%')
        .onLoad(() => { nativeClient.setScale(vp2px(1)); })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#101826')
  }
}
