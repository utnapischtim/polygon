define([
  'log',
  'underscore',
  'backbone'
],

function (l, _, bb) {

  let waiting_models = {};

  let createWebSocket = function (opts) {
    let socket = new window.WebSocket(opts.url, 'protocolOne');

    _.each(opts.events, (func, event) => {
      socket[event] = func;
    });

    return socket;
  };

  let onmessage = function (event) {
    let data = JSON.parse(event.data),
        syncId = _.keys(data)[0];

    if (waiting_models.hasOwnProperty(syncId)) {
      // set for models
      // add for collection
      // func only one call to server
      let func = "", trigger = false;
      if (waiting_models[syncId] instanceof bb.Model) {
        func = 'set';
        trigger = true;
      } else if (waiting_models[syncId] instanceof bb.Collection) {
        func = 'add';
        trigger = true;
      } else
        func = 'func';

      waiting_models[syncId][func](data[syncId], {parse: true});

      if (trigger)
        waiting_models[syncId].trigger('sync');
    } else
      l.warn("bb.sync onmessage -", syncId, "- is not a valid syncId, whole data: ", data);
  };

  let onerror = function () {
    window.alert("server is not running");
  };

  let defaultSocketConfig = {
    url: 'ws://127.0.0.1:9000',
    events: {onmessage, onerror}
  };

  let sockets = {
    default: createWebSocket(defaultSocketConfig)
  };

  // options must have date attribute
  // with {data, option}
  bb.sync = function (method, model, options) {
    // model has to be added to waiting_models only once, because it is never removed!
    if (!model.hasOwnProperty('syncId') || model.syncId === undefined) {
      model.syncId = _.uniqueId('sync');
      waiting_models[model.syncId] = model;
    }

    options['syncId'] = model.syncId;

    let socketId = 'default';

    if (model.hasOwnProperty("socketId")) {
      if (sockets.hasOwnProperty(model.socketId))
        socketId = model.socketId;
      else {
        l.error(`socket with name ${model.socketId} was not created yet`);
        return;
      }
    }

    sockets[socketId].send(JSON.stringify(options));
  };

  bb.createWebsocket = function (socketId, url, events) {
    events = _.extend({onmessage, onerror}, events);
    sockets[socketId] = createWebSocket({url, events});
  };

  bb.closeWebsocket = function (socketId) {
    sockets[socketId].close();
    delete sockets[socketId];
  };

});
