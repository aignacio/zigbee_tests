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

	$tag = $_POST['tag'];
      
	$result = mysql_query("SELECT value FROM sensors WHERE tag='$tag'");         
	$row = mysql_fetch_array($result);

	//Test if we need to put value to '1' or to '0'
	if($row['value'] == 1)
	{
		$result = mysql_query("UPDATE sensors SET value=0 WHERE tag='$tag'"); 
	}
	else
	{
		$result = mysql_query("UPDATE sensors SET value=1 WHERE tag='$tag'"); 
	}

	$result = mysql_query("SELECT value FROM sensors WHERE tag='$tag'");         
	$row = mysql_fetch_array($result);

	echo $row['value'];
?>
