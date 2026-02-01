#include <iostream>
#include <WS2tcpip.h> // WINAPI библиотека сокетов

// Подключаем библиотеку на этапе компоновки
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
    // Инициализируем WinSock
    WSADATA data;
    // Запускаем сокет версии 2.2
    int wsOk = WSAStartup(MAKEWORD(2, 2), &data);
    if (wsOk != 0)
    {
        cout << "Can't start Winsock! " << wsOk;
        return;
    }

    // Создаем hint сервера
    sockaddr_in server;
    server.sin_family = AF_INET; // AF_INET = IPv4 addresses
    server.sin_port = htons(54000); // Задаем порт
    // Функция преобразует строку ip в массив байт
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    // Создаем сокет с типом данных SOCK_DGRAM
    SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

    string msg = "";
    while (true)
    {
        msg.clear();
        cin >> msg; // читаем из консоли

        // Отправляем сообщение
        int sendOk = sendto(out, msg.c_str(), msg.size() + 1, 0, (sockaddr*)&server, sizeof(server));
        if (sendOk == SOCKET_ERROR)
        {
            cout << "That didn't work! " << WSAGetLastError() << endl;
        }
    }

    // Закрываем сокет
    closesocket(out);
    WSACleanup();
}