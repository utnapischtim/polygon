define([
  'log',
  'jquery',
  'backbone',
  'js/Collections/GeneratorList',
  'js/Views/GeneratorView'
],
function (l, $, bb, GeneratorList, GeneratorView) {

  let GeneratorListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-generator-list',

    initialize: function () {
      this.collection = new GeneratorList();

      this.listenTo(this.collection, 'sync', this.addViews);

      this.collection.fetch();
    },

    render: function () {
      return this;
    },

    addViews: function () {
      this.collection.each((model) => {
        let view = new GeneratorView({model});
        this.$el.append(view.render().el);
      });
    }
  });

  return GeneratorListView;
});
