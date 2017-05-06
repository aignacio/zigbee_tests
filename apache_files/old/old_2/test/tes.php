<!DOCTYPE html>
<html>
<head>
<title>Hello, Ajax</title>
</head>
 
<body>
  <h2>Hello, Ajax</h2>
  <table>
    <tr>
      <td><textarea id="inText" name="inText" cols="40" rows="5">Enter your text here...</textarea></td>
      <td><textarea id="ajaxText" name="ajaxText" cols="40" rows="5"></textarea></td>
    </tr>
   </table>
   <a href="test" onclick="return loadAjaxText()">SEND</a>
</body>
 
<script type="text/javascript">
// Append Ajax responseText into element with id="ajaxText"
function loadAjaxText() {
   // Allocate an XMLHttpRequest object
   if (window.XMLHttpRequest) {
      // IE7+, Firefox, Chrome, Opera, Safari
      var xmlhttp=new XMLHttpRequest();
   } else {
      // IE6, IE5
      var xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
   }
   // Set up the readyState change event handler
   xmlhttp.onreadystatechange = function() {
      if ((this.readyState === 4) && (this.status === 200)) {
         if (this.responseText !== null) {
            var ajaxElm = document.getElementById('ajaxText');
            ajaxElm.innerHTML = this.responseText + ajaxElm.innerHTML; // append in front
         }
      }
   }
   // Open an asynchronous POST connection and send request
   xmlhttp.open("POST", "HelloAjax.php", true);
   xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
   var inTextElm = document.getElementById('inText');
   xmlhttp.send("inText=" + inTextElm.value);
   return false;  // Do not follow hyperlink
}
</script>
</html>