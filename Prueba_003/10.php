<?php
   session_start();
    if (session_status() === PHP_SESSION_ACTIVE) {
    	echo "la session esta habilitada y existe una";
	}else if (session_status() === PHP_SESSION_NONE) {
		echo "la session esta habilitada pero no existe";
	}
?>