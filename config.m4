dnl $Id$
dnl config.m4 for extension fiber


PHP_ARG_ENABLE(fiber, whether to enable fiber support,
[  --enable-fiber           Enable fiber support])

if test "$PHP_FIBER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-fiber -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/fiber.h"  # you most likely want to change this
  dnl if test -r $PHP_FIBER/$SEARCH_FOR; then # path given as parameter
  dnl   FIBER_DIR=$PHP_FIBER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for fiber files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       FIBER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$FIBER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the fiber distribution])
  dnl fi

  dnl # --with-fiber -> add include path
  dnl PHP_ADD_INCLUDE($FIBER_DIR/include)

  dnl # --with-fiber -> check for lib and symbol presence
  dnl LIBNAME=fiber # you may want to change this
  dnl LIBSYMBOL=fiber # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $FIBER_DIR/$PHP_LIBDIR, FIBER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_FIBERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong fiber lib version or lib not found])
  dnl ],[
  dnl   -L$FIBER_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(FIBER_SHARED_LIBADD)
  
  sources="
    libcoro/coro.c                 \
    src/fiber.c                    \
  "
  
  PHP_NEW_EXTENSION(fiber, $sources, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
