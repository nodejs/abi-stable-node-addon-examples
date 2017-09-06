#include "myobject.h"

Napi::FunctionReference MyObject::constructor;

void MyObject::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function t = DefineClass(env, "MyObject", {
    InstanceMethod("plusOne", &MyObject::PlusOne),
    InstanceMethod("getValue", &MyObject::GetValue),
    InstanceMethod("multiply", &MyObject::Multiply)
  });

  constructor = Napi::Persistent(t);
  constructor.SuppressDestruct();

  exports.Set("MyObject", t);
}

MyObject::MyObject(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MyObject>(info)  {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  int length = info.Length();

  if (length <= 0 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
  }

  Napi::Number value = info[0].As<Napi::Number>();
  info.This().As<Napi::Object>().DefineProperty(Napi::PropertyDescriptor::Value("value", value, napi_default));
  this->value_ = value.DoubleValue();
}

Napi::Value MyObject::GetValue(const Napi::CallbackInfo& info) {
  double num = this->value_;

  return Napi::Number::New(info.Env(), num);
}

Napi::Value MyObject::PlusOne(const Napi::CallbackInfo& info) {
  this->value_ = this->value_ + 1;

  return MyObject::GetValue(info);
}

Napi::Value MyObject::Multiply(const Napi::CallbackInfo& info) {
  Napi::Number multiple;
  if (info.Length() <= 0 || !info[0].IsNumber()) {
    multiple = Napi::Number::New(info.Env(), 1);
  } else {
    multiple = info[0].As<Napi::Number>();
  }

  Napi::Object obj = constructor.New({ Napi::Number::New(info.Env(), MyObject::GetValue(info).As<Napi::Number>().DoubleValue() * multiple.DoubleValue()) });
  MyObject* myobj = MyObject::Unwrap(obj);

  return obj;
}
