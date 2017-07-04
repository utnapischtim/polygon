define([
  'log',
  'jquery',
  'backbone',
  'd3'
],
function (l, $, bb, d3) {
  let scaleX = d3.scaleLinear()
        .domain([-30, 30])
        .range([0, 600]);

  let scaleY = d3.scaleLinear()
        .domain([0, 50])
        .range([500, 0]);


  let VisualizationView = bb.View.extend({
    tagName: 'div',
    className: 'pl-visualization',

    initialize: function () {
      this.listenTo(this.collection, 'sync', this.renderPolygon);
    },

    render: function () {
      this.vis = d3.select(this.el).append("svg")
        .attr("width", 1000)
        .attr("height", 667);

      return this;
    },

    renderPolygon: function () {
      let points = this.collection.toPolygon(scaleX, scaleY);

      this.vis.selectAll("polyline")
        .data([points])
       .enter()
        .append("polyline")
        .attr("points", d => d)
        .attr("fill", "white")
        .attr("stroke", "black")
        .attr("stroke-width", 2);
    }
  });

  return VisualizationView;
});
