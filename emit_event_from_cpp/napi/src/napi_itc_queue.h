#ifndef __NAPI_ITC_QUEUE_H
#define __NAPI_ITC_QUEUE_H

#include <node_api.h>
#include <uv.h>

#ifdef __cplusplus
  #define EXTERN_C_START extern "C" {
  #define EXTERN_C_END }
#else
  #define EXTERN_C_START
  #define EXTERN_C_END

  #ifndef true
    typedef int bool;
    #define true 1
    #define false 0
  #endif
#endif

EXTERN_C_START

typedef struct napi_itc_handle__ *napi_itc_handle; // opaque pointer a la NAPI

typedef void (*napi_itc_consumer_callback) (napi_env, napi_itc_handle, void*, void*);
typedef void (*napi_itc_complete_callback) (napi_env, napi_itc_handle, napi_status, void*);

napi_status napi_itc_init(
  napi_env env,
  napi_itc_consumer_callback userExeCallback,
  napi_itc_complete_callback userCpCallback,
  void* userdata,
  napi_itc_handle*);

void napi_itc_send(napi_itc_handle ctx, void* data);
void napi_itc_complete(napi_itc_handle handle);

EXTERN_C_END

#endif
