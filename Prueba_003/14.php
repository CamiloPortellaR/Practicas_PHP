<?php
    //ini_set("session.cache_expire", 10);
	//ini_set("session.gc_probability", 1000);
	//ini_set("session.gc_maxlifetime", 10);
	ini_set("session.cookie_lifetime", 10);
	session_start();
	echo $_SESSION['valor1']." ".$_SESSION['valor2']." ".$_SESSION['valor3']." ".$_SESSION['valor4'].PHP_EOL;
?>