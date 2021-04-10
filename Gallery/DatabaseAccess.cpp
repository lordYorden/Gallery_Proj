#include "DatabaseAccess.h"
#define SQL_DB_NAME "galleryDB.sqlite"
#define NOT_A_TAG 2
#define PICID_PLACE 0
#define USERID_PLACE 1
#define IS_A_TAG 2

/*
	the function get all the albums in the db using sql 
	and the function fetchAlbums
	input: none
	output: albums - all the albums in the db (const std::list<Album>)
*/
const std::list<Album> DatabaseAccess::getAlbums()
{
	std::string sqlStatement = "SELECT * FROM ALBUMS";
	return fetchAlbums(sqlStatement);
}

/*
	the function get all the albums of a given user in the db using sql
	and the function fetchAlbums
	input: none
	output: albums - all the albums of a given user in the db (const std::list<Album>)
*/
const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "SELECT * FROM ALBUMS WHERE user_id = " << user.getId() << std::endl;
	std::string sqlStatement = sqlStatementBuilder.str();
	return fetchAlbums(sqlStatement);
}

/*
* the function addes a given album to the db
* using sql
* input: album - the album to add (const Album&)
*/
void DatabaseAccess::createAlbum(const Album& album)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "INSERT INTO ALBUMS(NAME, USER_ID, CREATION_DATE)" << std::endl
	<< "VALUES('" << album.getName() << "','" << album.getOwnerId() << "','" << album.getCreationDate() << "'" << ");";
	std::string sqlStatement = sqlStatementBuilder.str();
	sqlExec(sqlStatement);
}

/*
* the function deletes the album using an sql Sttatment 
* input: albumName - the album to delete (const std::string&)
*		,userId  - the userId of which the album is of (int)
* output: none
*/
void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::stringstream sqlStatementBuilder;
	std::string sqlStatement;
	sqlStatementBuilder << "DELETE FROM TAGS" << std::endl
		<< "WHERE user_id = (SELECT user_id FROM ALBUMS WHERE name LIKE " << "'" << albumName << "'" << " AND user_id = " << userId << ");";
	sqlStatement = sqlStatementBuilder.str();
	std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
	sqlStatement = "VACUUM";
	sqlExec(sqlStatement);

	sqlStatementBuilder << "DELETE FROM PICTURES" << std::endl
		<< "WHERE album_id = (SELECT ID FROM ALBUMS WHERE name LIKE " << "'" << albumName << "'" << " AND user_id = " << userId << ");";
	sqlStatement = sqlStatementBuilder.str();
	//std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
	sqlStatement = "VACUUM";
	sqlExec(sqlStatement);

	
	sqlStatementBuilder << "DELETE FROM ALBUMS" << std::endl
		<< "WHERE name LIKE " << '"' << albumName << '"' << " AND user_id =" << userId << ";";
	sqlStatement = sqlStatementBuilder.str();
	sqlStatementBuilder.str("");
	//std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
	sqlStatement = "VACUUM";
	sqlExec(sqlStatement);
}

/*
* the function checks if the album given exist in the db
* input: albumName - the album to check if exists (const std::string&)
*		,userId - the user id of the album owner
* output: doseAlbumExists - dose the album was found in the db (bool)
*/
bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "SELECT * FROM ALBUMS WHERE name LIKE" << "'" << albumName << "'" << " AND user_id = " << userId<< std::endl;
	std::string sqlStatement = sqlStatementBuilder.str();
	bool doseExists = false;
	sqlExec(sqlStatement, &doseExists, doseExist);
	return doseExist;
}


/*
	the function return the album given using the sql db 
	and the function fetchAlbums
	input: albumName - the album to open (const std::string&)
	output: album - the album given from the db (Album)
*/
Album DatabaseAccess::openAlbum(const std::string& albumName)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "SELECT * FROM ALBUMS WHERE name LIKE " << "'" << albumName << "'" << std::endl;
	std::string sqlStatement = sqlStatementBuilder.str();
	std::list<Album> albums = fetchAlbums(sqlStatement);
	return *(albums.begin());
}

