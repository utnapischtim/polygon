define([
  'backbone',
  'hbs!tpl/t-common-setting'
],
function (bb, tCommonSetting) {

  let CommonSettingView = bb.View.extend({
    tagName: 'div',
    className: 'pl-common-setting',

    events: {
      'click .pl-common-setting-input-checkbox': 'activate',
      'blur .pl-common-setting-input-number': 'blur',
      'blur .pl-common-setting-input-text': 'blur'
    },

    render: function () {
      this.$el.append(tCommonSetting(this.model.toJSON()));
      return this;
    },

    activate: function () {
      let active = !this.model.get("active");
      this.model.set("active", active);
    },

    blur: function (e) {
      // set it allways as a precaution to true
      this.$el.find("input[type='checkbox']").prop("checked", true);
      this.model.set("active", true);

      this.model.set("val", e.currentTarget.value);
    }
  });

  return CommonSettingView;
});
