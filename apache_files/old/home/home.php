<?php
	$level_allowed = 10; //Everyone can access this page
	 
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
		header("Location: index.php"); exit;
	}
?>

<!DOCTYPE html> 
<html> 
<head> 
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link href="style_home.css" type="text/css" rel="stylesheet" />
	<link rel="icon" type="image/jpeg" href="images/logo/favicon.jpg"/>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
	<script type="text/javascript" src="script.js"></script>
	<title>Homestark - Home Page</title> 
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
			Homepage Control
		</div>	
		<div id="register" <?php if ($_SESSION['userlevel']!=1){?>style="display:none"<?php } ?>>
			<form method="get" action="register.php" id="register">
	    		<input type="submit" value="Register new user"/>
			</form>
		</div>
		<div id="delete" <?php if ($_SESSION['userlevel']!=1){?>style="display:none"<?php } ?>>
			<form method="get" action="delete.php" id="delete">
	    		<input type="submit" value="Delete user"/>
			</form>
		</div>
	</div>
	<div id="teste" style="width:500px; background-color:red; height:500px;">
	</div>
</body> 
</html>


