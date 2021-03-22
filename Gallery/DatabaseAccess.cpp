#include "DatabaseAccess.h"
#define SQL_DB_NAME "galleryDB.sqlite"

/*
* the function closes the album and released it's allocated memory
* input: pAlbum - a pointer to the album to delete deleted (Album&)
* output: none
*/
void DatabaseAccess::closeAlbum(Album& pAlbum)
{
	this->_albums.remove(pAlbum);
}

/*
* the functiion opens the DB of the gallery
* if the table of the BD arn't there creates them
* input: none
* output: none
*/
bool DatabaseAccess::open()
{
	std::string dbFileName = SQL_DB_NAME;
	int doesFileExist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &_db);
	if (res != SQLITE_OK)
	{
		_db = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		return false;
	}

	//creates a table if there isn't
	if (doesFileExist == -1) {
		char* errMessage = nullptr;
		std::string sqlStatement = "CREATE TABLE IF NOT EXISTS USERS(\
			ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
			NAME TEXT NOT NULL\
			); ";

		sqlExec(sqlStatement, errMessage);

		sqlStatement = "CREATE TABLE IF NOT EXISTS ALBUMS(\
			ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
			NAME TEXT NOT NULL,\
			CREATION_DATE DATE NOT NULL,\
			USER_ID INTEGER NOT NULL,\
			FOREIGN KEY(USER_ID) REFERENCES USERS(ID)\
			); ";
		sqlExec(sqlStatement, errMessage);

		sqlStatement = "CREATE TABLE IF NOT EXISTS PICTURES(\
			ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
			NAME TEXT NOT NULL,\
			LOCATION TEXT NOT NULL,\
			CREATION_DATE DATE NOT NULL,\
			ALBUM_ID INTEGR NOT NULL,\
			FOREIGN KEY(ALBUM_ID) REFERENCES ALBUMS(ID)\
			); ";
		sqlExec(sqlStatement, errMessage);
		
		sqlStatement = "CREATE TABLE IF NOT EXISTS TAGS(\
			ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
			PICTURE_ID INTEGER NOT NULL,\
			USER_ID INTEGER NOT NULL,\
			FOREIGN KEY(PICTURE_ID) REFERENCES PICTURES(ID)\
			FOREIGN KEY(USER_ID) REFERENCES USERS(ID)\
			); ";

		sqlExec(sqlStatement, errMessage);
	}
	return true;
}

/*
* the function closes the DB
* input: none
* output: none
*/
void DatabaseAccess::close()
{
	sqlite3_close(_db);
	_db = nullptr;
}

/*
* the function clears the DB
* and clears the user and albums lists
* input: none
* output: none
*/
void DatabaseAccess::clear()
{
	char* errMessage = nullptr;
	std::string sqlStatement = "DELETE FROM ALBUMS;";
	sqlExec(sqlStatement, errMessage);
	sqlStatement = "DELETE FROM USERS;";
	sqlExec(sqlStatement, errMessage);
	sqlStatement = "DELETE FROM TAGS;";
	sqlExec(sqlStatement, errMessage);
	sqlStatement = "DELETE FROM PICTURES;";
	sqlExec(sqlStatement, errMessage);
	sqlStatement = "VACUUM";
	sqlExec(sqlStatement, errMessage);

	this->_albums.clear();
	this->_users.clear();
}

/*
* the function gets an sql statment and execut the statment
* input: none
* output: none
*/
void DatabaseAccess::sqlExec(std::string& sqlStatement, char* errMessage)
{
	//std::cout << sqlStatement << std::endl;
	bool res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Failed to execut the sql statement" << std::endl;
		std::cout << "Resson: " << errMessage << std::endl;
	}
}