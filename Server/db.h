#pragma once

#include "config.h"
#include <iostream>
#include <vector>
#include <sstream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

class db_server
{
public:

	db_server();
	~db_server();

	struct MessageFromRoom;

	bool createTables();

	bool registerUser(const std::string& client_name, const std::string& client_pass);

	bool authenticateUser(const std::string& client_name, const std::string& client_pass);

	bool createRoom(const std::string& room_name);

	bool deleteRoom(const std::string& room_name);

	std::vector<std::string> showRooms(const std::string& client_name);

	std::vector<std::string> joinToRoom(const std::string& room_name);

	int getUserId(const std::string& client_name);

	int getRoomId(const std::string& room_name);

	bool addMember(const std::string& room_name, const std::string& user_name);

	bool kickMember(const std::string& room_name, const std::string& user_name);

	bool addMessage(const std::string& user_name, const std::string& room_name, const std::string& message);

private:

	sql::Connection* connectToDatebase(const std::string& dbServer, const std::string& dbUsername,
		const std::string& dbPassword, const std::string& dbName);

	sql::Connection* con_;
};