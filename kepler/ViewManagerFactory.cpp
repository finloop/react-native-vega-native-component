#include "ViewManagerFactory.h"

#include "ColorBoxViewManager.h"

#include <apmf/component.h>
#include <apmf/iface/com/amazon/apmf/InvalidArgumentError.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IViewModule.h>

namespace customview {

namespace vw = apmf::iface::com::amazon::kepler::uitoolkit::view;

Ptr<rx::IViewManager> ViewManagerFactory::makeViewManager(StringView name) {
  // The UI-Toolkit view module is a system in-process component; it builds the
  // concrete native views (makeView/makeTextView/...) our manager composes.
  auto viewModule = apmf::GetProcessObject()
                        ->getComponent("/com.amazon.kepler.uitoolkit.view")
                        .TryQueryInterface<vw::IViewModule>();

  // `name` matches uiViewClassName in the JS spec (and the autolink `components`).
  if (name == "ColorBox") {
    return Ptr<ColorBoxViewManager>::Make(viewModule);
  }
  throw apmf::InvalidArgumentError("CustomView: unknown component name");
}

}  // namespace customview

// Register the factory as the in-process component. The path MUST match
// `uiComponentName` in react-native.config.js and the OUTPUT_NAME in CMake.
APMF_COMPONENT("/com.example.customview", customview::ViewManagerFactory);
