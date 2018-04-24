<?php
   $página_Syte_Player     = file_get_contents('http://192.168.52.198/Tesis.html',false);
   $find_byte_status       = '"byte_status"';
   $find_fn                = '"fn"';
   $pos1                   = strpos($página_Syte_Player, $find_byte_status);
   $pos2                   = strpos($página_Syte_Player, $find_fn);
   $posdif                 = $pos2 - $pos1 + 4;
   $pagina_resumen         = substr($página_Syte_Player, $pos1,$posdif);//esto nos servira para limitar el espacio de busqueda
   //Declarar funcion
   function get_name_value($fname) {
    global $pagina_resumen; // Uso el keyword global para poder acceder a la variable global $pagina_resumen
        $pos_name  = strpos($pagina_resumen,$fname); //Ubico donde esta el nombre 
        echo $pos_name;
		$pos_value = strpos($pagina_resumen,"value",$pos_name); //Ubibo la posicion de la palabra valor asociado al nombre
		echo $pos_value;
		$comilla = chr(34);//The chr() function returns a character from the specified ASCII value.
		$pos_value_comilla_1 = strpos($pagina_resumen,$comilla,$pos_value);
		echo $pos_value_comilla_1;
		$pos_value_comilla_2 = strpos($pagina_resumen,$comilla,$pos_value_comilla_1+1);
		echo $pos_value_comilla_2;
		$pos_value_dif_comilla = $pos_value_comilla_2-$pos_value_comilla_1 ;
		echo $pos_value_dif_comilla;
		$pos_valor = substr($pagina_resumen, $pos_value_comilla_1+1,$pos_value_dif_comilla-1);
	    echo $pos_valor;
   }
   //Vamos a usar Associative Array , JavaScript no soporta estos tipos de array
   $ctr_1 = array("nombre"=>"ctr_1", "valor"=>"");
   $ma_1  = array("nombre"=>"ma_1",  "valor"=>"");
   get_name_value($ctr_1['nombre']);
   get_name_value($ma_1['nombre']) ;
   ?>