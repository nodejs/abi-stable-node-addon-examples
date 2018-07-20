#ifndef EXAMPLES_ASYNC_PI_ESTIMATE_ASYNC_H_
#define EXAMPLES_ASYNC_PI_ESTIMATE_ASYNC_H_

#include <node_api.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

napi_value CalculateAsync(napi_env env, napi_callback_info info);

#endif  // EXAMPLES_ASYNC_PI_ESTIMATE_ASYNC_H_
