#include "ColorBoxViewManager.h"

#include <apmf/log.h>  // device logs via APMF logger; surfaces in `vega device start-log-stream`

#include <string>

#include <apmf/array.h>
#include <apmf/string.h>

#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Color.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Size.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IFrame.h>

namespace customview {

using namespace apmf::log_levels;
APMF_DEFINE_LOGGER(ColorBoxLogger, "com.example.customview");

namespace gfx = apmf::iface::com::amazon::kepler::uitoolkit::graphics;

namespace {
// Parse "#RRGGBB" into a Color (components are 0..1 floats). Falls back to gray.
gfx::Color parseColor(const std::string& s) {
  auto nib = [](char c) -> int {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
  };
  if (s.size() == 7 && s[0] == '#') {
    const float r = (nib(s[1]) * 16 + nib(s[2])) / 255.0f;
    const float g = (nib(s[3]) * 16 + nib(s[4])) / 255.0f;
    const float b = (nib(s[5]) * 16 + nib(s[6])) / 255.0f;
    return gfx::Color{r, g, b, 1.0f};
  }
  return gfx::Color{0.27f, 0.27f, 0.27f, 1.0f};
}
}  // namespace

ColorBoxViewManager::ColorBoxViewManager(View<vw::IViewModule> viewModule)
    : viewModule_(viewModule) {}

// Create the native view instance (one per <ColorBox> mounted in JS).
Ptr<vw::IView> ColorBoxViewManager::makeView() {
  APMF_LOG(ColorBoxLogger, INFO) << "[ColorBox] makeView";
  return viewModule_->makeView();
}

// Declare the props this component accepts. The names MUST match the JS spec's
// validAttributes, and the values JS sends arrive in updateProps().
Ptr<rx::IPropsRegistry> ColorBoxViewManager::registerProps(
    View<rx::IPropsRegistryBuilder> builder) {
  APMF_LOG(ColorBoxLogger, INFO) << "[ColorBox] registerProps";
  builder->stringPropWithDefault("color", "#444444");
  builder->doublePropWithDefault("opacity", 1.0);
  return builder->build();
}

// Receive prop values from JS (initial mount + every update). Only the changed
// keys are present, so iterate whatever was sent.
void ColorBoxViewManager::updateProps(View<vw::IView> view, View<rx::IJsObject> props) {
  for (auto const& key : props->getAllKeys()) {
    if (key == "color") {
      const std::string hex(props->getString("color").c_str());
      view->setBackgroundColor(parseColor(hex));
      APMF_LOG(ColorBoxLogger, INFO) << "[ColorBox] updateProps color=" << hex;
    } else if (key == "opacity") {
      const float o = static_cast<float>(props->getDouble("opacity"));
      view->setOpacity(o);
      APMF_LOG(ColorBoxLogger, INFO) << "[ColorBox] updateProps opacity=" << o;
    }
  }
}

// Receive the React-computed (flexbox) layout and apply it to the native view.
void ColorBoxViewManager::updateLayout(View<vw::IView> view, View<rx::ILayout> layout) {
  const gfx::Size size = layout->getContentFrame()->getSize();
  view->setSize(size);
  APMF_LOG(ColorBoxLogger, INFO) << "[ColorBox] updateLayout " << size.width << "x" << size.height;
}

}  // namespace customview
