define([
  'log',
  'jquery',
  'backbone'
],
function (l, $, bb) {

  let VisualizationView = bb.View.extend({
    tagName: 'div',
    className: 'pl-visualization',

    render: function () {
      return this;
    }
  });

  return VisualizationView;
});