/*
* the function closes the album and released it's allocated memory
* input: pAlbum - a pointer to the album to delete (Album&)
* output: none
*/
void DatabaseAccess::closeAlbum(Album& pAlbum)
{
	//delete dosen't work so i don't know what to do here
}

/*
* the function print the the albums in the db
* *not my code it's from MemoryAccess.cpp modified
* input: none
* output: none
*/
void DatabaseAccess::printAlbums()
{
	std::list<Album> albums = getAlbums();
	if (albums.empty()) {
		throw MyException("There are no existing albums.");
	}
	std::cout << "Album list:" << std::endl;
	std::cout << "-----------" << std::endl;
	for (const Album& album : albums) {
		std::cout << std::setw(5) << "* " << album;
	}
}

/*
* the funtion adds a picture to an album in the sql db
* input: albumName - the album to add the picture into (const std::string&)
*		,picture - the picture to add to the album (const Picture&)
* output: none
*/
void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "INSERT INTO PICTURES(ID, NAME, LOCATION, CREATION_DATE, ALBUM_ID)" << std::endl
	<< "VALUES(" << picture.getId() << ",'" << picture.getName() << "', '" << picture.getPath() << "', '" << picture.getCreationDate() << "', (SELECT id from ALBUMS WHERE name LIKE '" << albumName << "'));";
	std::string sqlStatement = sqlStatementBuilder.str();
	sqlExec(sqlStatement);
}

/*
* the funtion deletes a picture from an album in the sql db
* input: albumName - the album to removce the picture from (const std::string&)
*		,pictureName - the picture to remove from the album (const std::string&)
* output: none
*/
void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "DELETE FROM PICTURES" << std::endl
		<< "WHERE name LIKE " << "'" << pictureName << "'" << " AND album_id = " << "(SELECT id from ALBUMS WHERE name LIKE '" << albumName << "');";
	std::string sqlStatement = sqlStatementBuilder.str();
	std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
}

/*
* the function tags the user to picture
* input: albumName - the album the picture is from (const std::string&)
*		,pictureName - the picture the to tag the user on (const std::string&)
*		,userId  - the user to tag (int)
* output: none
*/
void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	Album currAlbum = openAlbum(albumName);
	Picture currPicture = currAlbum.getPicture(pictureName);
	currAlbum.tagUserInPicture(userId, pictureName);
	char* errMessage = nullptr;
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "INSERT INTO TAGS(PICTURE_ID, USER_ID) VALUES('" << currPicture.getId() << "','" << userId << "');";
	std::string sqlStatement = sqlStatementBuilder.str();
	std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
}

/*
* the function untags the user to picture
* input: albumName - the album the picture is from (const std::string&)
*		,pictureName - the picture the to untag the user on (const std::string&)
*		,userId  - the user to untag (int)
* output: none
*/
void DatabaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	Album currAlbum = openAlbum(albumName);
	Picture currPicture = currAlbum.getPicture(pictureName);
	currAlbum.untagUserInPicture(userId, pictureName);
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "DELETE FROM TAGS" << std::endl
		<< "WHERE picture_id = " << currPicture.getId() << " AND user_id = " << userId << ";";
	std::string sqlStatement = sqlStatementBuilder.str();
	std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
	sqlStatement = "VACUUM";
	sqlExec(sqlStatement);
}

/*
* the function print the the users in the db
* *not my code it's from MemoryAccess.cpp modified
* input: none
* output: none
*/
void DatabaseAccess::printUsers()
{
	std::string sqlStatement = "SELECT * FROM USERS";
	std::list<User> users = fetchUsers(sqlStatement);
	std::cout << "Users list:" << std::endl;
	std::cout << "-----------" << std::endl;
	for (const auto& user : users) {
		std::cout << user << std::endl;
	}
}

/*
* the function creates a user based on a given user variable
* input: user - the user to create (User&)
* output: none
*/
void DatabaseAccess::createUser(User& user)
{
	char* errMessage = nullptr;
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "INSERT INTO USERS(ID,NAME) VALUES('" << user.getId() << "','" << user.getName() << "');";
	std::string sqlStatement = sqlStatementBuilder.str();
	std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
}

