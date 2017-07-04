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
      this.collection.each((model) => {
        let view = new FilterView({model});
        this.$el.append(view.render().el);
      });

    },

    getActivatedFilters: function () {
      return this.collection.filter(model => model.get("active"));
    }
  });

  return FilterListView;
});
