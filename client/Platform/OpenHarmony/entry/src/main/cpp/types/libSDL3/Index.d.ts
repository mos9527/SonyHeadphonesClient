import { abilityAccessCtrl, UIAbility } from '@kit.AbilityKit';
import { i18n } from '@kit.LocalizationKit';
import { inputMethod } from '@kit.IMEKit';
import pointer from '@ohos.multimodalInput.pointer';

export const provideArkTSObjects: (
  ability: UIAbility,
  atManager: abilityAccessCtrl.AtManager,
  system: typeof i18n.System,
  setPointerVisible: typeof pointer.setPointerVisibleSync,
  imeController: inputMethod.InputMethodController
) => void;
