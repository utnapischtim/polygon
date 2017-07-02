define([
  'backbone',
  'js/Models/Generator'
],
function (bb, Generator) {
  let GeneratorList = bb.Collection.extend({
    model: Generator,

    fetch: function () {
      bb.Collection.prototype.fetch.call(this, {job: 'generator'});
    }
  });

  return GeneratorList;
});
