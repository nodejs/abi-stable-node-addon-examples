#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <napi.h>
#include <uv.h>

class MyObject : public Napi::ObjectWrap<MyObject> {
 public:
  static void Init(Napi::Env env, Napi::Object exports);

  static Napi::FunctionReference constructor;
  MyObject(const Napi::CallbackInfo& info);

  Napi::Value GetValue(const Napi::CallbackInfo& info);
  Napi::Value PlusOne(const Napi::CallbackInfo& info);
  Napi::Value Multiply(const Napi::CallbackInfo& info);

  double value_;  

};

#endif
