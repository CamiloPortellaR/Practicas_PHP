<?php
    session_start();
	
	// ANTES DE LA REGENERACION SE DEBE ELIMINAR UNA SESSION Y EL CONTENIDO SU ARCHIVO EN CASO QUE EXISTA.
	// Esta situacion se puede dar cuando colocan un URL que apunte directamente AQUI, y sin
	// haber acabado la session.
	    session_unset();

   ///  REGENERACION DE ID DE SESSION(COOKIE), LA REGENERACION CREA UN NUEVO ARCHIVO, NO ELIMINA EL ANTERIOR
   //   Ya no es necesario el uso de SESSION.USE_STRICT_MODE
		session_regenerate_id();

    if ($_POST['input1']== "camilo" && $_POST['input2']==1) {
    		
    	$_SESSION['input1']=$_POST['input1'];
		$_SESSION['input2']=$_POST['input2'];

		// Colocamos la hora de conexion.
		$_SESSION['ultima_conexion'] = time();
		$_SESSION['segundos_permitidos'] = 60;
		
	    // ESTO NO FUNCIONA, ASI DICEN EL EL FORO DE PHP
	    // session_set_cookie_params($_SESSION['segundos_permitidos']); 
	
	    // ESTO SI FUNCIONA DICEN EN EL FORO DE PHP, AUNQUE IGUAL NO LO USAREMOS, YA QUE LO QUE HACE ES 
	    // CREAR UNA NUEVA COOKIE, CON EL MISMO ID, HACE COMO UNA REPLICA.
        // setcookie(session_name(),session_id(),$_SESSION['ultima_conexion']+$_SESSION['segundos_permitidos']);
		
        header('location:file3.php');
    } else {
    	$_SESSION["mensaje"]= "No coloco la clave correcta";
        header('location:index.php');
    }
?>