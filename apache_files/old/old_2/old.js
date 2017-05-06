function ValidLogin() 
{
	var username = document.getElementById('username');
	var password = document.getElementById('password');
	var result = document.getElementById("login_res");
	var xmlhttp;
	if (window.XMLHttpRequest)
	{	// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	}
	else
	{	// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	
	// Set up the readyState change event handler	
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{
			result.innerHTML=xmlhttp.responseText;
		}
		else
	  	{
			result.innerHTML = "Erro: " + xmlhttp.statusText;
		}
	}

	xmlhttp.open("GET","ajax_info.txt",true);
	xmlhttp.send();
	// Open an asynchronous POST connection and send request
	// xmlreq.open("POST", "valid.php", true);
	// xmlreq.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	// xmlreq.send("username=" + username.value + "&password=" + password.value);
}