define([
  'log',
  'jquery',
  'backbone',
  'js/Views/FilterView',
  'hbs!tpl/t-filter-list'
],
function (l, $, bb, FilterView, tFilterList) {

  let FilterListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-filter-list',

    initialize: function () {
      this.listenTo(this.collection, 'sync', this.addViews);

      // to be sure that this is done before addViews
      this.$el.append(tFilterList());

      this.collection.fetch();
    },

    render: function () {
      return this;
    },

    addViews: function () {
      let localListView = this.$el.find(".pl-filter-list-local"),
          globalListView = this.$el.find(".pl-filter-list-global");

      let localFilters = this.collection.filter({art: "local"}),
          globalFilters = this.collection.filter({art: "global"});

      localFilters.forEach((model) => {
        let view = new FilterView({model});
        localListView.append(view.render().el);
      });

      globalFilters.forEach((model) => {
        let view = new FilterView({model});
        globalListView.append(view.render().el);
      });

    }
  });

  return FilterListView;
});
