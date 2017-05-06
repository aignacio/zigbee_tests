<!DOCTYPE html> 
<html> 
<head> 
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link href="style.css" type="text/css" rel="stylesheet" />
	<link rel="icon" type="image/jpeg" href="images/logo/favicon.jpg"/>
	<title>Homestark</title> 
</head> 
<body> 
	<div id ="logo_scr">
	</div>
	<div id="scr_login">
		<form id="adj" action="valid.php" method="post">
			<fieldset style="font-family: Arial, Helvetica, sans-serif; color:	black; border:0; text-align:center;" >
				<legend align="center" style="font-weight: bold; font-family: Arial, Helvetica, sans-serif; color:	black; border:0; text-align:center;">Login:</legend>
				<p><label class="field" for="txUsuario">Username</label><input type="text" name="username" id="username"  /></p>
				<p><label class="field" for="txSenha">Password</label><input type="password" name="password" id="password" /></p>
				<p><input type="submit" value="Enter" align="center" id="enter"></p>
			</fieldset>
		</form>
	</div>
</body> 
</html>