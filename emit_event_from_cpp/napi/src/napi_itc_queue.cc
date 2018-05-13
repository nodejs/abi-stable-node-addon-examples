#include "napi_itc_queue.h"
#include "thread_safe_queue.h"

static inline void maybe_throw_fatal(napi_env env, napi_status status, int line) {
  bool is_pending;
  napi_is_exception_pending(env, &is_pending);
  if (is_pending || status != napi_ok) {
    char* loc = new char[200];
    snprintf(loc, sizeof loc, "file: %s, line: %d\n", __FILE__, line);
    napi_fatal_error(
      loc,
      NAPI_AUTO_LENGTH,
      "error when setting callback scope",
      NAPI_AUTO_LENGTH);
  }
}

static inline bool maybe_throw_unhandled(napi_env env) {
  bool is_pending;
  napi_is_exception_pending(env, &is_pending);
  if (is_pending) {
    napi_value fatal;
    napi_get_and_clear_last_exception(env, &fatal);
    napi_fatal_exception(env, fatal);
    return false;
  }
  return true;
}

#define ITC_CALL(env, call)                                                    \
  maybe_throw_fatal(env, call, __LINE__);

#define NAPI_CALL(call)                                                        \
  {                                                                            \
    napi_status status = call;                                                 \
    if (status != napi_ok) { return status; }                                  \
  }

// special value used to signal the uv_loop we are done pushing data
#define QUEUE_STOP ((void*)-1)

// static
struct NapiItcHandle {
  // uv_async_t must be first so a pointer to NapiItcHandle
  // can be safely cast to a pointer of uv_async_t
  uv_async_t uv_async;
  napi_itc_consumer_callback execute_cb;
  napi_itc_complete_callback complete_cb;
  napi_env env; // apparently safe to cache if used in nodejs uv loop
  ThreadSafeQueue<void*> consumerQu;
  void* userdata;
};
typedef NapiItcHandle napi_itc_handle_t;

static void uvFinalize(uv_handle_t* handle) {
  delete handle;
}

// usefull tool to setup and teardown the callback scope
class CallbackScope {
  public:
    CallbackScope(napi_env _env): env(_env) {
      napi_value resource_name, resource_object;
      ITC_CALL(env, napi_open_handle_scope(env, &scope));
      ITC_CALL(env, napi_create_object(env, &resource_object));
      ITC_CALL(env, napi_create_string_utf8(env, "itc_uv_resource", NAPI_AUTO_LENGTH, &resource_name));
      ITC_CALL(env, napi_async_init(env, resource_object, resource_name, &async_context));
      ITC_CALL(env, napi_open_callback_scope(env, resource_object, async_context, &callback_scope));
    }

    ~CallbackScope() {
      ITC_CALL(env, napi_close_callback_scope(env, callback_scope));
      ITC_CALL(env, napi_async_destroy(env, async_context));
      ITC_CALL(env, napi_close_handle_scope(env, scope));
    }

  private:
    napi_env env;
    napi_handle_scope scope;
    napi_callback_scope callback_scope;
    napi_async_context async_context;
};

// happens in the uv loop
static inline bool uvItcCallback(napi_itc_handle_t* hdl) {
  if (!hdl->execute_cb) {
    return false;
  }

  CallbackScope s(hdl->env);
  bool run = true;
  void* eventData;
  while(run && hdl->consumerQu.next(eventData)) {
    // should call the user defined callback
    if (eventData == QUEUE_STOP) {
      hdl->complete_cb != nullptr
        ? hdl->complete_cb(hdl->env, (napi_itc_handle)hdl, napi_ok, hdl->userdata)
        : 0;
      run = false;
    } else {
      hdl->execute_cb(hdl->env, (napi_itc_handle)hdl, hdl->userdata, eventData);
    }
    if (!maybe_throw_unhandled(hdl->env)) {
      run = false;
    }
  }
  return run;
}

// happens in the uv loop
static void uvCallback(uv_async_t* handle) {
  auto hdl = (napi_itc_handle_t*)handle;
  if (!uvItcCallback(hdl)) {
    uv_close((uv_handle_t*)handle, uvFinalize);
  }
}

EXTERN_C_START

napi_status napi_itc_init(
  napi_env env,
  napi_itc_consumer_callback userExeCallback,
  napi_itc_complete_callback userCpCallback,
  void* userdata,
  napi_itc_handle* handle)
{
  uv_loop_t* loop;
  NAPI_CALL(napi_get_uv_event_loop(env, &loop));

  napi_itc_handle_t* hdl = new napi_itc_handle_t;
  hdl->env = env;
  hdl->execute_cb = userExeCallback;
  hdl->complete_cb = userCpCallback;
  hdl->userdata = userdata;

  if (uv_async_init(loop, &hdl->uv_async, uvCallback)) {
    delete hdl;
    return napi_generic_failure;
  }

  *handle = (napi_itc_handle)hdl;
  return napi_ok;
}

void napi_itc_complete(napi_itc_handle handle) {
  ((napi_itc_handle_t*)handle)->consumerQu.push(QUEUE_STOP);
  uv_async_send((uv_async_t*)handle);
}

void napi_itc_send(napi_itc_handle handle, void* data) {
  ((napi_itc_handle_t*)handle)->consumerQu.push(data);
  uv_async_send((uv_async_t*)handle);
}

EXTERN_C_END
