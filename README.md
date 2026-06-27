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

## The two non-obvious gotchas

1. **`.so` naming = the deployment contract.** An app-provided in-process
   component is `dlopen`'d at runtime by a filename derived from its component
   path (`/com.example.customview` → `com.example.customview.so`), and its
   **SONAME must equal that filename** or the APMF self-registration
   (`ApmfRegisterDso`, run from the `.so`'s `INIT_ARRAY`) never resolves and you
   get *"could not find component … / Unimplemented component"*. CMake makes this
   exact with `set_target_properties(customview PROPERTIES OUTPUT_NAME
   "com.example.customview" PREFIX "")` — a real, flat `lib/<arch>/…so`, no
   omnikit/, no symlink. (Cross-checked against Amazon's own
   `@amazon-devices/vega-carousel`.)

2. **The UI-Toolkit IDL must be staged in the project.** The `react-native-vega`
   in-app sysroot has no CMake config for `KeplerUI{View,React,Graphics}Interface`,
   so the IDL trees are copied into `apmf/iface/` (the IDL importer searches the
   project dir). See setup below.

## Setup

```bash
npm install            # or copy node_modules from a working Vega RN project

# Stage the UI-Toolkit IDL the build imports (not in the in-app sysroot):
# copy the SDK's KeplerUI{View,React,Graphics}Interface include trees into
#   apmf/iface/com/amazon/kepler/uitoolkit/
# (flat <subpkg>-<Type>.idl.inc files, e.g. view-IView.idl.inc, graphics-Color.idl.inc)
```

## Build / run / verify

```bash
D=<your-device-serial>          # e.g. a Fire TV adb serial
npx --no-install react-native build-vega --build-type Debug --target armv7
VPKG=build/private/kepler/@amazon-devices/customviewdemo/undefined/vega/armv7/Debug/@amazon-devices/customviewdemo_armv7.vpkg
vega device install-app -d $D -p "$VPKG"
vega device launch-app  -d $D -a com.example.customviewdemo.main

# logs (native code logs via syslog — surfaces here; printf/stderr/tmp do not):
vega device start-log-stream -d $D | grep ColorBox
# screen capture (composited layer):
adb -s $D shell gwsi-tool-screenshooter /tmp/x.png && adb -s $D pull /tmp/x.png
```

Targets: `armv7` (Fire TV Stick) / `aarch64` / `x86_64` (simulator).

Expected `ColorBox` log on launch:
```
[AutoLinkService] Successfully linked 'ColorBox' from app metadata
[ColorBox] registerProps
[ColorBox] makeView            (×4)
[ColorBox] updateProps color=#ef4444 / #22c55e / #3b82f6 / #f59e0b
[ColorBox] updateLayout 603x220 …  1856x160
[ColorBox] updateProps opacity=0.35 → 0.66 …   (animated)
```
