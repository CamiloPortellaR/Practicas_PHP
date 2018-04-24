<?php
   //Obtencion del string resumido
   $página_Syte_Player     = file_get_contents('http://192.168.52.198/Tesis.html',false);
   $find_byte_status       = '"byte_status"';
   $find_fn                = '"fn"';
   $pos1                   = strpos($página_Syte_Player, $find_byte_status);
   $pos2                   = strpos($página_Syte_Player, $find_fn);
   $posdif                 = $pos2 - $pos1 + 4;
   $pagina_resumen         = substr($página_Syte_Player, $pos1,$posdif);//esto nos servira para limitar el espacio de busqueda
   
   //Declaracion de variables del SitePlayer
   $ctr_1 = array("nombre"=>"ctr_1", "valor"=>"");
   $ma_1  = array("nombre"=>"ma_1",  "valor"=>"");
   
   //Declarar funcion para obtencion del valor apartir del nombre
   function get_name_value($fname) {
    global $pagina_resumen; // Uso el keyword global para poder acceder a la variable global $pagina_resumen
        $pos_name  = strpos($pagina_resumen,$fname); //Ubico donde esta el nombre 
		$pos_value = strpos($pagina_resumen,"value",$pos_name); //Ubibo la posicion de la palabra valor asociado al nombre
		$comilla = chr(34);//The chr() function returns a character from the specified ASCII value.
		$pos_value_comilla_1 = strpos($pagina_resumen,$comilla,$pos_value);
		$pos_value_comilla_2 = strpos($pagina_resumen,$comilla,$pos_value_comilla_1+1);
		$pos_value_dif_comilla = $pos_value_comilla_2-$pos_value_comilla_1 ;
		$pos_valor = substr($pagina_resumen, $pos_value_comilla_1+1,$pos_value_dif_comilla-1);
	    return $pos_valor;
   }
   
   //Vamos a usar Associative Array , JavaScript no soporta estos tipos de array 
   $ctr_1['valor'] = get_name_value($ctr_1['nombre']);
   echo $ctr_1['valor'];
   $ma_1['valor']  = get_name_value($ma_1['nombre']);
   echo $ma_1['valor'] ;
   $serverName = "192.168.52.194";
   $connectionInfo = array( "Database"=>"siteplayer", "UID"=>"sa", "PWD"=>"sa");
   $conn = sqlsrv_connect( $serverName, $connectionInfo);
   if( $conn ) {
     echo "Conexión establecida.<br />";
    }else{
     echo "Conexión no se pudo establecer.<br />";
     die( print_r( sqlsrv_errors(), true));
  }
	$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ma_1['nombre'], $ma_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
		$sql = "INSERT INTO valores (nombre, valor,fecha) VALUES (?, ?, convert(varchar(20),getdate(),113))";
    $params = array($ctr_1['nombre'], $ctr_1['valor']);

    $stmt = sqlsrv_query( $conn, $sql, $params);
        if( $stmt === false ) {
            die( print_r( sqlsrv_errors(), true));
		}
   ?>