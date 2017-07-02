define([
  'log',
  'jquery',
  'backbone',
  'js/Views/FilterListView',
  'js/Views/GeneratorListView'
],
function (l, $, bb, FilterListView, GeneratorListView) {

  let SettingsView = bb.View.extend({
    tagName: 'div',
    className: 'pl-settings',

    render: function () {
      let filterListView = new FilterListView(),
          generatorListView = new GeneratorListView();

      this.$el.append(filterListView.render().el);
      this.$el.append(generatorListView.render().el);

      return this;
    }
  });

  return SettingsView;
});
