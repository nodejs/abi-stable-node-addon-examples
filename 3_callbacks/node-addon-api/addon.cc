#include <napi.h>
#include <uv.h>

void RunCallback(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  Napi::Function cb = info[0].As<Napi::Function>();
  cb.MakeCallback(env.Global(), { Napi::String::New(env, "hello world") });
}

void Init(Napi::Env env, Napi::Object exports, Napi::Object module) {
  module.Set(Napi::String::New(env, "exports"),                                    \
              Napi::Function::New(env, RunCallback));
}

NODE_API_MODULE(addon, Init)
