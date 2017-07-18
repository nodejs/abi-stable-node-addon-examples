#define NAPI_DISABLE_CPP_EXCEPTIONS

#include <napi.h>

void RunCallback(const Napi::CallbackInfo& info) {
  Napi::Function cb = info[0].As<Napi::Function>();
  const unsigned argc = 1;
  Napi::Value argv[argc] = { Napi::String::New(info.Env(), "hello world") };
  cb.MakeCallback(info.Env().Global(), { argv[0] });
}

void Init(Napi::Env env, Napi::Object exports, Napi::Object module) {
  module.DefineProperty(Napi::PropertyDescriptor::Function("exports", RunCallback));
}

NODE_API_MODULE(addon, Init)
