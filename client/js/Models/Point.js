define([
  'backbone'
], function (bb) {
  let Point = bb.Model.extend({
    scale: function (scaleX, scaleY) {
      const x = scaleX(this.get("x")),
            y = scaleY(this.get("y"));

      return `${x} ${y}`;
    }
  });

  return Point;
});
