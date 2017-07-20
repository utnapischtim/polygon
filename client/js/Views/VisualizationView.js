define([
  'log',
  'jquery',
  'backbone',
  'd3'
],
function (l, $, bb, d3) {
  // let scaleX = d3.scaleLinear()
  //       .domain([-30, 30])
  //       .range([0, 600]);

  // let scaleY = d3.scaleLinear()
  //       .domain([0, 50])
  //       .range([500, 0]);

  const width = 1500, height = 800;



  let VisualizationView = bb.View.extend({
    tagName: 'div',
    className: 'pl-visualization',

    initialize: function () {
      this.listenTo(this.collection, 'sync', this.renderPolygon);
    },

    render: function () {
      return this;
    },

    renderPolygon: function () {
      // TODO:
      // maybe there exists a nicer solution to do this, for moment it is enough
      let scaleX = d3.scaleLinear().domain([0, this.collection.width]).range([0, this.collection.width]),
          scaleY = d3.scaleLinear().domain([0, this.collection.height]).range([0, this.collection.height]);

      let points = this.collection.toPolygon(scaleX, scaleY);

      // TODO
      // there should be a possibility to do this with the d3 way to do it
      if (this.vis)
        this.vis.selectAll("svg").remove();

      this.vis = d3.select(this.el).append("svg")
        .attr("width", this.collection.width)
        .attr("height", this.collection.height);

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
