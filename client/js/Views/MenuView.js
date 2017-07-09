define([
  'backbone',
  'hbs!tpl/t-menu'
],
function (bb, tMenu) {

  let MenuView = bb.View.extend({
    tagName: 'div',
    className: 'pl-menu',

    events: {
      'click .pl-menu-generate': 'generate'
    },

    initialize: function (attrs) {
      this.filterList = attrs.filterList;
      this.generatorList = attrs.generatorList;
      this.commonSettingList = attrs.commonSettingList;
      this.pointList = attrs.pointList;
    },

    render: function () {
      this.$el.append(tMenu());
      return this;
    },

    generate: function () {
      let activatedFilters = this.filterList.getActivatedFilters(),
          chosenGenerator = this.generatorList.getChosenGenerator(),
          commonSettings = this.commonSettingList.getCommonSettings();

      this.pointList.width = this.commonSettingList.getWidth();
      this.pointList.height = this.commonSettingList.getHeight();

      this.pointList.fetch({activatedFilters, chosenGenerator, commonSettings});
    }
  });

  return MenuView;
});
