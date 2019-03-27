require.config({
  scriptType: 'text/javascript;version=1.8',
  baseUrl: '.',
  paths: {
    jquery: 'lib/jquery',
    underscore: 'lib/underscore',
    backbone: 'lib/backbone',
    handlebars: 'lib/handlebars',
    log: 'lib/loglevel',
    d3: 'lib/d3',
    hbs: 'lib/hbs',
    json2: 'lib/json2'
  }
});

require([
  'log',
  'jquery',
  'underscore',
  'backbone',
  'handlebars',
  'js/Views/GuiView',

  // has to be loaded once! maybe find better place for it!
  'js/backboneWebsocket'
], function (l, $, _, bb, hbs, GuiView) {

  l.setLevel(l.levels.INFO);

  let guiView = new GuiView();
  $("body").append(guiView.render().el);
});