/*
* the function deletes a user based on a given user variable
* input: user - the user to create (User&)
* output: none
*/
void DatabaseAccess::deleteUser(const User& user)
{
	cleanUserData(user);
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "DELETE FROM USERS" << std::endl
		<< "WHERE name LIKE " << '"' << user.getName() << '"' << " AND id = " << user.getId() << ";";
	std::string sqlStatement = sqlStatementBuilder.str();
	std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement);
	sqlStatement = "VACUUM";
	sqlExec(sqlStatement);
}

/*
* the function checks if the user given exist in the db
* input: userId - the user to check if exists (int)
* output: doseUserExists - dose the user was found in the db (bool)
*/
bool DatabaseAccess::doesUserExists(int userId)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "SELECT * FROM USERS WHERE ID = " << userId << std::endl;
	std::string sqlStatement = sqlStatementBuilder.str();
	bool doseExists = false;
	sqlExec(sqlStatement, &doseExists, doseExist);
	return doseExist;
}

/*
* the function get a user from the sql db given its id
* input: userId - the user to get from the db (int)
* output: user - the user data from the db (User)
*/
User DatabaseAccess::getUser(int userId)
{
	std::stringstream sqlStatementBuilder;
	sqlStatementBuilder << "SELECT * FROM USERS WHERE ID = " << userId << std::endl;
	std::string sqlStatement = sqlStatementBuilder.str();
	std::list<User> users = fetchUsers(sqlStatement);
	return *(users.begin());
}

/*
* the function counts the albums owned by user using an sql statement
* input: user - the user to count it's albums (const User&)
* output: counter - the number of a albums owned by user (int)
*/
int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	std::stringstream sqlStatementBuilder;
	int counter = 0 ;
	sqlStatementBuilder << "SELECT * FROM ALBUMS WHERE user_id = " << user.getId() << std::endl;
	std::string sqlStatement = sqlStatementBuilder.str();
	//std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement, &counter, increment);
	return counter;
}

/*
* the function counts the albums tagged by user using an sql statement
* input: user - the user to count it's tagged albums (const User&)
* output: counter - the number of a albums tagged by the user (int)
*/
int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	int albumsCount = 0;
	bool found = false;
	std::list<Album> albums = getAlbums();
	for (const auto& album : albums) //for each loop
	{
		std::list<Picture> pics = album.getPictures();
		found = false;
		for (std::list<Picture>::iterator picture = pics.begin(); picture != pics.end() && !found; picture++) {
			if (picture->isUserTagged(user)) 
			{
				albumsCount++;
				found = true;
			}
		}
	}
	return albumsCount;
}

/*
* the function counts how meny tgs the user have using sql
* input: user - the user to count it's tags (const User&)
* output: counter - the number of tags the user have (int)
*/
int DatabaseAccess::countTagsOfUser(const User& user)
{
	std::stringstream sqlStatementBuilder;
	int counter = 0;
	sqlStatementBuilder << "SELECT * FROM TAGS WHERE user_id = " << user.getId() << std::endl;
	std::string sqlStatement = sqlStatementBuilder.str();
	//std::cout << sqlStatement << std::endl;
	sqlExec(sqlStatement, &counter, increment);
	return counter;
}

/*
	the function uses the other statistic funcions to clac
	avg of tags per album of a give user
	*not my code from MemoryAccess.h 
*/
float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int albumsTaggedCount = countAlbumsTaggedOfUser(user);
	if (0 == albumsTaggedCount) {
		return 0;
	}
	return static_cast<float>(countTagsOfUser(user)) / albumsTaggedCount;
}

/*
* the funtion function return the most taged user
* input: none
* output: maxUser - the most taged user info (User)
*/
User DatabaseAccess::getTopTaggedUser()
{
	std::list<User> users = getUsers();
	int maxTggs = -1, numOfTags = 0;
	User* maxUser = nullptr;
	for (auto user = users.begin(); user != users.end(); user++)
	{
		numOfTags = countTagsOfUser(*user);
		if (maxTggs < numOfTags)
		{
			maxTggs = numOfTags;
			maxUser = &(*user);
		}
	}
	if (nullptr == maxUser) {
		throw MyException("There isn't any tagged users.");
	}
	return *maxUser;
}

