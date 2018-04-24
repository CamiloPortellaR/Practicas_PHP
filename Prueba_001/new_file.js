/**
 * @author Tesis
 */

  function cambiar_color() {
    alert("hola");
  }

   function  traer_script(){
   	
   	var xhttp = new XMLHttpRequest();
   	
   	xhttp.onreadystatechange = function (){
    		if ((xhttp.readyState == 4) && (xhttp.status == 200)){
    			
    			// Colocamos el contenido de la respuesta en el selector.
    			window.document.querySelector("#camilo").innerHTML = xhttp.responseText;
   	
   	}};
   	xhttp.open("POST","new_file.php", true);
   	xhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
   	
    xhttp.send();
    
    
    }
