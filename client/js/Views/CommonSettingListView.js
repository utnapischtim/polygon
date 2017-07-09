define([
  'backbone',
  'js/Collections/CommonSettingList',
  'js/Views/CommonSettingView',
  'hbs!tpl/t-common-setting-list'
],
function (bb, CommonSettingList, CommonSettingView, tCommonSettingList) {

  let CommonSettingListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-common-setting-list',

    initialize: function () {
      this.collection = new CommonSettingList();

      this.listenTo(this.collection, 'sync', this.addViews);

      this.collection.fetch();
    },

    render: function () {
      this.$el.append(tCommonSettingList());
      return this;
    },

    addViews: function () {
      this.collection.each((model) => {
        let view = new CommonSettingView({model});
        this.$el.append(view.render().el);
      });
    },

    getCommonSettings: function () {
      return this.collection.filter(model => model.get("active"));
    },

    getWidth: function () {
      let common_setting_area = this.collection.filter({name: 'area'})[0];
      return parseInt(common_setting_area.get("val").split("x")[0]);
    },

    getHeight: function () {
      let common_setting_area = this.collection.filter({name: 'area'})[0];
      return parseInt(common_setting_area.get("val").split("x")[1]);
    }
  });

  return CommonSettingListView;
});
