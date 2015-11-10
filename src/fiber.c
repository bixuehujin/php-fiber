#include "fiber.h"

zend_class_entry * fiber_ce_Fiber;
zend_object_handlers fiber_handler_Fiber;

static fiber_t *current_fiber = NULL;

#define array_addref(z)                                 \
    do {                                                \
        zend_array *_z1 = (z);                          \
        zend_refcounted *_gc = (zend_refcounted *)_z1;  \
        GC_REFCOUNT(_gc)++;                             \
    } while (0)

#define array_delref(z)                                 \
    do {                                                \
        zend_array *_z1 = (z);                          \
        zend_refcounted *_gc = (zend_refcounted *)_z1;  \
        GC_REFCOUNT(_gc)--;                             \
        if (GC_REFCOUNT(_gc) <= 1) {                    \
           zend_array_destroy(_z1);                     \
        }                                               \
    } while(0)


static inline fiber_t *fiber_from_obj(zend_object *obj) /* {{{ */ {
    return (fiber_t*)((char*)(obj) - XtOffsetOf(fiber_t, std));
}

static zend_object *fiber_object_new(zend_class_entry *ce)
{
    fiber_t *intern;

    intern = ecalloc(1, sizeof(fiber_t) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->started = 0;
    intern->params = NULL;

    intern->std.handlers = &fiber_handler_Fiber;

    return &intern->std;
}

static void fiber_object_free(zend_object *object)
{
    fiber_t *intern = fiber_from_obj(object);

    coro_stack_free(&intern->stack);

    zval_ptr_dtor(&intern->callback);
    if (intern->params) {
        array_delref(intern->params);
    }

    zend_object_std_dtor(&intern->std);
}

static int call_user_function_array(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, zval *params)
{
    zend_fcall_info fci;
    int retval;

    fci.size = sizeof(fci);
    fci.function_table = EG(function_table);
    fci.object = NULL;
    ZVAL_COPY_VALUE(&fci.function_name, function_name);
    fci.retval = retval_ptr;;
    fci.no_separation = (zend_bool) 1;
    fci.symbol_table = NULL;
    fci.param_count = 0;
    fci.params = NULL;

    zend_fcall_info_args(&fci, params);

    retval = zend_call_function(&fci, NULL);

    zend_fcall_info_args_clear(&fci, 1);

    return retval;
}

FIBER_METHOD(Fiber, __construct) {
    zval *callback;
    zend_array *params = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|H", &callback, &params) == FAILURE) {
        return;
    }

    if (!zend_is_callable(callback, 0, NULL)) {
        zend_throw_exception(spl_ce_InvalidArgumentException, "The parameter $callback is not a valid callable", 0);
        return;
    }

    fiber_t *intern = fiber_from_obj(Z_OBJ_P(getThis()));

    ZVAL_COPY(&intern->callback, callback);
    if (params) {
        array_addref(params);
        intern->params = params;
    }

    coro_stack_alloc(&intern->stack, 0);
    coro_create(&intern->ctx, NULL, NULL, intern->stack.sptr, intern->stack.ssze);
}

FIBER_METHOD(Fiber, switch) {
    zend_string *name;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
        return;
    }

    zval retval;
    fiber_t *intern = fiber_from_obj(Z_OBJ_P(getThis()));
    current_fiber = intern;

    if (!intern->started) {
        intern->started = 1;
        zval params;

        if (intern->params) {
            ZVAL_ARR(&params, intern->params);
            call_user_function_array(EG(function_table), NULL, &intern->callback, &retval, &params);
        } else {
            call_user_function(EG(function_table), NULL, &intern->callback, &retval, 0, NULL);
        }
    } else {
        if (intern != current_fiber) {
            coro_transfer(&current_fiber->ctx, &intern->ctx);
        }
    }
}


FIBER_METHOD(Fiber, throw) {
    zend_string *name;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
        return;
    }

}


ZEND_BEGIN_ARG_INFO_EX(arginfo_class___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, callback)
    ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_switch, 0, 0, 1)
    ZEND_ARG_INFO(0, retval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_throw, 0, 0, 1)
    ZEND_ARG_INFO(0, exception)
ZEND_END_ARG_INFO()

static const zend_function_entry class_methods[] = {
    FIBER_ME(Fiber, __construct,    arginfo_class___construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    FIBER_ME(Fiber, switch,         arginfo_class_switch,         ZEND_ACC_PUBLIC)
    FIBER_ME(Fiber, throw,         arginfo_class_throw,         ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fiber)
{
    ZEND_TSRMLS_CACHE_UPDATE();

    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Fiber", class_methods);
    fiber_ce_Fiber = zend_register_internal_class(&ce);
    fiber_ce_Fiber->create_object = fiber_object_new;

    memcpy(&fiber_handler_Fiber, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    fiber_handler_Fiber.offset = XtOffsetOf(fiber_t, std);
    fiber_handler_Fiber.free_obj = fiber_object_free;

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(fiber)
{


    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(fiber)
{


    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fiber)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "fiber support", "enabled");
    php_info_print_table_end();
}
/* }}} */


/* {{{ hash_module_entry
 */
zend_module_entry fiber_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "fiber",
    NULL,
    PHP_MINIT(fiber),
    NULL, //PHP_MSHUTDOWN(fiber),
    PHP_RINIT(fiber),
    PHP_RSHUTDOWN(fiber),
    PHP_MINFO(fiber),
#if ZEND_MODULE_API_NO >= 20010901
    "0.1.0", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_FIBER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(fiber)
#endif
