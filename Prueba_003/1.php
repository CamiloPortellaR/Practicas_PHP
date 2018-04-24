<?php 
 // Crear un flujo
    $opciones = array(
                'http'=>array(
                'method'=>"GET",
                'header'=>"Accept-language: en\r\n" .
                 "Cookie: foo=bar\r\n"
         )
       );
   $contexto = stream_context_create($opciones);
   $página_inicio = file_get_contents('http://192.168.52.189/Tesis.html',false, $contexto); 
   $find_nn1   = '"byte_status"';
   $find_fn    = '"fn"';
   $pos1 = strpos($página_inicio, $find_nn1);
   $pos2 = strpos($página_inicio, $find_fn);
   $posdif = $pos2 - $pos1+4;
   echo substr($página_inicio, $pos1,$posdif);
  ?>
  