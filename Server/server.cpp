#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Connect MySQL
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include "dbconfig.h"


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

sql::Connection* connectToDatebase(const std::string& dbServer, const std::string& dbUsername, 
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

bool authenticateUser(const std::string& client_name, const std::string& client_pass, sql::Connection* con)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;
        
        std::string query = "SELECT * FROM users WHERE username = ? and password = ?";

        pstmt = con->prepareStatement(query);

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

std::vector<std::string> showRooms(const std::string& client_name, sql::Connection* con)
{
    try 
    {
        std::vector<std::string> rooms;
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con->prepareStatement(
            "SELECT rooms.name FROM rooms INNER JOIN users ON rooms.user_id = users.user_id WHERE users.username = ?");

        pstmt->setString(1, client_name);
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

std::string serializerVector(const std::vector<std::string>& vector)
{
    std::ostringstream oss;
    for (auto elem : vector)
    {
        oss << elem << '\n';
    }

    return oss.str();
}

int getUserId(const std::string& client_name, sql::Connection* con)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;
        int result;

        pstmt = con->prepareStatement("SELECT user_id FROM users WHERE username = ?");
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

bool createRoom(const std::string& room_name, const std::string& client_name, sql::Connection* con)
{
    try
    {
        sql::PreparedStatement* pstmt;
        sql::ResultSet* res;

        pstmt = con->prepareStatement("INSERT INTO rooms(name, user_id) VALUES(?, ?)");
        pstmt->setString(1, room_name);
        pstmt->setInt(2, getUserId(client_name, con));

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


int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Connect to db

    sql::Connection* con = connectToDatebase(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME);

    // Authenticate data

    std::string client_name;
    std::string client_pass;

    while (true)
    {
        // Receive username and password from client
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            client_name = recvbuf;
            std::cout << client_name << '\n';
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("name recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            client_pass = recvbuf;
            std::cout << client_pass << '\n';
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("pass recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        // Authenticate

        if (authenticateUser(client_name, client_pass, con))
        {
            std::cout << "Authentication passed\n";

            iResult = send(ClientSocket, "1", 1, 0);
            if (iResult == SOCKET_ERROR) {
                printf("status send failed with error");
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            break;
        }
        else
        {
            std::cout << "Authentication failed\n";

            iResult = send(ClientSocket, "0", 1, 0);
            if (iResult == SOCKET_ERROR) {
                printf("status send failed with error");
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }
    }

    // Show rooms information

    std::vector<std::string> rooms;

    rooms = showRooms(client_name, con);

    if (!rooms.empty())
    {
        iResult = send(ClientSocket, "1", 1, 0);
        if (iResult == SOCKET_ERROR) {
            printf("rooms send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        iResult = send(ClientSocket, serializerVector(rooms).c_str(), serializerVector(rooms).size(), 0);
        if (iResult == SOCKET_ERROR) {
            printf("rooms send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }
    else
    {
        iResult = send(ClientSocket, "0", 1 , 0);
        if (iResult == SOCKET_ERROR) {
            printf("rooms send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        std::string room_name;

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            recvbuf[iResult] = '\0';
            room_name = recvbuf;
            std::cout << room_name << '\n';
        }

        createRoom(room_name, client_name, con);
        
    }

    


    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}