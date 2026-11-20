import { abilityAccessCtrl, AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { i18n } from '@kit.LocalizationKit';
import { inputMethod } from '@kit.IMEKit';
import pointer from '@ohos.multimodalInput.pointer';
import sdl from 'libSDL3.so';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    sdl.provideArkTSObjects(this, abilityAccessCtrl.createAtManager(), i18n.System,
      pointer.setPointerVisibleSync, inputMethod.getController());
  }
}
