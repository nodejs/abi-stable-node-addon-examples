#include <napi.h>
#include <uv.h>

void RunCallback(const Napi::CallbackInfo& info) {
  Napi::Function cb = info[0].As<Napi::Function>();
  const unsigned argc = 1;
  Napi::Value argv[argc] = { Napi::String::New(env, "hello world") };
  cb.MakeCallback(Napi::GetCurrentContext()->Global(), argc, argv);
}

void Init(Napi::Object exports, Napi::Object module) {
  Napi::SetMethod(module, "exports", RunCallback);
}

NODE_API_MODULE(addon, Init)
