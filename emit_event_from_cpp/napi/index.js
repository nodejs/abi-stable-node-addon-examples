"use strict";

const EventEmitter = require("events").EventEmitter;
const addon = require("bindings")("emit_from_cpp");

const emitter = new EventEmitter();

emitter.on("start", name => {
  console.log("### START ...", name);
});
emitter.on("data", evt => {
  console.log("event from addon >>", evt);
});

emitter.on("end", name => {
  console.log("### END ###", name);
});

addon.callEmit("producer1", emitter.emit.bind(emitter));
addon.callEmit("producer2", emitter.emit.bind(emitter));
