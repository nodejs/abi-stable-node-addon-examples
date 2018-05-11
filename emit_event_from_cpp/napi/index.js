"use strict";

const EventEmitter = require("events").EventEmitter;
const addon = require("bindings")("emit_from_cpp");

const emitter = new EventEmitter();
let handle;

emitter.on("start", () => {
  console.log("### START ...");
});
emitter.on("data", evt => {
  console.log("event from addon >>", evt);
});

emitter.on("end", () => {
  console.log("### END ###");
  clearInterval(handle);
});

addon.callEmit(emitter.emit.bind(emitter));
handle = setInterval(() => console.log("interval in JS"), 1000);
