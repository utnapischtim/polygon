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

    events: {
      'click .pl-filter-input-checkbox': 'activateFilter',
      'blur .pl-filter-input-number': 'blur'
    },

    render: function () {
      this.$el.append(tFilter(this.model.toJSON()));
      return this;
    },

    activateFilter: function () {
      let active = !this.model.get("active");
      this.model.set("active", active);
    },

    blur: function (e) {
      // set it allways as a precaution to true
      this.$el.find("input[type='checkbox']").prop("checked", true);
      this.model.set("active", true);
      this.model.set("val", parseInt(e.currentTarget.value));
    }
  });

  return FilterView;
});
