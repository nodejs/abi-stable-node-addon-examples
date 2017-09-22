#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <napi.h>
#include <uv.h>

class MyObject : public Napi::ObjectWrap<MyObject> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  MyObject(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor;

  Napi::Value GetValue(const Napi::CallbackInfo& info);
  Napi::Value PlusOne(const Napi::CallbackInfo& info);
  Napi::Value Multiply(const Napi::CallbackInfo& info);

  double value_;  

};

#endif
