<?php
    $server_Name    = "192.168.1.30";
    $connectionInfo = array("Database"=>"siteplayer","UID" =>"sa","PWD" => "sa");
	$conn = sqlsrv_connect($server_Name , $connectionInfo);
	if($conn === false){
		die(print_r(sqlsrv_errors(),TRUE));
	}
	else
		echo "Conexión establecida.<br />";
?>


