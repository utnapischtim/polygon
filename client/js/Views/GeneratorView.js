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

    render: function () {
      this.$el.append(tGenerator(this.model.toJSON()));
      return this;
    }
  });

  return GeneratorView;
});
