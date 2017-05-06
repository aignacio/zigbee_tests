<?php
	$local_db = "localhost";
	$default_user = "root";
	$db_password = "3102230";
	$default_db = "homestark";

	//Check if we have some POST and if the fields are empty
	if (!empty($_POST) AND (empty($_POST['username']) OR empty($_POST['password']))) 
	{
		echo "<script language=\"JavaScript\">\n";
		echo "alert('Please complete all fields!');\n";
		echo "window.location='index.php'";
		echo "</script>";
	}
	else
	{
		//Try to connect to database
		$try = mysql_connect($local_db,$default_user,$db_password) or trigger_error(mysql_error());
		if (!$try) 
		{
			die('ERROR to connect in MySQL: ' . mysql_error());
		}
		//echo 'Success to connect database!';
		
		//Try to select database
		$try = mysql_select_db($default_db) or trigger_error(mysql_error());
		if (!$try) 
		{
			die('ERROR to select db in MySQL: ' . mysql_error());
		}
		//echo "\n";
		//echo 'Success to select database!';

		$username = mysql_real_escape_string($_POST['username']);
		$password = mysql_real_escape_string($_POST['password']);

		//Search at the bank the username and password
		$sql = "SELECT `id`, `name`, `level` FROM `users` WHERE (`user` = '". $username ."') AND (`password` = '". sha1($password) ."') AND (`active` = 1) LIMIT 1";
		$query = mysql_query($sql);

		if (mysql_num_rows($query) != 1) 
		{
			echo "<script language=\"JavaScript\">\n";
			echo "alert('Invalid Login or inactive by admin!');\n";
			echo "window.location='index.php'";
			echo "</script>";
			exit;
		} 
		else 
		{
			//Save data from bank
			$result = mysql_fetch_assoc($query);

			//Create user session
			if (!isset($_SESSION)) 
				session_start();

			//Save data found in session
			$_SESSION['user_id'] = $result['id'];
			$_SESSION['username'] = $result['name'];
			$_SESSION['userlevel'] = $result['level'];

			//Redirect to homepage
			header("Location:/home/home.php"); 
			exit;
		}
	}
?>