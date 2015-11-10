--TEST--
Test for fiber with basic functionality
--SKIPIF--
<?php if (!extension_loaded("fiber")) print "skip"; ?>
--FILE--
<?php

$fiber1 = new Fiber('func1');
$fiber2 = new Fiber('func2', [$fiber1]);

function func1() {
    echo 'func1.1' . PHP_EOL;
    $GLOBALS['fiber2']->switch();
    echo 'func1.2' . PHP_EOL;
};


function func2($fiber1) {
    echo 'func2.1' . PHP_EOL;
    $fiber1->switch();
    echo 'func2.2' . PHP_EOL;
};

$fiber1->switch();

?>
--EXPECTF--
func1.1
func2.1
func2.2
func1.2
