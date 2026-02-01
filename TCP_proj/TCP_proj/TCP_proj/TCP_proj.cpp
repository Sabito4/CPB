// TCP Server with exit condition
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <chrono>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        cerr << "Can't initialize Winsock!" << endl;
        return 1;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        cerr << "Can't create socket!" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        cerr << "Bind failed!" << endl;
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    if (listen(listening, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed!" << endl;
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    cout << "Server listening on port 54000...\n";

    bool serverRunning = true;
    while (serverRunning) {
        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed!" << endl;
            continue;
        }

        char host[NI_MAXHOST]{}, service[NI_MAXSERV]{};
        if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
            cout << "Client " << host << ":" << service << " connected." << endl;
        }
        else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            cout << "Client " << host << ":" << ntohs(client.sin_port) << " connected." << endl;
        }

        char buf[4096];
        while (true) {
            ZeroMemory(buf, 4096);
            int bytesReceived = recv(clientSocket, buf, sizeof(buf) - 1, 0);
            if (bytesReceived <= 0) {
                if (bytesReceived == 0)
                    cout << "Client disconnected." << endl;
                else
                    cerr << "Recv error." << endl;
                break;
            }

            buf[bytesReceived] = '\0';
            string msg(buf);
            cout << "Client: " << msg << endl;

            // Условие выхода
            if (msg == "EXIT") {
                send(clientSocket, "Goodbye!", 8, 0);
                break;
            }
            if (msg == "SHUTDOWN") {
                send(clientSocket, "Server shutting down...", 23, 0);
                serverRunning = false;
                break;
            }

            // Эхо-ответ
            send(clientSocket, buf, bytesReceived, 0);
        }

        closesocket(clientSocket);
        if (!serverRunning) break;
    }

    closesocket(listening);
    WSACleanup();
    cout << "Server stopped." << endl;
    return 0;
}