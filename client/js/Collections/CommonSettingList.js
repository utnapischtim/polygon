define([
  'backbone',
  'js/Models/CommonSetting'
],
function (bb, CommonSetting) {

  let CommonSettingList = bb.Collection.extend({
    model: CommonSetting,

    fetch: function () {
      bb.Collection.prototype.fetch.call(this, {job: 'common'});
    }
  });

  return CommonSettingList;
});
