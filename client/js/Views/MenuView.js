define([
  'underscore',
  'backbone',
  'hbs!tpl/t-menu'
],
function (_, bb, tMenu) {

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

      if (!(chosenGenerator instanceof Object))
        alert("a generator should be choosen");

      else if (_.filter(commonSettings, o => o.get("name") == "nodes").length == 0)
        alert("nodes should have a value and should be activated");

      else if (_.filter(commonSettings, o => o.get("name") == "sampling grid").length == 0)
        alert("sampling grid should have a value and should be activated");

      else {
        this.pointList.width = this.commonSettingList.getWidth();
        this.pointList.height = this.commonSettingList.getHeight();

        this.pointList.fetch({activatedFilters, chosenGenerator, commonSettings});
      }
    }
  });

  return MenuView;
});
