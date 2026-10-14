import { ApplicationStateChangeCallback, common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import nativeClient from 'libmain.so';
import { ClientTransport } from '../bluetooth/ClientTransport';

@Entry
@Component
struct Index {
  private visible: boolean = false;
  private context?: common.UIAbilityContext;
  private applicationContext?: common.ApplicationContext;
  private transport: ClientTransport = new ClientTransport((message: string) => {
    hilog.info(0x0000, 'SHCClient', '%{public}s', message);
  });
  private applicationState: ApplicationStateChangeCallback = {
    onApplicationBackground: () => { this.pause(); },
    onApplicationForeground: () => {
      if (this.visible) {
        this.resume();
      }
    }
  };

  aboutToAppear(): void {
    this.context = this.getUIContext().getHostContext() as common.UIAbilityContext;
    this.applicationContext = this.context.getApplicationContext();
    this.applicationContext.on('applicationStateChange', this.applicationState);
    nativeClient.setScale(vp2px(1));
  }

  onPageShow(): void {
    this.visible = true;
    this.resume();
  }

  onPageHide(): void {
    this.visible = false;
    this.pause();
  }

  aboutToDisappear(): void {
    this.visible = false;
    this.pause();
    if (this.applicationContext) {
      this.applicationContext.off('applicationStateChange', this.applicationState);
    }
  }

  private resume(): void {
    nativeClient.setScale(vp2px(1));
    if (this.context) {
      this.transport.start(this.context);
    }
  }

  private pause(): void {
    this.transport.stop();
  }

  build() {
    Stack() {
      XComponent({ id: 'SDL_XComponent', type: XComponentType.SURFACE, libraryname: 'SDL3' })
        .id('SDL_XComponent')
        .width('100%')
        .height('100%')
        .onLoad(() => { nativeClient.setScale(vp2px(1)); })
        .onDestroy(() => { this.pause(); })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#101826')
  }
}
