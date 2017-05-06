<?php
	$local_db = "localhost";
	$default_user = "root";
	$db_password = "3102230";
	$default_db = "sensors_homestark";

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

	$username = mysql_real_escape_string($_POST['username1']);
	$password = mysql_real_escape_string($_POST['password1']);
	$name = mysql_real_escape_string($_POST['name1']);
	$email = mysql_real_escape_string($_POST['email1']);
	$active = mysql_real_escape_string($_POST['active1']);	
	$level = mysql_real_escape_string($_POST['level1']);

	$query = mysql_query("insert into users (id,name, user, email, password, level, active, register) values (NULL,'$name','$username','$email',sha1('$password'),'$level','$active',NOW())");
	echo "Form Submitted Succesfully";
	mysql_close($con); // Connection Closed
?>