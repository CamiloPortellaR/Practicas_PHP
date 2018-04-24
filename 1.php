<?php
    //Error:SQLSTATE[45000]: <<Unknown error>>: 12345 An error occurred ccc  => echo 'Error:'.$e->getMessage();
    
    function insertar_tabla_codeigniter_1($Nombre, $Ahorro, $Sexo, $Id_Pais) {
		 try{
           $mysqli_PDO_obj = new PDO('mysql:host=localhost;dbname=demo_ci','root','',array(PDO::MYSQL_ATTR_INIT_COMMAND=>'SET NAMES UTF8')); 
		   $mysqli_PDO_obj->setAttribute(PDO::ATTR_ERRMODE,PDO::ERRMODE_EXCEPTION); //PDO::ATTR_ERRMODE: Reporte de errores.  , PDO::ERRMODE_EXCEPTION: Lanza exceptions.
		   $consulta_sql="call insertar_tabla_codeigniter_1(:Nombre,:Ahorro,:Sexo,:Id_Pais)"; //"call obtener_datos(:nombre_profesor)";
		   $pdo_stmt=$mysqli_PDO_obj->prepare($consulta_sql);
		   $pdo_stmt->execute(array(':Nombre'=>$Nombre,':Ahorro'=>$Ahorro,':Sexo'=>$Sexo,':Id_Pais'=>$Id_Pais));
		   
		   $datos_persona="";
		   while ($registro=$pdo_stmt->fetch(PDO::FETCH_ASSOC)){
		   	  $datos_persona .= $registro["Id"].$registro["Nombre"].$registro["Ahorro"].$registro["Sexo"].$registro["Id_Pais"].PHP_EOL;
		   }
		   echo "$datos_persona";
		   
		   $pdo_stmt->closeCursor();
		 }catch(PDOException $e){
		 	echo 'Error:'.$e->getCode().PHP_EOL.$e->getMessage();
		 }
    }
	
	insertar_tabla_codeigniter_1('cam1iFDDDcvbGFdfgdfl24',234,'Mujer',2);
	
	/*
	 * Error:23000
       SQLSTATE[23000]: Integrity constraint violation: 1452 Cannot add or update a child row: a foreign key constraint fails (`demo_ci`.`tabla_practica_codelobster_1`, CONSTRAINT `FK_Id_Pais` FOREIGN KEY (`Id_Pais`) REFERENCES `tabla_pais` (`Id_Pais`))
	 * 
	 * Error:23000
     * SQLSTATE[23000]: Integrity constraint violation: 1062 Duplicate entry 'ale144fgf53fh5553' for key 'Nombre'
	 * 
	 * Error:90001
     * SQLSTATE[90001]: <<Unknown error>>: 12345 El sexo ingrasado no es hombre ni mujer, estan tratando de VULNERAR el sistema
	 * 
	 * 
	 */
	
	 
?>