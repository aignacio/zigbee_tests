<?php
	$level_allowed = 1;
	 
	// 1----------------------->High priority
	// 22
	// 333
	// 4444
	// 55555
	// 666666
	// 7777777
	// 88888888
	// 999999999
	// 101010101010------------->Less priority

	//We need to init the session in each page
	if (!isset($_SESSION)) 
		session_start();
	if (!isset($_SESSION['username']) OR ($_SESSION['userlevel']>$level_allowed))
	{
		//Redirect if the user is not in a session
		session_destroy();
		header("Location:../index.php"); exit;
	}
?>

<!DOCTYPE html> 
<html> 
<head> 
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link href="style.css" type="text/css" rel="stylesheet" />
	<link rel="icon" type="/image/jpeg" href="../images/logo/favicon.jpg"/>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script type="text/javascript" src="script.js"></script>
	<title>Homestark - Delete</title> 
</head> 
<body> 
	<div id="st">
		<div id ="tab">
			<div id="menus">
				<a href="home.php" id="username">
					Hi, <?php echo $_SESSION['username']; ?>			
				</a>
				<div id="level">
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
				<a href="register.php" id="register">
					Register				
				</a>
				<a href="delete.php" id="delete">
					Delete				
				</a>
				<a href="../index.php" id="logout">
					Logout				
				</a>
			</div>
		</div>
		<div id="forms">
			<div id="delete_user">
				<div id="users_delete">
				</div>
				<p><input type="submit" value="Delete User" class="delete" align="center" id="del_user"></p>
			</div>
			<div id="delete_sensor">
				<div id="sensor_delete">
				</div>
				<p><input type="submit" value="Delete Sensor" class="delete" align="center" id="del_sensor"></p>
			</div>
			<div id="delete_loc">
				<div id="loc_delete">
				</div>
				<p><input type="submit" value="Delete Location" class="delete" align="center" id="del_loc"></p>
			</div>
		</div>
	</div>
</body> 
</html>


