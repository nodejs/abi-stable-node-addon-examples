#include <node_api.h>
#include <assert.h>
#include "sync.h"   // NOLINT(build/include)
#include "async.h"  // NOLINT(build/include)

#define DECLARE_NAPI_METHOD(name, func)                          \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

// Expose synchronous and asynchronous access to our
// Estimate() function
napi_value Init(napi_env env, napi_value exports) {
  napi_status status;

  napi_property_descriptor desc[] = {
    DECLARE_NAPI_METHOD("calculateSync", CalculateSync),
    DECLARE_NAPI_METHOD("calculateAsync", CalculateAsync),
  };
  status =
    napi_define_properties(env, exports, sizeof(desc) / sizeof(*desc), desc);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)