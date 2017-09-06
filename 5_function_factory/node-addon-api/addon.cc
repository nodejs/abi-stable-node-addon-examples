#include <napi.h>
#include <uv.h>

Napi::String MyFunction(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "hello world");
}

Napi::Function CreateFunction(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Function fn = Napi::Function::New(env, MyFunction, "theFunction");

  return fn;
}

void Init(Napi::Env env, Napi::Object exports, Napi::Object module) {
  module.Set(Napi::String::New(env, "exports"),                                    \
             Napi::Function::New(env, CreateFunction));
}

NODE_API_MODULE(addon, Init)
