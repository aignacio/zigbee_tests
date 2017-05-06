<meta http-equiv="content-type" content="text/html;charset=utf-8" />
<?php
	echo "PROBLEMA<br />";
    $hostname = "localhost";    // Vamos considerar localhost ou máquina local
    $username = "root";    		// Username é userTeste
    $password = "3102230";     	// Password é passTeste (Cuidado com maiúsculas e minúsculas
 
    mysql_connect("$hostname", "$username", "$password") or die(mysql_error());
    echo "Conexão efectuada com sucesso!<br/>";
     
    mysql_select_db("sensors_homestark") or die(mysql_error());
    echo "Base de dados seleccionada!<br/>";
?>