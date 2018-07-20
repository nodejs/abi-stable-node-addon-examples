#include "pi_est.h"  // NOLINT(build/include)
#include "sync.h"  // NOLINT(build/include)

// Simple synchronous access to the `Estimate()` function
napi_value CalculateSync(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
    return nullptr;
  }

  napi_valuetype valuetype0;
  status = napi_typeof(env, args[0], &valuetype0);
  assert(status == napi_ok);

  if (valuetype0 != napi_number) {
    napi_throw_type_error(env, nullptr, "Wrong arguments");
    return nullptr;
  }

  int points;
  status = napi_get_value_int32(env, args[0], &points);
  assert(status == napi_ok);

  double est = Estimate(points);

  napi_value estimate;
  status = napi_create_double(env, est, &estimate);
  assert(status == napi_ok);

  return estimate;
}