/*
 * ColorBoxViewManager — a minimal native view manager that demonstrates the
 * three things every custom Vega Fabric component needs:
 *
 *   IViewManager   :: makeView()      -> create the native view
 *   IPropsDelegate :: registerProps() -> declare the props (schema)
 *                     updateProps()   -> receive prop values from JS
 *   ILayoutDelegate:: updateLayout()  -> receive the React-computed layout (size)
 *
 * The view is a plain container whose background color + opacity come from props,
 * and whose size comes from the React (flexbox) layout.
 */
#pragma once

#include <apmf/apmf_base.h>
#include <apmf/ptr.h>

#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IJsObject.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ILayout.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ILayoutDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsRegistry.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsRegistryBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IViewManager.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IView.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IViewModule.h>

namespace customview {

using apmf::ApmfBase;
using apmf::Ptr;
using apmf::View;
namespace rx = apmf::iface::com::amazon::kepler::uitoolkit::react;
namespace vw = apmf::iface::com::amazon::kepler::uitoolkit::view;

struct ColorBoxViewManager : ApmfBase<ColorBoxViewManager,
                                      rx::IViewManager,
                                      rx::IPropsDelegate,
                                      rx::ILayoutDelegate> {
  explicit ColorBoxViewManager(View<vw::IViewModule> viewModule);

  // IViewManager
  Ptr<vw::IView> makeView();

  // IPropsDelegate
  Ptr<rx::IPropsRegistry> registerProps(View<rx::IPropsRegistryBuilder> builder);
  void updateProps(View<vw::IView> view, View<rx::IJsObject> props);

  // ILayoutDelegate
  void updateLayout(View<vw::IView> view, View<rx::ILayout> layout);

 private:
  Ptr<vw::IViewModule> viewModule_;
};

}  // namespace customview
