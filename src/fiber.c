#include "fiber.h"

zend_class_entry * fiber_ce_Fiber;
zend_object_handlers fiber_handler_Fiber;

static fiber_t *current_fiber = NULL;

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

    intern->std.handlers = &fiber_handler_Fiber;

    return &intern->std;
}

static void fiber_object_free(zend_object *object)
{
    fiber_t *intern = fiber_from_obj(object);


    zend_object_std_dtor(&intern->std);
}

static void coro_callback(void *arg)
{
    fiber_t *intern = (fiber_t *)arg;


}

FIBER_METHOD(Fiber, __construct) {
    zval *callback;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &callback) == FAILURE) {
        return;
    }

    if (!zend_is_callable(callback, 0, NULL)) {
        zend_throw_exception(spl_ce_InvalidArgumentException, "The parameter $callback is not a valid callable", 0);
        return;
    }

    fiber_t *intern = fiber_from_obj(Z_OBJ_P(getThis()));

    ZVAL_COPY(&intern->callback, callback);

    coro_stack_alloc(&intern->stack, 0);
    coro_create(&intern->ctx, coro_callback, intern, intern->stack.sptr, intern->stack.ssze);
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
        call_user_function(EG(function_table), NULL, &intern->callback, &retval, 0, NULL);
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
    ZEND_ARG_INFO(0, callable)
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
