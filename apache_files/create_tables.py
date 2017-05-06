##############################################################
#															 
#	Project: Home Control - MySQL socket					 
#	Author: Anderson Ignacio da Silva						 
#	Date: 22/10/14											 
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
password       = "3102230"

##############################################################

#create_db(db,db_name) - Create a database into the mysql even if it exist
def create_db(db_name):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	command = "DROP DATABASE IF EXISTS " + db_name + ";"
	handle.execute(command)
	command = "CREATE DATABASE IF NOT EXISTS " + db_name + ";"
	handle.execute(command)
	print "\tDatabase created in %s - Name:%s " % (default_server,db_name)
	db.close()
	return 0

#Default locations table
#+----------+------------------+------+-----+---------+----------------+
#| Field    | Type             | Null | Key | Default | Extra          |
#+----------+------------------+------+-----+---------+----------------+
#| id       | int(11) unsigned | NO   | PRI | NULL    | auto_increment |
#| location | varchar(50)      | NO   | UNI | Room    |                |
#| level    | int(3)           | NO   |     | 1       |                |
#| register | datetime         | NO   |     | NULL    |                |
#+----------+------------------+------+-----+---------+----------------+
def create_loc_table(database_name):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()
	
	command = "USE " + database_name + ";"
	handle.execute(command)
	command = "CREATE TABLE IF NOT EXISTS `locations` (`id` INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,\
							   `location` VARCHAR( 50 ) NOT NULL DEFAULT 'Room',\
							   `level` INT( 3 ) NOT NULL DEFAULT '1',\
							   `register` DATETIME NOT NULL ,\
							   PRIMARY KEY (`id`),\
							   UNIQUE KEY `location`(`location`)\
							  ) ENGINE=MyISAM ;"
	handle.execute(command)

	print "\tTable created in %s - Name:%s " % (default_server,"locations")
	db.close()
	return "ok"

#Default user table
# +----------+------------------+------+-----+---------+----------------+
# | Field    | Type             | Null | Key | Default | Extra          |
# +----------+------------------+------+-----+---------+----------------+
# | id       | int(11) unsigned | NO   | PRI | NULL    | auto_increment |
# | name     | varchar(50)      | NO   |     | NULL    |                |
# | user     | varchar(25)      | NO   | UNI | NULL    |                |
# | password | varchar(40)      | NO   |     | NULL    |                |
# | email    | varchar(100)     | NO   |     | NULL    |                |
# | level    | int(1) unsigned  | NO   | MUL | 1       |                |
# | active   | tinyint(1)       | NO   |     | 1       |                |
# | register | datetime         | NO   |     | NULL    |                |
# +----------+------------------+------+-----+---------+----------------+
def create_user_table(database_name):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()
	
	command = "USE " + database_name + ";"
	handle.execute(command)
	command = "CREATE TABLE IF NOT EXISTS `users` (`id` INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,\
						       `name` VARCHAR( 50 ) NOT NULL ,\
						       `user` VARCHAR( 25 ) NOT NULL ,\
						       `password` VARCHAR( 40 ) NOT NULL ,\
				                       `email` VARCHAR( 100 ) NOT NULL ,\
						       `level` INT(1) UNSIGNED NOT NULL DEFAULT '1',\
						       `active` BOOL NOT NULL DEFAULT '1',\
						       `register` DATETIME NOT NULL ,\
						       PRIMARY KEY (`id`),\
						       UNIQUE KEY `user` (`user`),\
						       KEY `level` (`level`)\
						      ) ENGINE=MyISAM ;"
	handle.execute(command)

	print "\tTable created in %s - Name:%s " % (default_server,"users")
	db.close()
	return "ok"

