#pragma once
#include <iostream>
#include <sstream>
#include "Album.h"
#include "User.h"
#include "IDataAccess.h"
#include "sqlite3.h"
#include "io.h"
#include "MyException.h"

class DatabaseAccess : public IDataAccess
{

public:
	DatabaseAccess() = default;
	virtual ~DatabaseAccess() = default;

	// album related
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(const Album& album); //override;
	void deleteAlbum(const std::string& albumName, int userId) override;
	bool doesAlbumExists(const std::string& albumName, int userId) override;
	Album openAlbum(const std::string& albumName) override;
	void closeAlbum(Album& pAlbum) override;
	void printAlbums() override;

	//// picture related
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;
	void tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;
	void untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId) override;

	//// user related
	void printUsers() override;
	void createUser(User& user) override;
	void deleteUser(const User& user) override;
	bool doesUserExists(int userId) override;
	User getUser(int userId) override;

	//// user statistics
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user) override;
	float averageTagsPerAlbumOfUser(const User& user) override;

	//// queries
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	bool open() override;
	void close() override;
	void clear() override;

private:
	Album* _OpenAlbum;
	sqlite3* _db;

	//execute sql statements with or with out a callback
	void sqlExec(std::string& sqlStatement);
	void sqlExec(std::string& sqlStatement, void* data, int(*callback)(void*, int, char**, char**));
	//callabcks
	static int albumCreate(void* data, int argc, char** argv, char** azColName);
	static int pictureCreate(void* data, int argc, char** argv, char** azColName);
	static int userCreate(void* data, int argc, char** argv, char** azColName);
	static int tagMaker(void* data, int argc, char** argv, char** azColName);
	static int doseExist(void* data, int argc, char** argv, char** azColName);
	static int increment(void* data, int argc, char** argv, char** azColName);
	//fetchers from db
	std::list<Album> fetchAlbums(std::string& sqlStatement);
	std::list<User> fetchUsers(std::string& sqlStatement);
	//get all users in db
	std::list<User> getUsers();
	// fully deletes user
	void cleanUserData(const User& user);
};
