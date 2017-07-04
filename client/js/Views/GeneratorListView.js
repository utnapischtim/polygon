define([
  'log',
  'jquery',
  'backbone',
  'js/Collections/GeneratorList',
  'js/Views/GeneratorView',
  'hbs!tpl/t-generator-list'
],
function (l, $, bb, GeneratorList, GeneratorView, tGeneratorList) {

  let GeneratorListView = bb.View.extend({
    tagName: 'div',
    className: 'pl-generator-list',

    initialize: function () {
      this.collection = new GeneratorList();

      this.listenTo(this.collection, 'sync', this.addViews);

      this.collection.fetch();
    },

    render: function () {
      this.$el.append(tGeneratorList());
      return this;
    },

    addViews: function () {
      this.collection.each((model) => {
        let view = new GeneratorView({model});
        this.$el.append(view.render().el);
      });
    },

    getChosenGenerator: function () {
      return this.collection.filter(model => model.get("active"))[0];
    }
  });

  return GeneratorListView;
});
