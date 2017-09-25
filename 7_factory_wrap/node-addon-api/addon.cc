#include <napi.h>
#include "myobject.h"

Napi::Object CreateObject(const Napi::CallbackInfo& info) {
  return MyObject::NewInstance(info.Env(), info[0]);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  MyObject::Init(env, exports);
  return Napi::Function::New(env, CreateObject, "CreateObject");
}

NODE_API_MODULE(addon, InitAll)
