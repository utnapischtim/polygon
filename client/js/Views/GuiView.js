define([
  'log',
  'jquery',
  'backbone',
  'js/Views/VisualizationView',
  'js/Views/SettingsView'
],
function (l, $, bb, VisualizationView, SettingsView) {
  let GuiView = bb.View.extend({
    tagName: 'div',
    className: 'pl-gui',

    render: function () {
      let visualizationView = new VisualizationView(),
          settingsView = new SettingsView();

      this.$el.append(visualizationView.render().el);
      this.$el.append(settingsView.render().el);

      return this;
    }
  });

  return GuiView;
});
