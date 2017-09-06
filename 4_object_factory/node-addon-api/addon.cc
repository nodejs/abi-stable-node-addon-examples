#include <napi.h>
#include <uv.h>

Napi::Object CreateObject(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "msg"), info[0].ToString());

  return obj;
}

void Init(Napi::Env env, Napi::Object exports, Napi::Object module) {
  module.Set(Napi::String::New(env, "exports"),
             Napi::Function::New(env, CreateObject));
}

NODE_API_MODULE(addon, Init)
