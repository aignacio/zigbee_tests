<?php
	$level_allowed = 1; //Just administrator can modify
	//We need to init the session in each page
	if (!isset($_SESSION)) 
		session_start();
	if (!isset($_SESSION['username']) OR ($_SESSION['userlevel']>$level_allowed))
	{
		//Redirect if the user is not in a session
		session_destroy();
		header("Location: index.php"); exit;
	}
?>

<!DOCTYPE html> 
<html> 
<head> 
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link href="style_register.css" type="text/css" rel="stylesheet" />
	<link rel="icon" type="image/jpeg" href="images/logo/favicon.jpg"/>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.0/jquery.min.js"></script>
	<script src="script_reg.js"></script>
	<title>Homestark - Register</title> 
</head> 
<body> 
	<div id ="tab">
		<div id="button">
			<form method="get" action="logout.php" id="log">
	    		<input type="submit" value="Logout"/>
			</form>
		</div>
		<div id="level">
			Your level is 
			<?php 
				switch ($_SESSION['userlevel']) 
				{
					case 0:
						echo "<font color='white'>[Developer]</font>";
					break;
					case 1:
						echo "<font color='red'>[Administrator]</font>";
					break;
					case 2:
						echo "<font color='green'>[User]</font>";
					break;
					case 3:
						echo "<font color='blue'>[Monitor]</font>";
					break;
					default:
						echo "<font color='red'>[Monitor]</font>";
					break;
				}
			?>
		</div>
		<div id="name">
			Hi, <?php echo $_SESSION['username']; ?>
		</div>
		<div id="page">
			Register new user
		</div>
	</div>
	<div id="form_register">
		<form id="reg" action="" method="post">
			<fieldset style="font-family: Arial, Helvetica, sans-serif; color:	white; text-align:center;" >
				<legend align="center">Login:</legend>
				<p><label class="field" for="txUsuario">Name</label><input type="text" name="name" id="name_user"/></p>
				<p><label class="field" for="txUsuario">Username</label><input type="text" name="username" id="username_user"  /></p>
				<p><label class="field" for="txSenha">Password</label><input type="password" name="password" id="password_user" /></p>
				<p><label class="field" for="txSenha">E-mail</label><input type="text" name="email" id="email_user" /></p>
				<p><label class="field" for="txSenha">Level</label><input type="text" name="level" id="level_user" /></p>
				<p><label class="field" for="txSenha">Active</label><input type="text" name="active" id="active_user" /></p>
				<p><input type="submit" value="Submit" class="submit" align="center" id="enter"></p>
			</fieldset>
		</form>
	</div>
</body> 
</html>