#ifndef EXAMPLES_ASYNC_PI_ESTIMATE_SYNC_H_
#define EXAMPLES_ASYNC_PI_ESTIMATE_SYNC_H_

#include <node_api.h>
#include <assert.h>

napi_value CalculateSync(napi_env env, napi_callback_info info);

#endif  // EXAMPLES_ASYNC_PI_ESTIMATE_SYNC_H_
