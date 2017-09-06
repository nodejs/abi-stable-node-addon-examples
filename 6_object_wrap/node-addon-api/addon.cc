#include <napi.h>
#include <uv.h>
#include "myobject.h"

void InitAll(Napi::Env env, Napi::Object exports, Napi::Object module) {
  MyObject::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)
