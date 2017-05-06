var tag = new Array(),
	type = new Array(),
	loc_buttons = new Array();

$(document).ready(function()
{
	/************************************* HOME.PHP *************************************/
	//Create location buttons for the user navigate
	create_loc_op();

	/************************************* REGISTER.PHP *************************************/
	//Refresh the option list in all page register.php
	refresh_reg_op();
	//Used to register user in database
	$("#register_user").click(function()
	{
		var name = $("#name_user").val();
		var username = $("#username_user").val();
		var email = $("#email_user").val();
		var password = $("#password_user").val();
		var level = $("#level_op option:selected").text();
		var active = $("#active_user").val();
		var addsensor = '0';
		var adduser = '1';
		var addloc = '0';
		var dataString = 'addloc=' + addloc + '&username1=' + username + '&name1='+ name + '&email1='+ email + '&password1='+ password + '&level1='+ level + '&active1=' + active + '&adduser=' + adduser + '&addsensor=' + addsensor;
		
		if(name==''||email==''||password==''||level==''||active=='')
		{
			alert("Please Fill All Fields in new user");
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
		refresh_reg_op();
	return false;
	});
	//Used to register sensor in database
	$("#register_sensor").click(function()
	{
		var tag = $("#tag").val();
		var type = $("#type_op option:selected").text();
		var location = $("#location_op option:selected").text();
		var device_address = $("#device_address").val();
		var active = $("#active").val();
		var addsensor = '1';
		var adduser = '0';
		var addloc = '0';
		var dataString = 'addloc=' + addloc + '&tag=' + tag + '&type='+ type + '&location='+ location + '&active=' + active + '&adduser=' + adduser + '&addsensor=' + addsensor + '&device_address=' + device_address;
		
		if(tag==''||type==''||location==''||active==''||device_address== '')
		{
			alert("Please Fill All Fields in Sensor");
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
		refresh_reg_op();
	return false;
	});
	//Used to register locations 
	$("#register_location").click(function()
	{
		var location = $("#location_new").val();
		var level = $("#level_allow option:selected").text();
		var addsensor = '0';
		var adduser = '0';
		var addloc = '1';
		var dataString = 'level=' + level + '&location=' + location + '&addloc=' + addloc + '&adduser=' + adduser + '&addsensor=' + addsensor;
		
		if(level==''||location=='')
		{
			alert("Please Fill All Fields in Location");
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
		refresh_reg_op();
	return false;
	});
	
	/************************************* DELETE.PHP *************************************/
	//Refresh the delete list in all page delete.php
	refresh_del_op();
	//Used in delete.php to delete user
	$("#del_user").click(function()
	{
		delete_db($("#delete_op option:selected").text(),"user","users");
		refresh_del_op();
	});
	//Used in delete.php to delete sensor
	$("#del_sensor").click(function()
	{
		delete_db($("#delete_op_s option:selected").text(),"tag","sensors");
		refresh_del_op();
	});
	//Used in delete.php to delete location
	$("#del_loc").click(function()
	{
		delete_db($("#delete_op_l option:selected").text(),"location","locations");
		refresh_del_op();
	});
});

//Create location buttons for the user navigate
function create_loc_op()
{
	//Used to display list of locations
	$(function () 
	{
		var datas = 'tableName=locations'+'&id=location';

		$.ajax({  
				type: "POST",                                    
				url: 'get_loc_db.php',                         
				data: datas,                        
				dataType: 'json',                   
				success: function(data)         
						{
							var i,res;
							res = "<div style=\"margin-top:5px; font-weight: bold; font-family: Arial, Helvetica, sans-serif;\">Locations:</br></div>";
							
						 	for (var i in data)
						    {
						    	var row = data[i];          
						    	loc_buttons[i] = row[0];  
  						        res += "<div id=\""+loc_buttons[i]+"\" class= \"sensor_button\" onclick=\"javascript:list_sensors(this.id)\">"+loc_buttons[i]+"</div>";
							}       
							 
						    $('#sensors_panel').html(res);
						} 
				});
	});	
}

//Refresh the delete list in all page delete.php
function refresh_del_op()
{
	//Used to display list of users in select box
	$(function () 
	{	
		var datas = 'tableName=users'+'&id=user';
		$.ajax({  
				type: "POST",                                    
				url: 'get_data_db.php',                         
				data: datas,                        
				dataType: 'json',                //data format      
				success: function(data)          //on recieve of reply
						{
							var i,res;
							res = "Select user to delete:</br><select id=\"delete_op\" style=\"width:400px; size:10; font-weight: bold;\">";
							
						 	for (var i in data)
						    {
						      var row = data[i];          
						      var name = row[0];
						      res +="<option>"+name+"</option>";
						    } 
						    res+= "</select>"
						    $('#users_delete').html(res);							
						} 
				});
	});	
	//Used to display list of sensors in select box
	$(function () 
	{	
		var datas = 'tableName=sensors'+'&id=tag';
		$.ajax({  
				type: "POST",                                    
				url: 'get_data_db.php',                         
				data: datas,                        
				dataType: 'json',                //data format      
				success: function(data)          //on recieve of reply
						{
							var i,res;
							res = "Select sensor to delete:</br><select id=\"delete_op_s\" style=\"width:400px; size:10; font-weight: bold;\">";
							
						 	for (var i in data)
						    {
						      var row = data[i];          
						      var name = row[0];
						      res +="<option>"+name+"</option>";
						    } 
						    res+= "</select>"
						    $('#sensor_delete').html(res);							
						} 
				});
	});	
	//Used to display list of locations in select box
	$(function () 
	{	
		var datas = 'tableName=locations'+'&id=location';
		$.ajax({  
				type: "POST",                                    
				url: 'get_data_db.php',                         
				data: datas,                        
				dataType: 'json',                //data format      
				success: function(data)          //on recieve of reply
						{
							var i,res;
							res = "Select location to delete:</br><select id=\"delete_op_l\" style=\"width:400px; size:10; font-weight: bold;\">";
							
						 	for (var i in data)
						    {
						      var row = data[i];          
						      var name = row[0];
						      res +="<option>"+name+"</option>";
						    } 
						    res+= "</select>"
						    $('#loc_delete').html(res);							
						} 
				});
	});
}

//Refresh register options
function refresh_reg_op()
{
	$(function () 
	{	
		var datas = 'tableName=locations'+'&id=location';
		$.ajax({  
				type: "POST",                                    
				url: 'get_data_db.php',                         
				data: datas,                        
				dataType: 'json',                //data format      
				success: function(data)          //on recieve of reply
						{
							var i,res;
							res = "<select style=\"width:200px; size:10; font-weight: bold;\">";
							
						 	for (var i in data)
						    {
						      var row = data[i];          
						      var name = row[0];
						      res +="<option>"+name+"</option>";
						    } 
						    res+= "</select>"
						    $('#location_op').html(res);							
						} 
				});
	});
}

//Delete row in database
function delete_db(value,id,table)
{
	var dataString = "id=" + id + "&table=" + table + "&value=" + value;
	if(value=='')
	{
		alert("Please select an option!");
	}
	else
	{
		// AJAX Code To Submit Form.
		$.ajax({
				type: "POST",
				url: "delete_db.php",
				data: dataString,
				cache: false,
				success: function(result)
						 {
						 	alert(result);
						 }
				});
	}
}

//List sensors of a location
function list_sensors(value)
{  
	$(function () 
	{	
		var loc = 'loc='+value;
		$.ajax({  
				type: "POST",                                    
				url: 'get_loc_sensors.php',                         
				data: loc,                        
				dataType: 'json',                //data format      
				success: function(data)          //on recieve of reply
						{
							var i,res;
							res = "<div style=\"margin-top:5px; font-weight: bold; font-family: Arial, Helvetica, sans-serif;\">Sensors:</br></div>";
							
							//Do each row("sensor")
						 	for (var i in data)
						    {
						    	var row = data[i];          
						    	tag[i] = row[0];
						    	type[i] = row[1];
						   		value = row[2];

						    	switch(type[i]) 
						    	{
								    case 'light':
								        res += "<div id=sensor_"+i+" style=\"background-image:url(../images/Icons_location/light_icon.png)\" class= \"sensor_home\" onclick=\"javascript:show_op("+i+")\"><div id=\"text\">"+tag[i]+"</div></div>";
								        res += "<div id=\"op_"+i+"\">"
								        res += "<button id=\"toggle_button_"+i+"\" onclick=\"toggle("+i+")\""
								        if(value==1)
								        {
								        	res += "class=\"green\">"
								        }
								        else
								        {
								        	res += "class=\"red\">"
								        }
								        if(value==1)
								        {
								        	res += "ON"	
								        }
								        else
								        {
								        	res += "OFF"
								        }	
								        res += "</button>" 	
								        res += "</div>"
								        res += "<style>#op_"+i+"{display:none;}</style>"
								        res += "<style>#op_"+i+"{margin-right:auto; margin-left:auto; float:center; position:relative; width:780px; height:100px; background-image:url(../images/sensor_op_bkp.png);}</style>"
								        break;
								    case 'switch':
								        res += "<div id=sensor_"+i+" style=\"background-image:url(../images/Icons_location/switch_icon.png)\" class= \"sensor_home\" onclick=\"javascript:show_op("+i+")\"><div id=\"text\">"+tag[i]+"</div></div>"
								        res += "<div id=\"op_"+i+"\">"
								        res += "<button id=\"toggle_button_"+i+"\" onclick=\"toggle("+i+")\""
								        if(value==1)
								        {
								        	res += "class=\"green\">"
								        }
								        else
								        {
								        	res += "class=\"red\">"
								        }
								        if(value==1)
								        {
								        	res += "ON"	
								        }
								        else
								        {
								        	res += "OFF"
								        }	
								        res += "</button>" 	
								        res += "</div>"
								        res += "<style>#op_"+i+"{display:none;}</style>"
								        res += "<style>#op_"+i+"{margin-right:auto; margin-left:auto; float:center; position:relative; width:780px; height:100px; background-image:url(../images/sensor_op_bkp.png);}</style>"
								        break;
								    case 'light_rgb':
								        res += "<div id=sensor_"+i+" style=\"font-weight: bold; font-family: Arial, Helvetica, sans-serif; background-image:url(../images/Icons_location/light_rgb_icon.png)\" class= \"sensor_home\" onclick=\"javascript:show_op("+i+")\"><div id=\"text\">"+tag[i]+"</div></div>"
								        res += "<div id=\"op_"+i+"\">"
								        res += "Light RGB:<input id=\"color_"+i+"\" onchange=\"light_rgb(this.color,"+i+")\" class=\"color\" value=\"66ff00\" style=\"margin-top:35px;\">"
								        res += "</div>"
								        res += "<style>#op_"+i+"{display:none;}</style>"
								        res += "<style>#op_"+i+"{margin-right:auto; margin-left:auto; float:center; position:relative; width:780px; height:100px; background-image:url(../images/sensor_op_bkp.png);}</style>"
								        break;
								    default:
										alert('Unknow type of sensor! ='+row[1]);
								}
						    } 
						    $('#list_sensors').html(res);							
						} 
				});
	});	 
}

//Send color RGB light to database
function light_rgb(color,device)
{
	//alert("Color:"+color+"\nLight id:"+light_id);
	var dataString = 'tag=' + tag[device] + '&value=' + color;
	// AJAX Code To Submit Form.
	$.ajax({
			type: "POST",
			url: "set_sensors.php",
			data: dataString,
			cache: false,
			success: function(result)
					 {
					 	//alert(result);
					 }
			});

}

//Show options of a sensor
function show_op(div)
{
	$("#op_"+div).slideToggle();
	jscolor.init(); //Refresh jscolor jquery to see the effect and search for input with color class
}

//Toggle value in a switch ou light type sensor
function toggle(device)
{
	//alert("Tag:"+tag[device]+"\nType:"+type[device]);
	switch(type[device])
	{
		case 'light':
			var dataString = 'tag=' + tag[device];
			// AJAX Code To Submit Form.
			$.ajax({
					type: "POST",
					url: "set_sensors_toggle.php",
					data: dataString,
					cache: false,
					success: function(result)
							 {
							 	//alert(result);
							 	if(result==0)
							 	{
							 		$("#toggle_button_"+device).html("OFF");
							 		$("#toggle_button_"+device).removeClass( "green" );
							 		$("#toggle_button_"+device).addClass( "red" );
							 	}
							 	else if(result==1)
							 	{
							 		$("#toggle_button_"+device).html("ON");
							 		$("#toggle_button_"+device).removeClass( "red" );
							 		$("#toggle_button_"+device).addClass( "green" );
							 	}
							 }
					});
		break;
		case 'switch':
			var dataString = 'tag=' + tag[device];
			// AJAX Code To Submit Form.
			$.ajax({
					type: "POST",
					url: "set_sensors_toggle.php",
					data: dataString,
					cache: false,
					success: function(result)
							 {
							 	//alert(result);
							 	if(result==0)
							 	{
							 		$("#toggle_button_"+device).html("OFF");
							 		$("#toggle_button_"+device).removeClass( "green" );
							 		$("#toggle_button_"+device).addClass( "red" );
							 	}
							 	else if(result==1)
							 	{
							 		$("#toggle_button_"+device).html("ON");
							 		$("#toggle_button_"+device).removeClass( "red" );
							 		$("#toggle_button_"+device).addClass( "green" );
							 	}
							 }
					});
		break;
	}
}

//Used to show value to debug
function debug(id)
{
	alert("DEBUG: " + id)
}