/*
* the funtion function return the most taged Picture
* *modified from MemoryAccsess.h
* input: none
* output: mostTaggedPic - the most taged picture info (Picture)
*/
Picture DatabaseAccess::getTopTaggedPicture()
{
	int currentMax = -1;
	Picture mostTaggedPic;
	std::list<Album> albums = getAlbums();
	std::list<Album>::iterator albumIt = albums.begin();
	for (; albumIt != albums.end(); albumIt++)
	{
		std::list<Picture> picsInAlbum = albumIt->getPictures();
		std::list<Picture>::iterator picIt = picsInAlbum.begin();
		for(; picIt != picsInAlbum.end(); picIt++)
		{
			int tagsCount = picIt->getTagsCount();
			if (currentMax < tagsCount)
			{
				mostTaggedPic = *picIt;
				currentMax = tagsCount;
			}
		}
	}
	if (currentMax < -1) {
		throw MyException("There isn't any tagged picture.");
	}
	return mostTaggedPic;
}

/*
* the function return all the taged pictures of a user
* input: user - the user to find the taged pics of (const User&)
* output: pictures - all the taged pictures of a user (std::list<Picture>)
*/
std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{

	std::list<Picture> tagedPicList;
	std::list<Album> albums = getAlbums();
	std::list<Album>::iterator albumIt = albums.begin();
	for (; albumIt != albums.end(); albumIt++)
	{
		std::list<Picture> picsInAlbum = albumIt->getPictures();
		std::list<Picture>::iterator picIt = picsInAlbum.begin();
		for (; picIt != picsInAlbum.end(); picIt++)
		{
			if (picIt->isUserTagged(user))
			{
				tagedPicList.push_back(*picIt);
			}
		}
	}
	return tagedPicList;
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

		sqlExec(sqlStatement);

		sqlStatement = "CREATE TABLE IF NOT EXISTS ALBUMS(\
			ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
			NAME TEXT NOT NULL,\
			CREATION_DATE DATE NOT NULL,\
			USER_ID INTEGER NOT NULL,\
			FOREIGN KEY(USER_ID) REFERENCES USERS(ID)\
			); ";
		sqlExec(sqlStatement);

		sqlStatement = "CREATE TABLE IF NOT EXISTS PICTURES(\
			ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
			NAME TEXT NOT NULL,\
			LOCATION TEXT NOT NULL,\
			CREATION_DATE DATE NOT NULL,\
			ALBUM_ID INTEGR NOT NULL,\
			FOREIGN KEY(ALBUM_ID) REFERENCES ALBUMS(ID)\
			); ";
		sqlExec(sqlStatement);
		
		sqlStatement = "CREATE TABLE IF NOT EXISTS TAGS(\
			ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
			PICTURE_ID INTEGER NOT NULL,\
			USER_ID INTEGER NOT NULL,\
			FOREIGN KEY(PICTURE_ID) REFERENCES PICTURES(ID)\
			FOREIGN KEY(USER_ID) REFERENCES USERS(ID)\
			); ";

		sqlExec(sqlStatement);
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
	std::string sqlStatement = "DELETE FROM ALBUMS;";
	sqlExec(sqlStatement);
	sqlStatement = "DELETE FROM USERS;";
	sqlExec(sqlStatement);
	sqlStatement = "DELETE FROM TAGS;";
	sqlExec(sqlStatement);
	sqlStatement = "DELETE FROM PICTURES;";
	sqlExec(sqlStatement);
	sqlStatement = "VACUUM";
	sqlExec(sqlStatement);
}

/*
** helper method **
* the function gets an sql statment and execut the statment
* input: sqlStatement - the sql statment to exc (std::string&)
* output: none
*/
void DatabaseAccess::sqlExec(std::string& sqlStatement)
{
	char* errMessage = nullptr;
	bool res = sqlite3_exec(_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Failed to execut the sql statement" << std::endl;
		std::cout << "Resson: " << errMessage << std::endl;
	}
}

