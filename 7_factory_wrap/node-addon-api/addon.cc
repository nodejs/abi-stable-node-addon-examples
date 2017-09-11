#include <napi.h>
#include <uv.h>
#include "myobject.h"

Napi::Object CreateObject(const Napi::CallbackInfo& info) {
  return MyObject::NewInstance(info[0]);
}

void InitAll(Napi::Env env, Napi::Object exports, Napi::Object module) {
  Napi::HandleScope scope(env);

  MyObject::Init(env, exports);

  module.Set(Napi::String::New(env, "exports"),
             Napi::Function::New(env, CreateObject));
}

NODE_API_MODULE(addon, InitAll)
