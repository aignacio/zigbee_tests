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
	<script src="//code.jquery.com/jquery-1.10.2.js"></script>
	<script src="//code.jquery.com/jquery-1.10.2.js"></script>
  	<script src="//code.jquery.com/ui/1.11.2/jquery-ui.js"></script>
	<script type="text/javascript" src="script.js"></script>
	<title>Homestark - Register</title> 
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
			<div id="user_reg">
				<form id="reg" action="" method="post">
					<fieldset style="font-family: Arial, Helvetica, sans-serif; color:	black; text-align:center;" >
						<legend align="center">Create new user:</legend>
						<p><label class="field" for="txUsuario">Name</label><input type="text" name="name" id="name_user"/></p>
						<p><label class="field" for="txUsuario">Username</label><input type="text" name="username" id="username_user"  /></p>
						<p><label class="field" for="txSenha">Password</label><input type="password" name="password" id="password_user" /></p>
						<p><label class="field" for="txSenha">E-mail</label><input type="text" name="email" id="email_user" /></p>
						<p><label class="field" for="txSenha">Level</label>
						<select id="level_op" style="width:200px; size:10; font-weight: bold;">
							<option>1</option>
							<option>2</option>
							<option>3</option>
							<option>4</option>
							<option>5</option>
						</select>
						</p>
						<p><label class="field" for="txSenha">Active</label><input type="text" name="active" id="active_user" /></p>
						<p><input type="submit" value="Submit" class="submit" align="center" id="register_user"></p>
					</fieldset>
				</form>
			</div>
			<div id="sensor_reg">
				<form id="reg" action="" method="post">
					<fieldset style="font-family: Arial, Helvetica, sans-serif; color:	black; text-align:center;" >
						<legend align="center">Add new sensor:</legend>
						<p><label class="field" for="txUsuario">Tag</label><input type="text" name="tag" id="tag"/></p>
						<p><label class="field" for="txUsuario">Type</label>
						<select id="type_op" style="width:200px; size:10; font-weight: bold;">
							<option>light</option>
							<option>light_rgb</option>
							<option>switch</option>
						</select>	
						</p>
						<p><label class="field" for="txSenha">Location</label>
						<div id="location_op">
						</div>
						</p>
						<p><label class="field" for="txSenha">Active</label><input type="text" name="active" id="active" /></p>
						<p><label class="field" for="txUsuario">Device Address</label><input type="text" name="device_address" id="device_address" /></p>
						<p><input type="submit" value="Create Sensor" class="submit" align="center" id="register_sensor"></p>
					</fieldset>
				</form>	
			</div>
			<div id="location_reg">
			<form id="reg" action="" method="post">
				<fieldset style="font-family: Arial, Helvetica, sans-serif; color:	black; text-align:center;" >
					<legend align="center">Add new location:</legend>
					<p><label class="field_loc_id" for="txUsuario">Location</label><input type="text" name="location_new" id="location_new"/></p>
					<p><label class="field_loc_id" for="txSenha">Level Allowed (less or equal to)</label>
						<select id="level_allow" style="width:200px; size:10; font-weight: bold;">
							<option>1</option>
							<option>2</option>
							<option>3</option>
							<option>4</option>
							<option>5</option>
						</select>
						</p>
					<p><input type="submit" value="Add new location" class="submit" align="center" id="register_location"></p>
				</fieldset>
			</form>	
		</div>
		</div>
	</div>
</body> 
</html>


