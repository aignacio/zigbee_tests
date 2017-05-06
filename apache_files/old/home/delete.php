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
	<link href="style_delete.css" type="text/css" rel="stylesheet" />
	<link rel="icon" type="image/jpeg" href="images/logo/favicon.jpg"/>
	<script src="script_delete.js"></script>
	<title>Homestark - Delete user</title> 
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
			Delete User
		</div>
	</div>
	<div id="users_delete">
		<div id="delete_user">
			<form id="reg" action="" method="post">
				<fieldset style="font-family: Arial, Helvetica, sans-serif; color:	white; text-align:center;" >
					<legend align="center">User to delete:</legend>
					<p><label class="field" for="txUsuario">User</label><input type="text" name="user" id="user"/></p>
					<p><input type="submit" value="Delete" name="delete_s" id="delete_s"/></p>
				</fieldset>
			</form>
		</div>
		<div id="users">
			<input type="submit" value="Refresh users" name="rf" id="rfd" onclick="GetUsers();"/>
		</div>
	</div>
</body> 
</html>