/*
** helper method **
* the function gets an sql statment and execut the statment
* input: sqlStatement - the sql statment to exc (std::string&)
*		,callback - callback function for the statment (function* callback)
* output: none
*/
void DatabaseAccess::sqlExec(std::string& sqlStatement, void* data, int(*callback)(void*, int, char**, char**))
{
	char* errMessage = nullptr;
	bool res = sqlite3_exec(_db, sqlStatement.c_str(), callback, data, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "Failed to execut the sql statement" << std::endl;
		std::cout << "Resson: " << errMessage << std::endl;
	}
}

/*
** callback function **
* the function gets an sql db row and creates an album and push
* it into a list
* input: data - the list to push into (std::list<Album>*) 
*		,argc - the number of columns in table (int)
*		,argv - the row's data. Can be user info and such... (char**)
*		,azColName - the columns names in the table (char**)
* output: exitCode - the function exit code (int) 
*/
int DatabaseAccess::albumCreate(void* data, int argc, char** argv, char** azColName)
{
	Album album;
	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "NAME") {
			album.setName(argv[i]);
		}
		else if (std::string(azColName[i]) == "USER_ID") {
			album.setOwner(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "CREATION_DATE") {
			album.setCreationDate(argv[i]);
		}
	}
	((std::list<Album>*)data)->push_back(album);
	return 0;
}

/*
** callback function **
* the function gets an sql db row and creates an picture and adds
* it into an album
* input: data - the album to add to (Album*)
*		,argc - the number of columns in table (int)
*		,argv - the row's data. Can be user info and such... (char**)
*		,azColName - the columns names in the table (char**)
* output: exitCode - the function exit code (int)
*/
int DatabaseAccess::pictureCreate(void* data, int argc, char** argv, char** azColName)
{
	Album* album = (Album*)data;
	if (argc > NOT_A_TAG)
	{
		Picture picture;
		for (int i = 0; i < argc; i++) {
			if (std::string(azColName[i]) == "NAME") {
				picture.setName(argv[i]);
			}
			else if (std::string(azColName[i]) == "ID") {
				picture.setId(atoi(argv[i]));
			}
			else if (std::string(azColName[i]) == "CREATION_DATE") {
				picture.setCreationDate(argv[i]);
			}
			else if (std::string(azColName[i]) == "LOCATION") {
				picture.setPath(argv[i]);
			}
		}
		album->addPicture(picture);
	}
	return 0;
}

