define([
  'backbone'
], function (bb) {
  let Point = bb.Model.extend({
    scale: function (scaleX, scaleY, height) {
      const x = scaleX(this.get("x")),
            y = height - scaleY(this.get("y"));

      return `${x} ${y}`;
    }
  });

  return Point;
});
