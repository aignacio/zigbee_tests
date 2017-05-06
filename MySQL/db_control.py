##############################################################
#															 
#	Project: Home Control - MySQL socket					 
#	Author: Anderson Ignacio da Silva						 
#   Date: 22/10/14											 
#	Target: Cubieboard_2-A20								 
#	Inf.: http://www.esplatforms.blogspot.com.br 			 
#															 
##############################################################
#!/usr/bin/python
import MySQLdb

#Used to disable warnings from mysql
from warnings import filterwarnings
import MySQLdb as Database
filterwarnings('ignore', category = Database.Warning)
#Used to disable warnings from mysql

##############################################################
#Default database to connect 
default_server = "localhost"
default_user   = "root"
password	   = "3102230"

##############################################################

#create_db(db,db_name) - Create a database into the mysql even if it exist
def create_db(db_name):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	command = "DROP DATABASE IF EXISTS " + db_name + ";"
	handle.execute(command)
	command = "CREATE DATABASE " + db_name + ";"
	handle.execute(command)
	print "\tDatabase created in %s - Name:%s " % (default_server,db_name)
	db.close()
	return 0

# create_table_t1(db,database_name,table_name)
# Create a table type-I in this form
# +----------+-------------+------+-----+---------+----------------+
# | Field    | Type        | Null | Key | Default | Extra          |
# +----------+-------------+------+-----+---------+----------------+
# | id       | int(11)     | NO   | PRI | NULL    | auto_increment |
# | name     | varchar(30) | YES  |     | NULL    |                |
# | location | varchar(50) | YES  |     | NULL    |                |
# | value    | int(11)     | YES  |     | NULL    |                |
# | input    | int(11)     | YES  |     | NULL    |                |
# | output   | int(11)     | YES  |     | NULL    |                |
# +----------+-------------+------+-----+---------+----------------+
def create_table_t1(database_name,table_name):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()
	
	command = "USE " + database_name + ";"
	handle.execute(command)
	command = "DROP TABLE IF EXISTS " + table_name + ";"
	handle.execute(command)
	command = "CREATE TABLE " + table_name + "(id INT AUTO_INCREMENT PRIMARY KEY, \
											   name varchar(30),location varchar(50)\
											   ,value INT,input INT,output INT);"
	handle.execute(command)

	print "\tTable created    in %s - Name:%s " % (default_server,table_name)
	db.close()
	return "ok"

#insert_table(db,table_name,name_sensor,location_sensor, value_sensor, input_s, output_s) - Insert sensor values into the database Sensors_homestark
def insert_table(db_name,table_name,name_sensor,location_sensor, value_sensor, input_s, output_s):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	try:
		command = "USE " + db_name + ";"
		handle.execute(command)
		command = "INSERT INTO " + table_name + "(name,location,value,input,output) VALUES('%s','%s','%d','%d','%d');" % \
																						  (name_sensor,location_sensor,value_sensor,input_s,output_s)
		handle.execute(command)
		db.commit()
	except:
		# Rollback in case there is any error
		db.rollback()

	print "\tData inserted in table:%s" % (table_name)
	db.close()
	return "ok"

#update_table(db_name, table_name, device_address, row_to_change, value) - Update sensor values into the database Sensors_homestark
def update_table(db_name,table_name, device_address, col, value):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	command = "USE " + db_name + ";"
	handle.execute(command)
	
	try:
		command = "UPDATE  " + table_name + " SET %s='%s' where device_address = '%s';" % (col,value,device_address) 
		handle.execute(command)
		db.commit()
		print "\tTable %s updated in device_address %s" % (table_name,device_address)
	except:
	   # Rollback in case there is any error
	   db.rollback()
	
	db.close()
	return "ok"

#get_row(db_name,table_name,line) - Retrieve a row from db, returns a tupple
def get_row(db_name,table_name,line):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	command = "USE " + db_name + ";"
	handle.execute(command)
	#Prepare SQL query to INSERT a record into the database.
	command = "SELECT * FROM " + table_name +";"
	
	try:
		handle.execute(command) 
		#numrows = int (handle.rowcount)
		while line:
			line -= 1;
			row = handle.fetchone() #Return will be a tupple
		
		#print "Id:%d Name:%s Location:%s Value:%d Input:%d Output:%d" % (row[0],row[1],row[2],row[3],row[4],row[5])
		return row
		# Get and display the rows one at a time
		# for i in range (numrows):
		# 	row = handle.fetchone()
		# 	if (row):
		# 		print row[0], row[1], row[2], row[3], row[4], row[5]
	except:
		print "Error: unable to fetch data"

	db.close()

if __name__ == '__main__':
	insert_table(db_name,table_name,name_sensor,location_sensor, value_sensor, input_s, output_s)
	create_table_t1(db_name,table_name)
	update_table(db_name,table_name, device_address, col, value)
	get_row(db_name,table_name,line)