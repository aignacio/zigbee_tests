$(document).ready(function()
{
	$("#enter").click(function()
	{
		var name = $("#name_user").val();
		var username = $("#username_user").val();
		var email = $("#email_user").val();
		var password = $("#password_user").val();
		var level = $("#level_user").val();
		var active = $("#active_user").val();
		// Returns successful data submission message when the entered information is stored in database.
		var dataString = 'username1=' + username + '&name1='+ name + '&email1='+ email + '&password1='+ password + '&level1='+ level + '&active1='+ active;
		if(name==''||email==''||password==''||level==''||active=='')
		{
			alert("Please Fill All Fields");
		}
		else
		{
			// AJAX Code To Submit Form.
			$.ajax({
					type: "POST",
					url: "register_db.php",
					data: dataString,
					cache: false,
					success: function(result)
							 {
							 	alert(result);
							 }
					});
		}
	return false;
	});
});