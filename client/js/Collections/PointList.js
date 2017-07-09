define([
  'underscore',
  'backbone',
  'js/Models/Point'
],
function (_, bb, Point) {
  let PointList = bb.Collection.extend({
    model: Point,

    toPolygon: function (scaleX, scaleY) {
      return this.map(point => point.scale(scaleX, scaleY)).join(" ");
    },

    fetch: function (opts) {
      this.reset();
      bb.Collection.prototype.fetch.call(this, {opts, job: 'pointList'});
    }
  });

  return PointList;
});