/*
** callback function **
* the function gets an sql db row and creates an User and push
* it into a list
* input: data - the list to push into (std::list<User>*)
*		,argc - the number of columns in table (int)
*		,argv - the row's data. Can be user info and such... (char**)
*		,azColName - the columns names in the table (char**)
* output: exitCode - the function exit code (int)
*/
int DatabaseAccess::userCreate(void* data, int argc, char** argv, char** azColName)
{
	User user;
	for (int i = 0; i < argc; i++) {
		if (std::string(azColName[i]) == "ID") {
			user.setId(atoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "NAME") {
			user.setName(argv[i]);
		}
	}
	((std::list<User>*)data)->push_back(user);
	return 0;
}

/*
** callback function **
* the function gets an sql db row and creates a Tag adds
* it into a picture using the album
* input: data - the album to add the tag into (Album*)
*		,argc - the number of columns in table (int)
*		,argv - the row's data. Can be user info and such... (char**)
*		,azColName - the columns names in the table (char**)
* output: exitCode - the function exit code (int)
*/
int DatabaseAccess::tagMaker(void* data, int argc, char** argv, char** azColName)
{
	if (argc == IS_A_TAG)
	{
		Album* album = (Album*)data;
		bool found = false;
		int picId = atoi(argv[PICID_PLACE]);
		int userId = atoi(argv[USERID_PLACE]);
		std::list<Picture> pictures = album->getPictures();
		std::list<Picture>::iterator pic = pictures.begin();

		for (; pic != pictures.end() && !found; pic++)
		{
			if (pic->getId() == picId)
			{
				found = true;
				if (!pic->isUserTagged(userId))
				{
					album->tagUserInPicture(userId, pic->getName());
				}
			}
		}
	}
	return 0;
}

/*
** callback function **
* the funtion changes the value of a boolean given 
* if called
* input: data - the boolen to check a Existence in db of a certain row (bool*)
*		,argc - the number of columns in table (int)
*		,argv - the row's data. Can be user info and such... (char**)
*		,azColName - the columns names in the table (char**)
* output: exitCode - the function exit code (int)
*/
int DatabaseAccess::doseExist(void* data, int argc, char** argv, char** azColName)
{
	bool* isExist = (bool*)data;
	*isExist = true;
	return 0;
}

/*
** callback function **
* the funtion incrament the value of an integer if called
* input: data - the integer to count the rows (int*)
*		,argc - the number of columns in table (int)
*		,argv - the row's data. Can be user info and such... (char**)
*		,azColName - the columns names in the table (char**)
* output: exitCode - the function exit code (int)
*/
int DatabaseAccess::increment(void* data, int argc, char** argv, char** azColName)
{
	int* inc = (int*)data;
	(*inc)++;
	return 0;
}

/*
* the function fetchs the albums given from the db using the sql statement given
* input: sqlStatement - the statement to run on the db (std::string&) 
* output: albumsList - all the albums given from the data base filled
*		  with the pictures and tags (std::list<Album>)
*/
std::list<Album> DatabaseAccess::fetchAlbums(std::string& sqlStatement)
{
	std::stringstream sqlStatementBuilder;
	std::list<Album> listAlbum;
	std::list<Picture> picsInAlbum;
	void* data = &listAlbum;
	sqlExec(sqlStatement, data, albumCreate);

	//filing every album in listAlbum with pictures and tags
	for (auto albumIt = listAlbum.begin(); albumIt != listAlbum.end(); albumIt++)
	{
		//creates an sql statment to get the pictures of a certain album
		sqlStatementBuilder << "SELECT * FROM Pictures" << std::endl
			<< "WHERE album_id = (SELECT ID FROM ALBUMS WHERE name LIKE " << '"' << albumIt->getName() << '"' << " AND user_id = " << albumIt->getOwnerId() << ");" << std::endl;
		sqlStatement = sqlStatementBuilder.str();
		sqlStatementBuilder.str("");
		//*debug* std::cout << sqlStatement << std::endl;
		data = &(*albumIt);
		sqlExec(sqlStatement, data, pictureCreate);

		//gets the pictures and fills them with the tags
		picsInAlbum = albumIt->getPictures();
		for (auto picIt = picsInAlbum.begin(); picIt != picsInAlbum.end(); picIt++)
		{
			//gets the tags of a certain picture
			sqlStatementBuilder << "SELECT * FROM TAGS WHERE picture_id = " << picIt->getId() << ";";
			sqlStatement = sqlStatementBuilder.str();
			sqlStatementBuilder.str("");
			//*debug* std::cout << sqlStatement << std::endl;
			data = &(*albumIt);
			sqlExec(sqlStatement, data, tagMaker);
			//*debug* std::cout << "album: " << albumIt->getName() << " name: " << picIt->getName() << " id: " << picIt->getId() << " number of tags: " << picIt->getTagsCount() << std::endl;
		}
	}
	return listAlbum;
}

/*
* the function fetchs the users given from the db using the sql statement given
* input: sqlStatement - the statement to run on the db (std::string&)
* output: albumsList - all the user given from the db (std::list<User>)
*/
std::list<User> DatabaseAccess::fetchUsers(std::string& sqlStatement)
{
	std::list<User> listUser;
	void* data = &listUser;
	sqlExec(sqlStatement, data, userCreate);
	return listUser;
}

/*
* the function uses fetch users to give all the users in db
* input: none
* output: users - all the users in db (std::list<User>)
*/
std::list<User> DatabaseAccess::getUsers()
{
	std::string sqlStatement = "SELECT * FROM USERS";
	return fetchUsers(sqlStatement);
}

/*
* when a user gets delete makes sure all of it's pictures and tags
* all gets deleted
* input: user - the user to delete (const User&)
* output: none
*/
void DatabaseAccess::cleanUserData(const User& user)
{
	std::list<Album> albums = getAlbumsOfUser(user);
	for (auto album = albums.begin(); album != albums.end(); album++)
	{
		deleteAlbum(album->getName(), user.getId());
	}
}
