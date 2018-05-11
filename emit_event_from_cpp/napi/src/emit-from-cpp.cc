#include <node_api.h>
#include <uv.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <assert.h>

// Helper
#define NAPI_CALL(env, call)                                                   \
  {                                                                            \
    napi_status status = call;                                                 \
    assert(status == napi_ok);                                                 \
  }

using namespace std;
static uv_sem_t semaphore;
static uv_async_t uv_async;
static napi_env cached_env = nullptr; // !!use only within the uvCallback!!
static napi_ref func_ref;
static bool stop = false;
static int iteration;

static void uvFinalize(uv_handle_t* handle) {
  cout << "Finalize";
}

static void uvCallback(uv_async_t* handle) {
  // cout << "in uv loop\n";
  iteration++;

  napi_env env = cached_env;
  napi_async_context async_context;
  napi_handle_scope scope;
  napi_callback_scope callback_scope;
  napi_value argv[2], global, func, resource_name, resource_object;

  /* setup for the callback */
  NAPI_CALL(env, napi_open_handle_scope(env, &scope)); // create a new scope in this callback
  NAPI_CALL(env, napi_create_object(env, &resource_object));
  NAPI_CALL(env, napi_create_string_utf8(env, "resource_name", NAPI_AUTO_LENGTH, &resource_name));
  NAPI_CALL(env, napi_async_init(env, resource_object, resource_name, &async_context));
  NAPI_CALL(env, napi_open_callback_scope(env, resource_object, async_context, &callback_scope));
  /*end setup*/

  NAPI_CALL(env, napi_get_global(env, &global));
  NAPI_CALL(env, napi_get_reference_value(env, func_ref, &func));
  if (!stop) {
    NAPI_CALL(env, napi_create_string_utf8(env, "data", NAPI_AUTO_LENGTH, &argv[0]));
    char buffer[30];
    snprintf(buffer, sizeof buffer, "data ... %d", iteration);
    NAPI_CALL(env, napi_create_string_utf8(env, buffer, NAPI_AUTO_LENGTH, &argv[1]));
    NAPI_CALL(env, napi_make_callback(env, async_context, global, func, 2, argv, nullptr));
  } else {
    NAPI_CALL(env, napi_create_string_utf8(env, "end", NAPI_AUTO_LENGTH, &argv[0]));
    NAPI_CALL(env, napi_make_callback(env, async_context, global, func, 1, argv, nullptr));
    NAPI_CALL(env, napi_delete_reference(env, func_ref));
    uv_close((uv_handle_t*)(&uv_async), uvFinalize);
  }

  /* tear down*/
  NAPI_CALL(env, napi_close_callback_scope(env, callback_scope));
  NAPI_CALL(env, napi_async_destroy(env, async_context));
  NAPI_CALL(env, napi_close_handle_scope(env, scope));

  uv_sem_post(&semaphore); // signal work is done
}

static void threadRoutine() {
  for(int i = 0; i < 5; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    uv_async_send(&uv_async);
    uv_sem_wait(&semaphore);
    // have to wait for work, because multiple calls to uv_async_send are ignored if the uvCallback is not done being run
  }
  stop = true;
  uv_async_send(&uv_async);
  uv_sem_wait(&semaphore);
  uv_sem_destroy(&semaphore);
}


static napi_status initThread(napi_env env) {
  uv_sem_init(&semaphore, 0);
  iteration = 0;
  thread t(threadRoutine);
  t.detach();

  uv_loop_t* loop;
  NAPI_CALL(env, napi_get_uv_event_loop(env, &loop));
  assert(uv_async_init(loop, &uv_async, uvCallback) == 0);
  cached_env = env;
  return napi_ok;
}


static napi_value CallEmit(napi_env env, napi_callback_info info) {
  // this function is not thread safe, be sure to handle your own concurrency, this is just an example
  size_t argc = 1;
  napi_value func, event, global;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &func, nullptr, nullptr));
  NAPI_CALL(env, napi_get_global(env, &global));
  NAPI_CALL(env, napi_create_string_utf8(env, "start", NAPI_AUTO_LENGTH, &event));
  NAPI_CALL(env, napi_call_function(env, global, func, 1, &event, nullptr));
  NAPI_CALL(env, napi_create_reference(env, func, 1, &func_ref));

  NAPI_CALL(env, initThread(env));
  return nullptr;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_value callEmit;
  NAPI_CALL(env, napi_create_function(env, "", 0, CallEmit, nullptr, &callEmit));
  NAPI_CALL(env, napi_set_named_property(env, exports, "callEmit", callEmit))
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
