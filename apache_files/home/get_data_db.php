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

	$tableName = $_POST['tableName'];
	$id	= $_POST['id'];

	$result = mysql_query("SELECT $id FROM $tableName");         
	$data = array();
	while ( $row = mysql_fetch_row($result) )
	{
	  $data[] = $row;
	}
	echo json_encode( $data );
?>
