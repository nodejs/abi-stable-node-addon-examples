#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <assert.h>

#include "napi_itc_queue.h"

// Helper
#define NAPI_CALL(env, call)                                                   \
  {                                                                            \
    napi_status status = call;                                                 \
    assert(status == napi_ok);                                                 \
  }

using namespace std;

struct MyData {
  string type;
  string payload;

  MyData(string _type, string _payload)
    : type(_type),
      payload(_payload) {}
};

template <class T>
class AutoDelete
{
  public:
    AutoDelete (T * p = 0) : ptr_(p) {}
    ~AutoDelete () throw() { delete ptr_; }
  private:
    T *ptr_;
};

// This is the thread mocking any native thread producing data
static void threadRoutine(napi_itc_handle handle, string name) {
  // calling the thread safe function
  napi_itc_send(handle, new MyData("start", name));

  for(int i = 0; i < 5; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    napi_itc_send(handle, new MyData("data", name + "..." + to_string(i)));
  }

  napi_itc_send(handle, new MyData("end", name));
  napi_itc_complete(handle);
}

// this fonction is ran on the nodejs loop
static void consumer(napi_env env, napi_itc_handle handle, void* userdata, void* eventdata) {
  // cout << "in consumer\n";
  auto event = (MyData*)eventdata;
  AutoDelete<MyData> safe_del(event);

  napi_value func, argv[2], global;
  napi_get_global(env, &global);
  napi_get_reference_value(env, (napi_ref)userdata, &func);
  napi_create_string_utf8(env, event->type.c_str(), NAPI_AUTO_LENGTH, &argv[0]);
  napi_create_string_utf8(env, event->payload.c_str(), NAPI_AUTO_LENGTH, &argv[1]);
  napi_call_function(env, global, func, 2, argv, nullptr);
}

// this function is ran on the nodejs loop
static void complete(napi_env env, napi_itc_handle handle, napi_status exit_status, void* userdata) {
  // free up resources
  // cout << "in complete\n";
  napi_delete_reference(env, (napi_ref)userdata);
}


static napi_value CallEmit(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_ref func_ref;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

  char buf[200];
  NAPI_CALL(env, napi_get_value_string_utf8(env, argv[0], buf, sizeof buf, nullptr));
  NAPI_CALL(env, napi_create_reference(env, argv[1], 1, &func_ref));

  napi_itc_handle handle;
  NAPI_CALL(env, napi_itc_init(env, consumer, complete, (void*)func_ref, &handle));

  string thread_name(buf);
  thread t(threadRoutine, handle, thread_name);
  t.detach();
  return nullptr;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_value callEmit;
  NAPI_CALL(env, napi_create_function(env, "", 0, CallEmit, nullptr, &callEmit));
  NAPI_CALL(env, napi_set_named_property(env, exports, "callEmit", callEmit));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
