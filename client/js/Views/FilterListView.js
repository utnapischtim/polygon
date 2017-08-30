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
      this.collection.forEach((model) => {
        let view = new FilterView({model});
        this.$el.append(view.render().el);
      });
    }
  });

  return FilterListView;
});
