#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>
#include <unordered_map>

#include "User.h"

class User;

class ChatBox
{
public:
    ChatBox();
    ChatBox(const std::string& name);
    ~ChatBox();
    void sendMsg(std::string& msg);  //кладёт строку msg в массив строк m_messages
    void sendMsgFromFile(std::string& msg);  //кладёт строку msg в массив строк m_messages
    void showChatAll() const;             //печатает в консоль весь массив m_messages
    void showChatLast() const;            //печатает в консоль только последнее сообщение (метод не используется...)
    void setChatStatus(bool val);
    bool getChatStatus() const;
    void addUserToChatList(std::shared_ptr<User> User_ch);
    void deleteUserFromChatList(std::shared_ptr<User> User_ch);
    std::vector<std::string>* getContainer();       //вернуть указатель на внутренний контейнер сообщений
    std::unordered_map<std::string, std::shared_ptr<User> >* getUmap();
private:
    std::vector<std::string> m_messages;    //массив сообщений
    bool active;      //статус чата: активен или нет в данный момент в клиентской программе, за раз может быть активен только один чат
    std::string filename;       //имя файла, куда будет писаться лог данного чата
    std::fstream m_file;        //файл, в который пишется лог данного чата
    bool m_toggle;              //флаг, который сигнализирует, открывается ли чат первый раз за сессию
    std::unordered_map<std::string, std::shared_ptr<User> > m_users_active;     //ключ - имя пользователя, значение - указатель на пользователя
};

