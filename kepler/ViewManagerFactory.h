/*
 * Minimal custom Fabric (IView) component for Amazon Vega / Kepler.
 *
 * The factory is the entry point: KeplerScript calls makeViewManager(name) once
 * per registered component name, then drives the returned IViewManager.
 */
#pragma once

#include <apmf/apmf_base.h>
#include <apmf/process.h>
#include <apmf/ptr.h>
#include <apmf/string_view.h>

#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IViewManager.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IViewManagerFactory.h>

namespace customview {

using apmf::ApmfBase;
using apmf::Ptr;
using apmf::StringView;
namespace rx = apmf::iface::com::amazon::kepler::uitoolkit::react;

struct ViewManagerFactory : ApmfBase<ViewManagerFactory, rx::IViewManagerFactory> {
  Ptr<rx::IViewManager> makeViewManager(StringView name);
};

}  // namespace customview
