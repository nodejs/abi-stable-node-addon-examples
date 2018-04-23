#include "node_api.h"

// Throw an error with a specified message if the N-API call fails.
#define NAPI_CALL_BASE(env, call, message, return_value)                       \
  {                                                                            \
    napi_status status = call;                                                 \
    if (status != napi_ok) {                                                   \
      napi_throw_error((env), NULL, (message));                                \
      return return_value;                                                     \
    }                                                                          \
  }

#define NAPI_CALL(env, call, message) \
  NAPI_CALL_BASE((env), (call), (message), NULL)

// The last parameter is intentionally empty.
#define NAPI_CALL_VOID(env, call, message) \
  NAPI_CALL_BASE((env), (call), (message),)

// Add a property whose name is stored in prefix to the value stored either in
// this, or in the first argument. The value of the property will be `true`.
#define SET_TARGET_PROPERTY(env, use_this, info, prefix)                       \
  {                                                                            \
    napi_value target, js_true;                                                \
    if (use_this) {                                                            \
      NAPI_CALL((env),                                                         \
                napi_get_cb_info((env), (info), NULL, NULL, &target, NULL),    \
                prefix ": Failed to retrieve target via callback info");       \
    } else {                                                                   \
      size_t argc = 1;                                                         \
      NAPI_CALL((env),                                                         \
                napi_get_cb_info((env), (info), &argc, &target, NULL, NULL),   \
                prefix ": Failed to retrieve target via callback info");       \
    }                                                                          \
    NAPI_CALL(env, napi_get_boolean((env), true, &js_true),                    \
              prefix ": Failed to create boolean value");                      \
    NAPI_CALL(env, napi_set_named_property((env), target, (prefix), js_true),  \
              prefix ": Failed to set property");                              \
  }

// Set super_ctor as the superclass of ctor.
static void napi_inherits(napi_env env, napi_value ctor,
                                napi_value super_ctor) {
  napi_value global, global_object, set_proto, ctor_proto_prop,
      super_ctor_proto_prop;
  napi_value args[2];

  NAPI_CALL_VOID(env, napi_get_global(env, &global),
                 "napi_inherits: Failed to retrieve global object");
  NAPI_CALL_VOID(env, napi_get_named_property(env, global, "Object",
                                              &global_object),
                 "napi_inherits: Failed to retrieve 'Object' from global");
  NAPI_CALL_VOID(env, napi_get_named_property(env, global_object,
                                              "setPrototypeOf", &set_proto),
            "napi_inherits: Failed to retrieve 'setPrototypeOf' from 'Object'");
  NAPI_CALL_VOID(env,
                 napi_get_named_property(env, ctor, "prototype",
                                         &ctor_proto_prop),
                 "napi_inherits: Failed to retrieve 'prototype' from subclass");
  NAPI_CALL_VOID(env, napi_get_named_property(env, super_ctor, "prototype",
                                              &super_ctor_proto_prop),
               "napi_inherits: Failed to retrieve 'prototype' from superclass");

  args[0] = ctor_proto_prop;
  args[1] = super_ctor_proto_prop;
  NAPI_CALL_VOID(env, napi_call_function(env, global, set_proto, 2, args, NULL),
               "napi_inherits: Failed to call 'setPrototypeOf' for prototypes");

  args[0] = ctor;
  args[1] = super_ctor;
  NAPI_CALL_VOID(env, napi_call_function(env, global, set_proto, 2, args, NULL),
             "napi_inherits: Failed to call 'setPrototypeOf' for constructors");
}

static napi_value construct_superclass(napi_env env, napi_callback_info info) {
  SET_TARGET_PROPERTY(env, true, info, "construct_superclass");
  return NULL;
}

static napi_value superclass_method(napi_env env, napi_callback_info info) {
  SET_TARGET_PROPERTY(env, true, info, "superclass_method");
  return NULL;
}

static napi_value superclass_static_method(napi_env env,
                                           napi_callback_info info) {
  SET_TARGET_PROPERTY(env, false, info, "superclass_static_method");
  return NULL;
}

static napi_value superclass_overridden_method(napi_env env,
                                               napi_callback_info info) {
  SET_TARGET_PROPERTY(env, true, info, "superclass_overridden_method");
  return NULL;
}

static napi_value superclass_overridden_static_method(napi_env env,
                                                      napi_callback_info info) {
  SET_TARGET_PROPERTY(env, false, info,
                      "superclass_overridden_static_method");
  return NULL;
}

static napi_value superclass_chained_method(napi_env env,
                                            napi_callback_info info) {
  SET_TARGET_PROPERTY(env, true, info, "superclass_chained_method");
  return NULL;
}

