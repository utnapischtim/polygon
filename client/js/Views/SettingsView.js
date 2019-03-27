define([
  'log',
  'backbone',
  'js/Collections/CommonSettingList',
  'js/Collections/FilterList',
  'js/Collections/GeneratorList',
  'js/Views/CommonSettingListView',
  'js/Views/FilterListView',
  'js/Views/GeneratorListView',
  'js/Views/MenuView',
  'hbs!tpl/t-settings'
],
function (l, bb,
          CommonSettingList, FilterList, GeneratorList,                        // Collections
          CommonSettingListView, FilterListView, GeneratorListView, MenuView,  // Views
          tSettings) {

  let SettingsView = bb.View.extend({
    tagName: 'div',
    className: 'pl-settings',

    initialize: function (attrs) {
      this.pointList = attrs.pointList;

      let filterList = new FilterList(),
          generatorList = new GeneratorList(),
          commonSettingList = new CommonSettingList();

      this.views = [
        new FilterListView({collection: filterList}),
        new GeneratorListView({collection: generatorList}),
        new CommonSettingListView({collection: commonSettingList}),
        new MenuView({filterList, generatorList, commonSettingList, pointList: attrs.pointList})
      ];
    },

    render: function () {
      this.$el.append(tSettings());

      let settingBoxes = this.$el.find(".pl-settings-boxes");

      this.views.forEach((view) => {
        settingBoxes.append(view.render().el);
      });

      return this;
    }
  });

  return SettingsView;
});
