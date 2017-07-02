define([
  'log',
  'jquery',
  'backbone',
  'js/Collections/FilterList',
  'js/Views/FilterView'
],
function (l, $, bb, FilterList, FilterView) {

  let FilterListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-filter-list',

    initialize: function () {
      this.collection = new FilterList();

      this.listenTo(this.collection, 'sync', this.addViews);

      this.collection.fetch();
    },

    render: function () {
      return this;
    },

    addViews: function () {
      this.collection.each((model) => {
        let view = new FilterView({model});
        this.$el.append(view.render().el);
      });

    }
  });

  return FilterListView;
});
