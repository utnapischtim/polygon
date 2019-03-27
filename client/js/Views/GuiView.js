define([
  'log',
  'jquery',
  'backbone',
  'js/Collections/PointList',
  'js/Views/VisualizationView',
  'js/Views/SettingsView'
],
function (l, $, bb, PointList, VisualizationView, SettingsView) {
  let GuiView = bb.View.extend({
    tagName: 'div',
    className: 'pl-gui',

    render: function () {
      let pointList = new PointList(),

          visualizationView = new VisualizationView({collection: pointList}),
          settingsView = new SettingsView({pointList});

      this.$el.append(settingsView.render().el);
      this.$el.append(visualizationView.render().el);

      return this;
    }
  });

  return GuiView;
});
