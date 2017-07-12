define([
  'backbone',
  'js/Models/CommonSetting'
],
function (bb, CommonSetting) {

  let CommonSettingList = bb.Collection.extend({
    model: CommonSetting,

    fetch: function () {
      bb.Collection.prototype.fetch.call(this, {job: 'common'});
    },

    getCommonSettings: function () {
      return this.filter(model => model.get("active"));
    },

    getWidth: function () {
      let common_setting_sampling_grid = this.filter({name: 'sampling grid'})[0];
      return parseInt(common_setting_sampling_grid.get("val").split("x")[0]);
    },

    getHeight: function () {
      let common_setting_sampling_grid = this.filter({name: 'sampling grid'})[0];
      return parseInt(common_setting_sampling_grid.get("val").split("x")[1]);
    }
  });

  return CommonSettingList;
});
