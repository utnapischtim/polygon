require.config({
  scriptType: 'text/javascript;version=1.8',
  baseUrl: '.',
  paths: {
    jquery: 'lib/jquery/jquery',
    underscore: 'lib/underscore/underscore',
    backbone: 'lib/backbone/backbone',
    handlebars: 'lib/handlebars/dist/handlebars',
    log: 'lib/loglevel/lib/loglevel',
    hbs: 'lib/require-handlebars-plugin/hbs'
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
