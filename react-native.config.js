// Autolink config for the custom native UI component.
//
// `uiComponentName` is the in-process component path the app provides; it must
// match APMF_COMPONENT(...) in ViewManagerFactory.cpp and the OUTPUT_NAME in
// CMakeLists.txt. `components` lists the JS element names (uiViewClassName from
// the spec) this component provides. No `libraryName` (that's for turbo modules;
// supplying both is rejected). This autolink entry is what writes the component
// into the app's generated keplerscript-app-config.json so RN can resolve it.
module.exports = {
  dependency: {
    platforms: {
      kepler: {
        autolink: {
          'com.example.customview': {
            uiComponentName: '/com.example.customview',
            provider: 'application',
            linkDynamic: true,
            components: ['ColorBox'],
            turbomodules: [],
          },
        },
      },
    },
  },
};
