<?php
	$local_db = "localhost";
	$default_user = "root";
	$db_password = "3102230";
	$default_db = "homestark";

	//Try to connect to database
	$con = mysql_connect($local_db,$default_user,$db_password) or trigger_error(mysql_error());
	if (!$con) 
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

	if ($_POST['adduser']=='1')
	{
		$username = mysql_real_escape_string($_POST['username1']);
		$password = mysql_real_escape_string($_POST['password1']);
		$name = mysql_real_escape_string($_POST['name1']);
		$email = mysql_real_escape_string($_POST['email1']);
		$active = mysql_real_escape_string($_POST['active1']);	
		$level = mysql_real_escape_string($_POST['level1']);

		$result = mysql_query("SELECT user FROM users WHERE user='$username'");         
		$row = mysql_fetch_array($result);

		//Test if exist a user with same username
		if($row['user'] == $username)
		{
			 echo "Already exist a user with this username, please choose other!";
		}
		else
		{
			$query = mysql_query("insert into users (id,name, user, email, password, level, active, register) values (NULL,'$name','$username','$email',sha1('$password'),'$level','$active',NOW())");
			echo "Successfully added user.";
			mysql_close($con); // Connection Closed
		}
		$_POST['adduser']='0';
	}
	else if ($_POST['addsensor']=='1')
	{
		$tag = mysql_real_escape_string($_POST['tag']);
		$type = mysql_real_escape_string($_POST['type']);
		$location = mysql_real_escape_string($_POST['location']);
		$device_address = mysql_real_escape_string($_POST['device_address']);
		$active = mysql_real_escape_string($_POST['active']);

		$result = mysql_query("SELECT tag FROM sensors WHERE tag='$tag'");         
		$row = mysql_fetch_array($result);

		//Test if exist a sensor with same tag
		if($row['tag'] == $tag)
		{
			 echo "Already exist a sensor with this tag, please choose other!";
		}
		else
		{
			$query = mysql_query("insert into sensors (id, tag, type, request, value, device_address, location, active, register) values (NULL,'$tag','$type','0','0','$device_address','$location','$active',NOW())");
			echo "Successfully added sensor.";
			mysql_close($con); // Connection Closed
		}
		$_POST['addsensor']='0';
	}
	else if($_POST['addloc']=='1')
	{
		$location = mysql_real_escape_string($_POST['location']);
		$level = mysql_real_escape_string($_POST['level']);

		$result = mysql_query("SELECT location FROM locations WHERE location='$location'");         
		$row = mysql_fetch_array($result);

		//Test if exist a location with same name
		if($row['location'] == $location)
		{
			 echo "Already exist a location with this name, please choose other!";
		}
		else
		{
			$query = mysql_query("insert into locations (id,location, level, register) values (NULL,'$location','$level',NOW())");
			echo "Successfully added location.";
			mysql_close($con); // Connection Closed
		}
		$_POST['addloc']='0';
	}
?>