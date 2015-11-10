/*
 * fiber.h
 *
 *  Created on: Mar 14, 2015
 *      Author: hujin
 */

#ifndef FIBER_H_
#define FIBER_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_variables.h"
#include "ext/spl/spl_exceptions.h"
#include "libcoro/coro.h"

#ifdef PHP_WIN32
#   define PHP_FIBER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_FIBER_API __attribute__ ((visibility("default")))
#else
#   define PHP_FIBER_API
#endif

typedef struct _fiber {
    coro_context ctx;
    struct coro_stack stack;
    zval callback;
    zend_array *params;
    zend_bool started;
    zend_object std;
}fiber_t;

extern zend_class_entry * fiber_ce_Fiber;


#define FIBER_MN(name) zim_fiber_##name
#define FIBER_METHOD(classname, name) ZEND_NAMED_FUNCTION(FIBER_MN(classname##_##name))

#define FIBER_ME(classname, name, arg_info, flags)    ZEND_FENTRY(name, FIBER_MN(classname##_##name), arg_info, flags)
#define FIBER_ABSTRACT_ME ZEND_ABSTRACT_ME

#define FIBER_MINIT_FUNCTION(module) PHP_MINIT_FUNCTION(fiber_##module)
#define FIBER_MSHUTDOWN_FUNCTION(module) PHP_MSHUTDOWN_FUNCTION(fiber_##module)
#define FIBER_RINIT_FUNCTION(module) PHP_RINIT_FUNCTION(fiber_##module)
#define FIBER_RSHUTDOWN_FUNCTION(module) PHP_RSHUTDOWN_FUNCTION(fiber_##module)

#define FIBER_MINIT(module) zm_startup_fiber_##module
#define FIBER_MSHUTDOWN(module) zm_shutdown_fiber_##module
#define FIBER_RINIT(module) zm_activate_fiber_##module
#define FIBER_RSHUTDOWN(module) zm_deactivate_fiber_##module


#define FIBER_CAST(variable, type)  (type)(variable)




ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_magic_get, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_magic_set, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_magic_isset, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_magic_unset, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()




#endif /* FIBER_H_ */
