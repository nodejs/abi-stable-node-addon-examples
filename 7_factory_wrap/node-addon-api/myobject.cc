#include <napi.h>
#include <uv.h>
#include "myobject.h"

using namespace Napi;

MyObject::MyObject(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyObject>(info) {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  this->counter_ = info[0].As<Napi::Number>().DoubleValue();
};

Napi::FunctionReference MyObject::constructor;

Napi::Object MyObject::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "MyObject", {
      InstanceMethod("plusOne", &MyObject::PlusOne)
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("MyObject", func);
  return exports;
}

Napi::Object MyObject::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({ arg });
  return obj;
}

Napi::Value MyObject::PlusOne(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  this->counter_ = this->counter_ + 1;

  return Napi::Number::New(env, this->counter_);
}
