// blok3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma warning(disable:4996)
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "WS2_32.lib")

#define TRUE 1
#define FALSE 0
#define DEFAULT_BUFLEN 4096
#define GREEN 10
#define BLUE 9

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
        printf("Connected to server!\n");
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
    //pointer for end of string
    char* p;
    iResult = recv(*p_ConnectSocket, recvBuff, recvBuffLen, 0);

    if (iResult > 0)
    {
        printf("Bytes received: %d\n", iResult);
    }
    else if (iResult == 0)
    {
        printf("Connection closed\n");
        return 2;
    }
    else 
    {
        printf("recv failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    p = strchr(recvBuff, '\n');
    *p = '\0';

    return 0;
}

void print(const char* string) 
{
    int stringLen;
    stringLen = strlen(string);

    for (int i = 0; i < stringLen; i++)
    {
        printf("%c", string[i]);
        Sleep(100);
    }
    printf("\n");
}

int codeFromId(char* idString, int *storeCodeHere)
{
    int sum = 0, readDigit = 0, digitToDevideAt = 5, digitToDevideAtValue = 1;
    unsigned int idStrLen = strlen(idString);
    for (int digit = 0; digit < (idStrLen-2); digit++)
    {
        if (isdigit(idString[digit]) != FALSE)
        {
            sscanf((idString+digit), "%1d", &readDigit);
            sum = sum + readDigit;
            if (digit == (digitToDevideAt-1))
            {
                if (readDigit != 0) 
                {
                    digitToDevideAtValue = readDigit;
                }
                else
                {
                    digitToDevideAtValue = 9;
                }
            }
        }
        else 
        {
            printf("Nie je cislo\n");
            return 1;
        }
    }
    sum = sum % digitToDevideAtValue;
    *storeCodeHere = sum;
    return 0;
}

int main()
{
    
    int err;

    struct addrinfo* result = NULL, * ptr = NULL;     //struktura pre pracu s adresami
    struct addrinfo hints;

    int recvSendBuffLen = DEFAULT_BUFLEN;
    char recvBuf[DEFAULT_BUFLEN];
    char sendBuf[DEFAULT_BUFLEN];
    strcpy(sendBuf, "cc");

    err = initialSettings(&result, &ptr, &hints, "147.175.115.34", "777");
    if (err == TRUE)
    {
        return 1;
    }

    //Vytvorenie socketu a pripojenie sa
    SOCKET ConnectSocket = INVALID_SOCKET;

    err = connectToServer(&ConnectSocket, &result, &ptr);
    if (err == TRUE)
    {
        return 1;
    }
    
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleOutputCP(CP_UTF8);
    int iResult;
    do
    {
        iResult = recieveData(recvBuf, recvSendBuffLen, &ConnectSocket);
        SetConsoleTextAttribute(hConsole, GREEN);
        print(recvBuf);

        iResult = sendString(sendBuf, &ConnectSocket);
        
        SetConsoleTextAttribute(hConsole, BLUE);
        fgets(sendBuf, recvSendBuffLen, stdin);
        //printf("string na odoslanie %s\n", sendBuf);
        iResult = sendString(sendBuf, &ConnectSocket);

    } while (iResult == 0);

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
