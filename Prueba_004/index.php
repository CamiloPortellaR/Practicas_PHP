<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="utf-8">

		<!-- Always force latest IE rendering engine (even in intranet) & Chrome Frame
		Remove this if you use the .htaccess -->
		<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">

		<title>index</title>
		<meta name="description" content="">
		<meta name="author" content="Administrator">

		<meta name="viewport" content="width=device-width, initial-scale=1.0">

		<!-- Replace favicon.ico & apple-touch-icon.png in the root of your domain and delete these references -->
		<link rel="shortcut icon" href="/favicon.ico">
		<link rel="apple-touch-icon" href="/apple-touch-icon.png">
	</head>

	<body>
		<div>
			
			<h1> Ingrese sus datos</h1>
			<?php session_start();?> 
			<?php (array_key_exists("mensaje",$_SESSION))? print($_SESSION['mensaje']):print(PHP_EOL); ?>
			<?php unset($_SESSION['mensaje']);?>
			<form action="file2.php" method="POST">
				
				<input type="text" name="input1"  placeholder="ingrese dato"/><br /><br />
				<select name="input2"><br />
					<option value="1">hola1</option>
					<option value="2">hola2</option>
					<option value="3">hola3</option>
				</select><br /><br />
				<input type="submit" /><br />
				
				
			</form>
		</div>
	</body>
</html>