static napi_value superclass_chained_static_method(napi_env env,
                                                   napi_callback_info info) {
  SET_TARGET_PROPERTY(env, false, info, "superclass_chained_static_method");
  return NULL;
}

static napi_value construct_subclass(napi_env env, napi_callback_info info) {
  construct_superclass(env, info);
  SET_TARGET_PROPERTY(env, true, info, "construct_subclass");
  return NULL;
}

static napi_value subclass_method(napi_env env, napi_callback_info info) {
  SET_TARGET_PROPERTY(env, true, info, "subclass_method");
  return NULL;
}

static napi_value subclass_static_method(napi_env env,
                                         napi_callback_info info) {
  SET_TARGET_PROPERTY(env, false, info, "subclass_static_method");
  return NULL;
}

static napi_value
subclass_superclass_overridden_method(napi_env env, napi_callback_info info) {
  SET_TARGET_PROPERTY(env, true, info,
                      "subclass_superclass_overridden_method");
  return NULL;
}

static napi_value
subclass_superclass_overridden_static_method(napi_env env,
                                             napi_callback_info info) {
  SET_TARGET_PROPERTY(env, false, info,
                      "subclass_superclass_overridden_static_method");
  return NULL;
}

static napi_value subclass_superclass_chained_method(napi_env env,
                                                     napi_callback_info info) {
  superclass_chained_method(env, info);
  SET_TARGET_PROPERTY(env, true, info,
                      "subclass_superclass_chained_method");
  return NULL;
}

static napi_value
subclass_superclass_chained_static_method(napi_env env,
                                          napi_callback_info info) {
  superclass_chained_static_method(env, info);
  SET_TARGET_PROPERTY(env, false, info,
                      "subclass_superclass_chained_static_method");
  return NULL;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_value superclass, subclass;

  napi_property_descriptor superclass_properties[] = {
      {"superclassMethod", NULL, superclass_method, NULL, NULL, NULL,
       napi_enumerable, NULL},
      {"superclassStaticMethod", NULL, superclass_static_method, NULL, NULL,
       NULL, napi_enumerable | napi_static, NULL},
      {"superclassOverriddenMethod", NULL, superclass_overridden_method, NULL,
       NULL, NULL, napi_enumerable, NULL},
      {"superclassOverriddenStaticMethod", NULL,
       superclass_overridden_static_method, NULL, NULL, NULL,
       napi_enumerable | napi_static, NULL},
      {"superclassChainedMethod", NULL, superclass_chained_method, NULL, NULL,
       NULL, napi_enumerable, NULL},
      {"superclassChainedStaticMethod", NULL, superclass_chained_static_method,
       NULL, NULL, NULL, napi_enumerable | napi_static, NULL}};

  napi_property_descriptor subclass_properties[] = {
      {"subclassMethod", NULL, subclass_method, NULL, NULL, NULL,
       napi_enumerable, NULL},
      {"subclassStaticMethod", NULL, subclass_static_method, NULL, NULL, NULL,
       napi_enumerable | napi_static, NULL},
      {"superclassOverriddenMethod", NULL,
       subclass_superclass_overridden_method, NULL, NULL, NULL, napi_enumerable,
       NULL},
      {"superclassOverriddenStaticMethod", NULL,
       subclass_superclass_overridden_static_method, NULL, NULL, NULL,
       napi_enumerable | napi_static, NULL},
      {"superclassChainedMethod", NULL, subclass_superclass_chained_method,
       NULL, NULL, NULL, napi_enumerable, NULL},
      {"superclassChainedStaticMethod", NULL,
       subclass_superclass_chained_static_method, NULL, NULL, NULL,
       napi_enumerable | napi_static, NULL}};

  NAPI_CALL(env, napi_define_class(env, "Superclass", NAPI_AUTO_LENGTH,
                                   construct_superclass, NULL,
                                   sizeof(superclass_properties) /
                                       sizeof(superclass_properties[0]),
                                   superclass_properties, &superclass),
            "Failed to define superclass");

  NAPI_CALL(env,
            napi_define_class(
                env, "Subclass", NAPI_AUTO_LENGTH, construct_subclass, NULL,
                sizeof(subclass_properties) / sizeof(subclass_properties[0]),
                subclass_properties, &subclass),
            "Failed to define subclass");

  napi_inherits(env, subclass, superclass);

  NAPI_CALL(env,
            napi_set_named_property(env, exports, "Superclass", superclass),
            "Failed to assign superclass to exports");

  NAPI_CALL(env, napi_set_named_property(env, exports, "Subclass", subclass),
            "Failed to assign subclass to exports");

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
