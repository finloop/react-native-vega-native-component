# Minimal custom IView (Fabric component) for Amazon Vega

A tiny, **verified-on-device** reference for building a custom native UI component
(`IView`) for Amazon Vega / Kepler (Fire TV), driven from React Native. Amazon
ships an `IViewManager` *guide* but no runnable example — this is one, kept as
small as possible while still exercising the parts a real component needs:

- **props (string)** — `color="#RRGGBB"` → native `setBackgroundColor`
- **props (number, live-updating)** — `opacity={0..1}` → native `setOpacity`
- **layout** — React flexbox sizes each native view via `ILayoutDelegate`
- **multiple instances** of one component

The component, `<ColorBox>`, is just a native view whose background color +
opacity come from props and whose size comes from the React layout.

![verified on a Fire TV](docs/verified.png)

> Verified on a physical Fire TV (`armv7`): three `<ColorBox>` in a flex row
> (red/green/blue) + a full-width amber box with animated opacity.

## How it works

A custom Vega component is an **in-process APMF component** that implements the
UI-Toolkit *react* delegates. KeplerScript calls them in this order:

```
IViewManagerFactory::makeViewManager(name)   // once per component name
IViewManager::makeView()                      // once per mounted instance
IPropsDelegate::registerProps(builder)        // declare the prop schema
IPropsDelegate::updateProps(view, jsObject)   // prop values (mount + updates)
ILayoutDelegate::updateLayout(view, layout)   // React-computed size
```

`ColorBoxViewManager` implements `IViewManager + IPropsDelegate + ILayoutDelegate`
(see [`kepler/ColorBoxViewManager.cpp`](kepler/ColorBoxViewManager.cpp)). The
factory registers itself with `APMF_COMPONENT("/com.example.customview", …)`.

On the JS side, [`src/ColorBox.tsx`](src/ColorBox.tsx) calls
`registerGeneratedViewConfig('ColorBox', { uiViewClassName, validAttributes })` —
`validAttributes` lists the props forwarded to native and **must match**
`registerProps()`.

## File map

| File | Role |
|---|---|
| `kepler/ViewManagerFactory.{h,cpp}` | `IViewManagerFactory` + `APMF_COMPONENT` registration |
| `kepler/ColorBoxViewManager.{h,cpp}` | the component: `makeView` / `registerProps` / `updateProps` / `updateLayout` |
| `src/ColorBox.tsx` | JS element ↔ native binding (`register` + `validAttributes`) |
| `src/App.tsx` | demo using `<ColorBox color=… opacity=… style=…/>` |
| `react-native.config.js` | autolink: `uiComponentName` + `components: ['ColorBox']` |
| `CMakeLists.txt` | imports the UI-Toolkit IDL, builds the component `.so` |
| `manifest.toml` / `app.json` | app identity (`com.example.customviewdemo`) |

## Setup

```bash
npm install
```

## Build / run / verify

Build for your target — `armv7` for a physical Fire TV Stick, or the VVD
simulator's arch (`aarch64` on Apple Silicon, `x86_64` on Intel):

```bash
ARCH=aarch64                    # armv7 (Fire TV) | aarch64 / x86_64 (VVD simulator)
npx --no-install react-native build-vega --build-type Debug --target $ARCH
VPKG=build/private/kepler/react-native-vega-native-component/undefined/vega/$ARCH/Debug/react-native-vega-native-component_${ARCH}.vpkg
```

### Simulator (VVD)

Start it with `vega virtual-device start`, then target it with the literal
`-d Simulator` selector (or omit `-d` entirely when the VVD is the only device).
Note: the `amazon-…` serial that `vega device list` prints is **not** accepted
by `-d` — use `Simulator`.

```bash
vega device install-app -d Simulator -p "$VPKG"
vega device launch-app  -d Simulator -a com.example.customviewdemo.main
# native logs (via the APMF logger — surfaces here; printf/stderr/tmp do not):
vega device start-log-stream -d Simulator | grep ColorBox

# screenshot: gwsi-tool-screenshooter is physical-only (writes a 0-byte file on a VVD).
adb -s emulator-5554 emu screenrecord screenshot ./shots   # writes ./shots/Screenshot_*.png
```

### Physical Fire TV

Target the serial from `vega device list`:

```bash
D=<device-serial>
vega device install-app -d $D -p "$VPKG"
vega device launch-app  -d $D -a com.example.customviewdemo.main
vega device start-log-stream -d $D | grep ColorBox

# screenshot: official gwsi-tool-screenshooter via the device adapter (vda):
vega exec vda -s $D shell gwsi-tool-screenshooter /tmp/x.png && vega exec vda -s $D pull /tmp/x.png ./x.png
```

Targets: `armv7` (Fire TV Stick) / `aarch64` / `x86_64` (VVD simulator).

Expected `ColorBox` log on launch:
```
[AutoLinkService] Successfully linked 'ColorBox' from app metadata
[ColorBox] registerProps
[ColorBox] makeView            (×4)
[ColorBox] updateProps color=#ef4444 / #22c55e / #3b82f6 / #f59e0b
[ColorBox] updateLayout 603x220 …  1856x160
[ColorBox] updateProps opacity=0.35 → 0.66 …   (animated)
```
