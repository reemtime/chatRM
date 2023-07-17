#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

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

    const std::string dbServer = "tcp://127.0.0.1:3306";
    const std::string dbUser = DB_USERNAME;
    const std::string dbPassword = DB_PASSWORD;

    sql::Driver* driver;
    sql::Connection* con;


    
    try
    {
        driver = get_driver_instance();
        con = driver->connect(dbServer, dbUser, dbPassword);
    }
    catch (sql::SQLException e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << '\n';
        system("pause");
        exit(1);
    }

    

    // Authentication data

    std::string username;
    std::string password;

    while (true)
    {
        // Receive username and password from client
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            username = recvbuf;
            std::cout << username << '\n';
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            password = recvbuf;
            std::cout << password << '\n';
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

        // Authenticate

        try
        {
            sql::Statement* stmt;
            sql::ResultSet* res;

            con->setSchema("chat");
            stmt = con->createStatement();

            std::string query = "SELECT * FROM users WHERE username = '" + std::string(username)
                + "' AND password = '" + std::string(password) + "'";
            std::cout << query << '\n';
            res = stmt->executeQuery(query);

            if (res->next())
            {
                std::cout << "Authentication passed\n";
                const char status{1};
                send(ClientSocket, &status, sizeof(status), 0);
                break;
            }
            else
            {
                std::cout << "Authentication failed\n";
                const char status{ 0 };
                send(ClientSocket, &status, sizeof(status), 0);
            }
            delete stmt;
            delete res;
        }
        catch (sql::SQLException e)
        {
            std::cout << "Could recive data from SQL server: " << e.what() << '\n';
            system("pause");
            exit(1);
        }
    }

    // Sent rooms information


    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;
    pstmt = con->prepareStatement(
        "SELECT rooms.name FROM rooms INNER JOIN users ON rooms.user_id = users.user_id WHERE users.username = ?");
    pstmt->setString(1, username);
    res = pstmt->executeQuery();

    while (res->next())
    {
        std::cout << res->getString("name") << '\n';
    }
    delete res;
    delete pstmt;


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