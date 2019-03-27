define([
  'log',
  'jquery',
  'backbone',
  'js/Views/GeneratorView',
  'hbs!tpl/t-generator-list'
],
function (l, $, bb, GeneratorView, tGeneratorList) {

  let GeneratorListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-generator-list',

    initialize: function () {
      this.listenTo(this.collection, 'sync', this.addViews);

      // should be in render, BUT to make realy sure that it is done
      // before addViews it has to be here
      this.$el.append(tGeneratorList());

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
