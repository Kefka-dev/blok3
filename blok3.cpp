// blok3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma warning(disable:4996)
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")

#define TRUE 1
#define FALSE 0
#define DEFAULT_BUFLEN 4096

int initialSettings(struct addrinfo **result, struct addrinfo **ptr, struct addrinfo *hints, const char* ipAddress, const char* portNumber)
{
    //struktura na pracu s Winsock
    WSADATA wsaData; 
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0)
    {
        printf("WSAStartup failed : % d\n", iResult);
        return 1;
    }

    ZeroMemory(hints, sizeof(struct addrinfo));
    (*hints).ai_family = AF_UNSPEC;
    (*hints).ai_socktype = SOCK_STREAM;
    (*hints).ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(ipAddress, portNumber, hints, result);
    if (iResult != 0)     //kontrola, ci nenastala chyba
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    else
        printf("getaddrinfo didn't fail...\n");

    return 0;
}

int connectToServer(SOCKET *p_ConnectSocket, struct addrinfo **p_result, struct addrinfo **p_ptr)
{
    int iResult;
    *p_ptr = *p_result;

    *p_ConnectSocket = socket((*p_ptr)->ai_family, (*p_ptr)->ai_socktype, (*p_ptr)->ai_protocol);

    if (*p_ConnectSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(*p_result);
        WSACleanup();
        return 1;
    }
    else
    {
        printf("Error didn't occur...\n");
    }

    // pokus o pripojenie sa

    iResult = connect(*p_ConnectSocket, (*p_ptr)->ai_addr, (int)(*p_ptr)->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("Not connected to the server...\n");
    }
    else
    {
        printf("Connected to server!");
    }

    if (iResult == SOCKET_ERROR)    //osetrenie chyboveho stavu
    {
        closesocket(*p_ConnectSocket);
        *p_ConnectSocket = INVALID_SOCKET;
        WSACleanup();
        return 1;
    }

    return 0;
}

int sendString(char *sendBuffer, SOCKET *p_ConnectSocket)
{
    int iResult;
    iResult = send(*p_ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);

    if (iResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(*p_ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes sent: %ld\n", iResult);
    return 0;
}

int recieveData(char* recvBuff, int recvBuffLen, SOCKET* p_ConnectSocket)
{
    int iResult;

    iResult = recv(*p_ConnectSocket, recvBuff, recvBuffLen, 0);

    if (iResult > 0)
    {
        printf("Bytes received: %d\n", iResult);
    }
    else if (iResult == 0)
    {
        printf("Connection closed\n");
    }
    else 
    {
        printf("recv failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    return 0;
}

int main()
{
    
    int err;

    struct addrinfo* result = NULL, * ptr = NULL;     //struktura pre pracu s adresami
    struct addrinfo hints;
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
