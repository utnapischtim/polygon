define([
  'log',
  'jquery',
  'backbone',
  'hbs!tpl/t-filter'
],
function (l, $, bb, tFilter) {

  let FilterView = bb.View.extend({
    tagName: 'div',
    className: 'pl-filter',

    render: function () {
      this.$el.append(tFilter(this.model.toJSON()));
      return this;
    }
  });

  return FilterView;
});
