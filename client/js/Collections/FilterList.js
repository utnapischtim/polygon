define([
  'backbone',
  'js/Models/Filter'
],
function (bb, Filter) {
  let FilterList = bb.Collection.extend({
    model: Filter,

    fetch: function () {
      bb.Collection.prototype.fetch.call(this, {job: 'filter'});
    },

    getActivatedFilters: function () {
      return this.filter(model => model.get("active"));
    }
  });

  return FilterList;
});
