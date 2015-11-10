--TEST--
Test for fiber constructor
--SKIPIF--
<?php if (!extension_loaded("fiber")) print "skip"; ?>
--FILE--
<?php
function valid_func() {}
$fiber = new Fiber('valid_func');
var_dump($fiber);

new Fiber('non_exists_func');
?>
--EXPECTF--
object(Fiber)#%d (0) {
}

Fatal error: Uncaught InvalidArgumentException: The parameter $callback is not a valid callable in %s:%d
Stack trace:
#0 %s(%d): Fiber->__construct('non_exists_func')
#1 {main}
  thrown in %s on line %d
