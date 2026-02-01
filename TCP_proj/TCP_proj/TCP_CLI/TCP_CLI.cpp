#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <cstdlib> // для system

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    string ipAddress = "127.0.0.1";
    int port = 54000;

    WSADATA data; //используем WSADATA вместо nullptr
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        cerr << "WSAStartup failed! Error code: " << WSAGetLastError() << endl;
        system("pause");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed! Error: " << WSAGetLastError() << endl;
        WSACleanup();
        system("pause");
        return 1;
    }

    DWORD timeout = 30000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    sockaddr_in hint{};
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    if (connect(sock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        cerr << "Failed to connect! Error: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        system("pause");
        return 1;
    }

    cout << "Connected to server. Type 'EXIT' to quit.\n";

    string userInput;
    while (true) {
        cout << "> ";
        getline(cin, userInput);

        if (userInput.empty()) continue;

        if (send(sock, userInput.c_str(), (int)userInput.size(), 0) == SOCKET_ERROR) {
            cerr << "Send failed!" << endl;
            break;
        }

        if (userInput == "EXIT") break;

        char buf[4096]{};
        int bytes = recv(sock, buf, sizeof(buf) - 1, 0);
        if (bytes > 0) {
            buf[bytes] = '\0';
            cout << "SERVER> " << string(buf) << endl;
        }
        else if (bytes == 0) {
            cout << "Server closed connection." << endl;
            break;
        }
        else if (WSAGetLastError() == WSAETIMEDOUT) {
            cout << "Timeout: no response from server in 30 seconds. Exiting." << endl;
            break;
        }
        else {
            cerr << "Recv error: " << WSAGetLastError() << endl;
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    cout << "Client exiting." << endl;
    system("pause");
    return 0;
}