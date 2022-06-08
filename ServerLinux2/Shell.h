#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#else
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif


#include "LinkStorage.h"
#include "ChatBox.h"
#include "User.h"
#include "UserStorage.h"

const int bufsize = 256;

void send_advanced(int socket_descriptor, std::string& str, int flag = 0);

class Shell
{
public:
    Shell();        //����������� ������
    void work();         //main loop
    ~Shell();
private:
    UserStorage m_users;     //��������� ������ (������ ���� � �������� �� �����) � ������ ������ (� �������� �� ������)
    LinkStorage m_links;     //��������� ���� ����� � ������������ ����� ������� � ������

    //������, ����������� � ��������� �������
    fd_set master;    // ������� ��� ������������
    fd_set read_fds;  // ��������� ��� ������������ ��� select()
    int fdmax;        // ����. ����� ������������
    sockaddr_in server_addr;
    int server_socket;
    int bind_status;
    int connection_status;
    sockaddr remoteaddr;	// ����� �������
    socklen_t addrlen;
    int newfd;		// ���������� ��� ����� ���������� ����� accept()
    int nbytes;
    char buf[bufsize];
};