#Default sensor table
# +----------------+------------------+------+-----+---------+----------------+
# | Field          | Type             | Null | Key | Default | Extra          |
# +----------------+------------------+------+-----+---------+----------------+
# | id             | int(11) unsigned | NO   | PRI | NULL    | auto_increment |
# | tag            | varchar(50)      | NO   | UNI | NULL    |                |
# | type           | varchar(25)      | NO   |     | NULL    |                |
# | request        | tinyint(1)       | NO   |     | 0       |                |
# | value          | varchar(100)     | NO   |     | NULL    |                |
# | device_address | int(10)          | NO   |     | NULL    |                |
# | location       | varchar(100)     | NO   |     | NULL    |                |
# | active         | tinyint(1)       | NO   |     | 1       |                |
# | register       | datetime         | NO   |     | NULL    |                |
# +----------------+------------------+------+-----+---------+----------------+
def create_sensor_table(database_name):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()
	
	command = "USE " + database_name + ";"
	handle.execute(command)
	command = "CREATE TABLE IF NOT EXISTS `sensors` (`id` INT(11) UNSIGNED NOT NULL AUTO_INCREMENT,\
							 `tag` VARCHAR( 50 ) NOT NULL ,\
							 `type` VARCHAR( 25 ) NOT NULL ,\
							 `request` BOOL NOT NULL DEFAULT '0',\
							 `value` VARCHAR( 100 ) NOT NULL ,\
							 `device_address` INT( 10 ) NOT NULL,\
							 `location` VARCHAR( 100 ) NOT NULL ,\
							 `active` BOOL NOT NULL DEFAULT '1',\
							 `register` DATETIME NOT NULL ,\
							 PRIMARY KEY (`id`),\
							 UNIQUE KEY `tag` (`tag`)\
							) ENGINE=MyISAM ;"
	handle.execute(command)

	print "\tTable created in %s - Name:%s " % (default_server,"locations")
	db.close()
	return "ok"

def insert_sensor(db_name,tag,type_s,device_address,location,active):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	try:
		command = "USE " + db_name + ";"
		handle.execute(command)
		#command = "INSERT INTO users VALUES (NULL,'%s','%s',sha1('%s'),'%s','%d','%d',NOW());" % (name,user,password_user,email,level,active)																						  (name_sensor,location_sensor,value_sensor,input_s,output_s)
		command = "INSERT INTO sensors VALUES (NULL,'%s','%s',0,0,'%s','%s','%d',NOW());" % \
					(tag,type_s,device_address,location,active)

		handle.execute(command)
		db.commit()
	except:
		# Rollback in case there is any error
		db.rollback()

	print "\tData inserted in table:sensors"
	db.close()
	return "ok"

def insert_user(db_name,name,user,password_user,email,level,active):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	try:
		command = "USE " + db_name + ";"
		handle.execute(command)
		#command = "INSERT INTO users VALUES (NULL,'%s','%s',sha1('%s'),'%s','%d','%d',NOW());" % (name,user,password_user,email,level,active)																						  (name_sensor,location_sensor,value_sensor,input_s,output_s)
		command = "INSERT INTO users VALUES (NULL,'%s','%s',SHA1('%s'),'%s','%d','%d',NOW());" % (name,user,password_user,email,level,active)

		handle.execute(command)
		db.commit()
	except:
		# Rollback in case there is any error
		db.rollback()

	print "\tData inserted in table:users"
	db.close()
	return "ok"

def insert_location(db_name,location,level):
	db = MySQLdb.connect(default_server,default_user,password)
	handle = db.cursor()

	try:
		command = "USE " + db_name + ";"
		handle.execute(command)
		command = "INSERT INTO locations VALUES (NULL,'%s','%d',NOW());" % (location,level)

		handle.execute(command)
		db.commit()
	except:
		# Rollback in case there is any error
		db.rollback()

	print "\tData inserted in table:location"
	db.close()
	return "ok"

if __name__ == '__main__':
	#create_db(db_name)
	#create_user_table(database_name,table_name)
	#insert_user(db_name,name,user,password_user,email,level,active)

	print "Creating database..."
	create_db("homestark");

	print "\nCreating table users..."
	create_user_table("homestark");

	print "\nCreating table sensors..."
	create_sensor_table("homestark");
	
	print "\nCreating table locations..."
	create_loc_table("homestark");

	print "\nAdding location: Quarto do Anderson in locations..."
	insert_location("homestark","Quarto do Anderson",1);

	print "\nCreating sensor light in Andersons room..."
	insert_sensor("homestark","Abajur","light",1,"Quarto do Anderson",1);

	print "\nAdding aignacio(admin) user in table..."
	insert_user("homestark","Anderson I.","aignacio","3102230","anderson.igns@gmail.com",1,1);