/*
 * <ColorBox> — JS side of the minimal custom native component.
 *
 * `register` wires the JS element name "ColorBox" to the native ViewManager that
 * the C++ APMF_COMPONENT exposes. `validAttributes` lists the props that get
 * forwarded to native — these names MUST match registerProps() in
 * ColorBoxViewManager.cpp. `style` (layout) is handled by React Native and
 * arrives natively via ILayoutDelegate::updateLayout.
 */

// @ts-ignore - internal RN-Kepler helper, ships without type declarations
import register from '@amazon-devices/react-native-kepler/Libraries/Utilities/registerGeneratedViewConfig';
import type {HostComponent, ViewProps} from 'react-native';

export interface ColorBoxProps extends ViewProps {
  /** "#RRGGBB" -> native setBackgroundColor */
  color?: string;
  /** 0..1 -> native setOpacity */
  opacity?: number;
}

const ColorBoxNativeName = 'ColorBox';

register(ColorBoxNativeName, {
  uiViewClassName: ColorBoxNativeName,
  validAttributes: {
    color: true,
    opacity: true,
  },
});

export default ColorBoxNativeName as unknown as HostComponent<ColorBoxProps>;
