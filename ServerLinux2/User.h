#pragma once
#include <memory>
#include "SHA1.h"
#include "ChatBox.h"

class ChatBox;

class User
{
public:
    User(const std::string& login, const std::string& password);    //Констуктор создания нового юзера по логину и паролю
    User(const std::string& login, const std::vector<uint>& hash); //конструктор для добавления готового юзера
    ~User();
    std::string GetName() const;    //выдать имя
    std::vector<uint> GetPassword() const;                   //выдать пароль, а теперь уже хеш (больше не используется)
    bool isPasswordCorrect(const std::string& password) const;
    void SetPassword(const std::string& password);                    //смена пароля
    int GetID() const;                             //Получить ID пользователя

    bool GetStatus() const;                  //Узнать статус пользоваетля: онлайн или оффлайн
    int GetSocket() const;             //выдать дескриптор сокета, куда привязан пользователь, -1 - пользователь оффлайн
    std::shared_ptr<ChatBox> GetChat() const;       //выдать чат, в который пишет пользователь, нульптр - пользователь оффлайн
    void SetStatus(bool status);     //Установить статус пользователя: онлайн или оффлайн
    void SetSocket(const int socket); //Приязать сокет пользователю
    void SetChat(const std::shared_ptr<ChatBox> chat);           //установить пользователю чат
private:
    std::string m_login;        //логин - оно же имя
    //uint* m_hpassword;     //ТЕПЕРЬ ЗДЕСЬ ХЕШ ОТ ПАРОЛЯ
    std::vector<uint> m_hpassword;    //хеш от пароля теперь через vector
    static int ID_count;        //счетчик для установки ID
    int ID;                     //уникальный ID каждого пользователя
    bool m_status;              //Статус - онлайн (true) пользователь или оффлайн (false)
    std::shared_ptr<ChatBox> m_CHAT;   //указатель на текущий активный ДЛЯ ДАННОГО ПОЛЬЗОВАТЕЛЯ чат
    int m_socket;               //сюда передаётся дескриптор сокета (на стороне сервера), через который пользователь обращается к серверу
};
