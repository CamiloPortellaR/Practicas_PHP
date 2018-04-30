<?php
    //ini_set("session.cache_expire", 10);
    //ini_set("session.gc_probability", 1000);
	//ini_set("session.gc_maxlifetime", 10);
	ini_set("session.cookie_lifetime", 20);
	session_start();
	$_SESSION['valor1']=23.9;
	$_SESSION['valor2']='camilo2';
	$_SESSION['valor3']=1;
	$_SESSION['valor4']=TRUE;
	$_SESSION['valor5']=array('key_camilo'=>'valor_camilo',4=>3);
	echo $_SESSION['valor1']." ".$_SESSION['valor2']." ".$_SESSION['valor3']." ".$_SESSION['valor4'].PHP_EOL;
	echo session_cache_expire();
	phpinfo();
?>