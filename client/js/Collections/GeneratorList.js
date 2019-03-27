define([
  'backbone',
  'js/Models/Generator'
],
function (bb, Generator) {
  let GeneratorList = bb.Collection.extend({
    model: Generator,

    fetch: function () {
      bb.Collection.prototype.fetch.call(this, {job: 'generator'});
    },

    cleanActive: function () {
      this.each(m => m.set("active", false));
    },

    getChosenGenerator: function () {
      return this.filter(model => model.get("active"))[0];
    }
  });

  return GeneratorList;
});
