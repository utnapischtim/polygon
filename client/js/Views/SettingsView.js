define([
  'log',
  'backbone',
  'js/Views/CommonSettingListView',
  'js/Views/FilterListView',
  'js/Views/GeneratorListView',
  'hbs!tpl/t-settings'
],
function (l, bb, CommonSettingListView, FilterListView, GeneratorListView, tSettings) {

  let SettingsView = bb.View.extend({
    tagName: 'div',
    className: 'pl-settings',

    events: {
      'click button': 'generate'
    },

    initialize: function (attrs) {
      this.pointList = attrs.pointList;

      this.filterListView = new FilterListView();
      this.generatorListView = new GeneratorListView();
      this.commonSettingListView = new CommonSettingListView();
    },

    render: function () {
      this.$el.append(tSettings());

      let settingBoxes = this.$el.find(".pl-settings-boxes");

      settingBoxes.append(this.filterListView.render().el);
      settingBoxes.append(this.generatorListView.render().el);
      settingBoxes.append(this.commonSettingListView.render().el);

      return this;
    },

    generate: function () {
      let activatedFilters = this.filterListView.getActivatedFilters(),
          chosenGenerator = this.generatorListView.getChosenGenerator(),
          commonSettings = this.commonSettingListView.getCommonSettings();

      this.pointList.width = this.commonSettingListView.getWidth();
      this.pointList.height = this.commonSettingListView.getHeight();

      this.pointList.fetch({activatedFilters, chosenGenerator, commonSettings});
    }
  });

  return SettingsView;
});
