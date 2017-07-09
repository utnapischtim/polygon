define([
  'backbone',
  'js/Views/CommonSettingView',
  'hbs!tpl/t-common-setting-list'
],
function (bb, CommonSettingView, tCommonSettingList) {

  let CommonSettingListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-common-setting-list',

    initialize: function () {
      this.listenTo(this.collection, 'sync', this.addViews);

      // this should be in render, but to be sure that this is done
      // before addViews it is here
      this.$el.append(tCommonSettingList());

      this.collection.fetch();
    },

    render: function () {
      return this;
    },

    addViews: function () {
      this.collection.each((model) => {
        let view = new CommonSettingView({model});
        this.$el.append(view.render().el);
      });
    }
  });

  return CommonSettingListView;
});
