"use strict";

const EventEmitter = require("events").EventEmitter;
const addon = require("bindings")("emit_from_cpp");

const emitter = new EventEmitter();

emitter.on("start", () => {
  console.log("### START ...");
});
emitter.on("data", evt => {
  console.log("event from addon >>", evt);
});

emitter.on("end", () => {
  console.log("### END ###");
});

setTimeout(() => {
  addon.callEmit(emitter.emit.bind(emitter));
}, 3000);
addon.callEmit(emitter.emit.bind(emitter));
