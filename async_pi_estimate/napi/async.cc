#include "pi_est.h"  // NOLINT(build/include)
#include "async.h"  // NOLINT(build/include)

typedef struct {
  napi_ref _callback;
  napi_async_work _request;

  int points;
  double estimate;

} async_calculate_data;

// Executed inside the worker-thread.
// It is not safe to access JS engine data structure
// here, so everything we need for input and output
// should go on `async_calculate_data`.
void doCalculation(napi_env env, void* param) {
  async_calculate_data* data = static_cast<async_calculate_data*>(param);
  data->estimate = Estimate(data->points);
}

// Executed when the async work is complete
// this function will be run inside the main event loop
// so it is safe to use JS engine data again
void doneCalculation(napi_env env, napi_status status, void* param) {
  async_calculate_data* data = static_cast<async_calculate_data*>(param);

  napi_value callback;
  status = napi_get_reference_value(env, data->_callback, &callback);
  assert(status == napi_ok);

  napi_value global;
  status = napi_get_global(env, &global);
  assert(status == napi_ok);

  napi_value argv[2];
  status = napi_get_undefined(env, &argv[0]);
  status = napi_create_double(env, data->estimate, &argv[1]);
  assert(status == napi_ok);

  napi_value result;
  status = napi_call_function(env, global, callback, 2, argv, &result);
  assert(status == napi_ok);
}

// Asynchronous access to the `Estimate()` function
napi_value CalculateAsync(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  assert(status == napi_ok);

  if (argc < 2) {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
    return nullptr;
  }

  napi_valuetype valuetype0;
  status = napi_typeof(env, args[0], &valuetype0);
  assert(status == napi_ok);

  napi_valuetype valuetype1;
  status = napi_typeof(env, args[1], &valuetype1);
  assert(status == napi_ok);

  if (valuetype0 != napi_number || valuetype1 != napi_function) {
    napi_throw_type_error(env, nullptr, "Wrong arguments");
    return nullptr;
  }

  async_calculate_data* data = (async_calculate_data*)malloc(sizeof(async_calculate_data));

  status = napi_get_value_int32(env, args[0], &data->points);
  assert(status == napi_ok);

  status = napi_create_reference(env, args[1], 1, &data->_callback);
  assert(status == napi_ok);

  napi_value resourceName;
  status = napi_create_string_utf8(env, "asyncPiCalculation", -1, &resourceName);
  assert(status == napi_ok);

  status = napi_create_async_work(env, 0, resourceName,
    doCalculation, doneCalculation, data, &data->_request);
  assert(status == napi_ok);

  status = napi_queue_async_work(env, data->_request);
  assert(status == napi_ok);

  return nullptr;
}