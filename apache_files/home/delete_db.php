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

	$value = mysql_real_escape_string($_POST['value']);
	$table = mysql_real_escape_string($_POST['table']);
	$id = mysql_real_escape_string($_POST['id']);

	$result = mysql_query("DELETE from $table WHERE $id='$value'");   
	echo $value." was removed!";
?>
