#include "User.h"

User::User(const std::string& login, const std::string& password) : m_login(login)
{
    ID = ID_count;
    ++ID_count;
    std::string password_tmp = password;        //временная строка для пароля, который будет солиться
    uint salt = static_cast<uint>(ID);
    password_tmp += std::to_string(salt);       //добавляем её к паролю
    m_hpassword = sha1_str(password_tmp);       //считаем солёный хеш
    sha1_add_salt(m_hpassword, salt);            //добавляем эту соль к хешу
    m_status = false;                           //при создании новый пользователь всегда оффлайн по умолчанию
    m_CHAT = nullptr;                           //активного чата нет, т.к. пользователь оффлайн
    m_socket = -1;                              //сокета тоже нет, т.к. пользователь оффлайн
    std::cout << "USER constructor " << this << " called\n";
}

User::User(const std::string& login, const std::vector<uint>& hash) : m_login(login), m_hpassword(hash)    //конструктор для добавления готового юзера
{
    ID = ID_count;              //данный конструктор должен вызываться только ОДИН раз и только при старте программы!
    ++ID_count;
    m_status = false;                           //при добавлении из файла пользователь всегда оффлайн по умолчанию
    m_CHAT = nullptr;                           //активного чата нет, т.к. пользователь оффлайн
    m_socket = -1;                              //сокета тоже нет, т.к. пользователь оффлайн
    std::cout << "User " << this << " added from disk storage\n";
}

User::~User()
{
    //delete[] m_hpassword;                 //с использованием вектора делит стал не нужен
    std::cout << "USER destructor " << this << " called\n";
}  //зачем виртуальный? Чтобы потом класс Moderator унаследовать (но это не точно)

std::string User::GetName() const { return m_login; }    

std::vector<uint> User::GetPassword() const { return m_hpassword; }

bool User::isPasswordCorrect(const std::string& password) const
{
    std::string password_tmp = password;
    uint salt = sha1_pick_salt(m_hpassword);
    password_tmp += std::to_string(salt);
    std::vector<uint> hash_to_check = sha1_str(password_tmp);         //std::vector<uint> hash_to_check = sha1_str(password); добавили соль
    bool compare = two_hashes_compare(m_hpassword, hash_to_check, hash_size);
    //delete[] hash_to_check;
    return compare;
};

void User::SetPassword(const std::string& password)                    //смена пароля
{
    //delete[] m_hpassword;             //с использованием умного указателя делит стал не нужен
    m_hpassword = sha1_str(password);
};

int User::GetID() const { return ID; };

bool User::GetStatus() const { return m_status; };

int User::GetSocket() const { return m_socket; };

std::shared_ptr<ChatBox> User::GetChat() const { return m_CHAT; };

void User::SetStatus(bool status) { m_status = status; };

void User::SetSocket(const int socket) { m_socket = socket; };

void User::SetChat(const std::shared_ptr<ChatBox> chat)           //установить пользователю чат
{
    m_CHAT = chat;
};
