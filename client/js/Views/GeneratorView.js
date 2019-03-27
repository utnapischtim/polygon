define([
  'log',
  'jquery',
  'backbone',
  'hbs!tpl/t-generator'
],
function (l, $, bb, tGenerator) {

  let GeneratorView = bb.View.extend({
    tagName: 'div',
    className: 'pl-generator',

    events: {
      'click input': 'chooseGenerator'
    },

    render: function () {
      this.$el.append(tGenerator(this.model.toJSON()));
      return this;
    },

    chooseGenerator: function () {
      this.model.collection.cleanActive();
      this.model.set("active", true);
    }
  });

  return GeneratorView;
});
