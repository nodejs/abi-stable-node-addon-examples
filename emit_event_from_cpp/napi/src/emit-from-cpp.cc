#include <node_api.h>
#include <uv.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <assert.h>

#include "napi_itc_queue.h"
#include "utils.h"

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

// This is the thread mocking any native thread producing data
static void threadRoutine(napi_itc_handle handle) {
  // calling the thread safe function
  napi_itc_send(handle, new MyData("start", ""));

  for(int i = 0; i < 5; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    napi_itc_send(handle, new MyData("data", "..." + to_string(i)));
  }

  napi_itc_send(handle, new MyData("end", ""));
  napi_itc_complete(handle);
}

static void consumer(napi_env env, napi_itc_handle handle, void* userdata, void* eventdata) {
  cout << "in consumer\n";
  auto event = (MyData*)eventdata;
  AutoDelete<MyData> safe_del(event);

  napi_value func, argv[2], global;
  napi_get_global(env, &global);
  napi_get_reference_value(env, (napi_ref)userdata, &func);
  napi_create_string_utf8(env, event->type.c_str(), NAPI_AUTO_LENGTH, &argv[0]);
  napi_create_string_utf8(env, event->payload.c_str(), NAPI_AUTO_LENGTH, &argv[1]);
  napi_call_function(env, global, func, 2, argv, nullptr);
}

static void complete(napi_itc_handle handle, void* userdata) {
  // free up resources
}


static napi_value CallEmit(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value func;
  napi_ref func_ref;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &func, nullptr, nullptr));
  NAPI_CALL(env, napi_create_reference(env, func, 1, &func_ref));

  napi_itc_handle handle;
  NAPI_CALL(env, napi_itc_init(env, consumer, complete, (void*)func_ref, &handle));

  thread t(threadRoutine, handle);
  t.detach();
  return nullptr;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_value callEmit;
  NAPI_CALL(env, napi_create_function(env, "", 0, CallEmit, nullptr, &callEmit));
  NAPI_CALL(env, napi_set_named_property(env, exports, "callEmit", callEmit))
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
