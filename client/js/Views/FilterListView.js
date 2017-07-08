define([
  'log',
  'jquery',
  'backbone',
  'js/Collections/FilterList',
  'js/Views/FilterView',
  'hbs!tpl/t-filter-list'
],
function (l, $, bb, FilterList, FilterView, tFilterList) {

  let FilterListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-filter-list',

    initialize: function () {
      this.collection = new FilterList();

      this.listenTo(this.collection, 'sync', this.addViews);

      this.collection.fetch();
    },

    render: function () {
      this.$el.append(tFilterList());
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

    },

    getActivatedFilters: function () {
      return this.collection.filter(model => model.get("active"));
    }
  });

  return FilterListView;
});
