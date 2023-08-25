#include "db.h"

db_server::db_server()
{
    con_ = connectToDatebase(db::db_server, db::db_username, db::db_password, db::db_name);
    createTables();
}

db_server::~db_server()
{
    delete con_;
}

bool db_server::createTables()
{
    try
    {
        sql::Statement* stmt;

        stmt = con_->createStatement();

        stmt->execute("CREATE TABLE IF NOT EXISTS rooms(room_id INT PRIMARY KEY AUTO_INCREMENT, \
                                name VARCHAR(255) NOT NULL);");

        std::cout << "Table rooms has been created" << std::endl;

        stmt->execute("CREATE TABLE IF NOT EXISTS users(user_id INT PRIMARY KEY AUTO_INCREMENT, \
                                username VARCHAR(255) NOT NULL, \
                                password VARCHAR(255) NOT NULL);");

        std::cout << "Table users has been created" << std::endl;

        stmt->execute("CREATE TABLE IF NOT EXISTS messages(message_id INT PRIMARY KEY AUTO_INCREMENT, \
                                room_id INT, user_id INT, sent_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
                                message_text TEXT, \
                                FOREIGN KEY(room_id) REFERENCES rooms(room_id) ON DELETE CASCADE, \
                                FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE);");

        std::cout << "Table messages has been created" << std::endl;

        stmt->execute("CREATE TABLE IF NOT EXISTS members(member_id INT PRIMARY KEY AUTO_INCREMENT, \
                                room_id INT, user_id INT, \
                                FOREIGN KEY(room_id) REFERENCES rooms(room_id) ON DELETE CASCADE, \
                                FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE);");

        std::cout << "Table members has been created" << std::endl;

        delete stmt;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Creating tables failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

bool db_server::registerUser(const std::string& client_name, const std::string& client_pass)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con_->prepareStatement("INSERT INTO users(username, password) VALUES(?, ?)");
        pstmt->setString(1, client_name);
        pstmt->setString(2, client_pass);

        res = pstmt->executeQuery();

        bool result = res;

        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Register error: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

bool db_server::authenticateUser(const std::string& client_name, const std::string& client_pass)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        std::string query = "SELECT * FROM users WHERE username = ? and password = ?";

        pstmt = con_->prepareStatement(query);

        pstmt->setString(1, client_name);
        pstmt->setString(2, client_pass);

        res = pstmt->executeQuery();
        bool result = res->next();

        delete pstmt;
        delete res;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Authenticatate user failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

std::vector<std::string> db_server::showRooms(const std::string& client_name)
{
    try
    {
        std::vector<std::string> rooms;
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con_->prepareStatement(
            "SELECT rooms.name FROM members INNER JOIN rooms ON rooms.room_id = members.room_id WHERE members.user_id = ?");

        pstmt->setInt(1, getUserId(client_name));
        res = pstmt->executeQuery();

        while (res->next())
        {
            rooms.push_back(res->getString("name"));
        }

        delete res;
        delete pstmt;

        return rooms;
    }

    catch (sql::SQLException e)
    {
        std::cout << "Show rooms failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

int db_server::getUserId(const std::string& client_name)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;
        int result;

        pstmt = con_->prepareStatement("SELECT user_id FROM users WHERE username = ?");
        pstmt->setString(1, client_name);

        res = pstmt->executeQuery();
        while (res->next())
        {
            result = res->getInt(1);
        }


        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Get user_id failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

int db_server::getRoomId(const std::string& room_name)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;
        int result;

        pstmt = con_->prepareStatement("SELECT room_id FROM rooms WHERE name = ?");
        pstmt->setString(1, room_name);

        res = pstmt->executeQuery();
        while (res->next())
        {
            result = res->getInt(1);
        }


        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Get room_id failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

bool db_server::createRoom(const std::string& room_name)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con_->prepareStatement("INSERT INTO rooms(name) VALUES(?)");
        pstmt->setString(1, room_name);

        res = pstmt->executeQuery();

        bool result = res;

        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Create room failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

bool db_server::deleteRoom(const std::string& room_name)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con_->prepareStatement("DELETE FROM rooms WHERE name = ?");
        pstmt->setString(1, room_name);

        res = pstmt->executeQuery();

        bool result = res;

        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Delete room failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

struct db_server::MessageFromRoom
{
    std::string username;
    std::string text;
    std::string sent_at;

    std::string msgToString()
    {
        std::ostringstream oss;

        oss << username << ": " << text << " (" << sent_at << ")\n";
        return oss.str();
    }
};

bool db_server::addMember(const std::string& room_name, const std::string& user_name)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con_->prepareStatement("INSERT INTO members(room_id, user_id) VALUES(?, ?)");
        pstmt->setInt(1, getRoomId(room_name));
        pstmt->setInt(2, getUserId(user_name));

        res = pstmt->executeQuery();

        bool result = res;

        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Error add the member: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

bool db_server::kickMember(const std::string& room_name, const std::string& user_name)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con_->prepareStatement("DELETE FROM members WHERE room_id = ? AND user_id = ?");
        pstmt->setInt(1, getRoomId(room_name));
        pstmt->setInt(2, getUserId(user_name));

        res = pstmt->executeQuery();

        bool result = res;

        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Error add the member: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

bool db_server::addMessage(const std::string& user_name, const std::string& room_name, const std::string& message)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con_->prepareStatement("INSERT INTO messages(user_id, room_id, message_text) VALUES(?, ?, ?)");
        pstmt->setInt(1, getUserId(user_name));
        pstmt->setInt(2, getRoomId(room_name));
        pstmt->setString(3, message);

        res = pstmt->executeQuery();

        bool result = res;

        delete res;
        delete pstmt;

        return result;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Error receiving the message: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}


std::vector<std::string> db_server::joinToRoom(const std::string& room_name)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;
        MessageFromRoom msg;

        std::vector<std::string> messages;


        pstmt = con_->prepareStatement("SELECT users.username, messages.message_text, messages.sent_at \
                                        FROM messages INNER JOIN users ON messages.user_id = users.user_id \
                                        INNER JOIN rooms ON messages.room_id = rooms.room_id WHERE messages.room_id = ?;");
        pstmt->setInt(1, getRoomId(room_name));

        res = pstmt->executeQuery();

        while (res->next())
        {
            msg.username = res->getString(1);
            msg.text = res->getString(2);
            msg.sent_at = res->getString(3);

            messages.push_back(msg.msgToString());
        }

        delete res;
        delete pstmt;

        return messages;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Join room failed: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}

sql::Connection* db_server::connectToDatebase(const std::string& dbServer, const std::string& dbUsername,
                                    const std::string& dbPassword, const std::string& dbName)
{
    sql::Driver* driver;
    sql::Connection* con;

    try
    {
        driver = get_driver_instance();
        con = driver->connect(dbServer, dbUsername, dbPassword);
        con->setSchema(dbName);
        std::cout << "Database is connected.\n";
        return con;
    }
    catch (sql::SQLException e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << '\n';
        system("pause");
        exit(1);
    }
}
