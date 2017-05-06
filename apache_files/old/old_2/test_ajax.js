
function ValidLogin()
{
  var username = document.getElementById('username');
  var password = document.getElementById('password');
var xmlhttp;
if (window.XMLHttpRequest)
  {// code for IE7+, Firefox, Chrome, Opera, Safari
  xmlhttp=new XMLHttpRequest();
  }
else
  {// code for IE6, IE5
  xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
xmlhttp.onreadystatechange=function()
  {
  if (xmlhttp.readyState==4 && xmlhttp.status==200)
    {
    document.getElementById("myDiv").innerHTML=xmlhttp.responseText;
    }
  }
// Open an asynchronous POST connection and send request
  xmlreq.open("POST", "valid.php", true);
  xmlreq.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xmlreq.send("username=" + username.value + "&password=" + password.value);
}
