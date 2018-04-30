<?php
   session_start();
   
   if(!isset($_SESSION['input1'])){
   	   $_SESSION['mensaje']="No te creas gracioso, ingresa tu clave";
       die(header("location:index.php"));
   }

   // Una vez verificado que el archivo tiene contenido.
   //  verificamos que no se haiga exedido el tiempo permitido
   if(time()-$_SESSION['ultima_conexion'] > $_SESSION['segundos_permitidos']){
   	   session_unset();
	   $_SESSION['mensaje']= "Sobrepaso el tiempo permitido";
	   die(header("location:index.php"));
   }
   
   /////  ESTE BLOQUE NO ES INDISPENSABLE //////////////////
    echo "paso la prueba Primera PAGINA"."<br /><br />";
  
  
   echo "La hora en inglaterra es: ".date_default_timezone_get();
	echo date("d-M-Y H:i:s")."<br /><br /><br />";
   
   date_default_timezone_set("America/Lima");
   
   echo "La hora en peru es: ".date_default_timezone_get();
   echo date("d-M-Y H:i:s")."<br /><br /><br />";
   
   echo "El tiempo que le sobraba es : ".($_SESSION['segundos_permitidos']-(time()-$_SESSION['ultima_conexion']))."<br /><br />";
   ////////////////////////////////////////////////////////////////////////////////
   
   $_SESSION['ultima_conexion'] = time();
?>

  <html>
  	<head></head>
  	<body>
  		<br />
  		<a href="file4.php">Pasar a file4</a>
  		<br />
  		<form action="logout.php">
  			<input type="submit" name="logout.php" value="Salir"/>
  		</form>
  		
  	</body>
  </html>