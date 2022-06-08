#include "ChatBox.h"

ChatBox::ChatBox()
{
    active = false;     //по умолчанию чат неактивен
    m_toggle = false;   //по умолчинаю флаг в нуле
    std::cout << "Chat box " << this << " created\n";
}

ChatBox::ChatBox(const std::string& name)
{
    filename = name;
    active = false;     //по умолчанию чат неактивен
    m_toggle = false;   //по умолчинаю флаг в нуле
    m_file.open(filename, std::ios::in | std::ios::out);
    m_file.close();
    std::cout << "Chat box " << this << " with name: " << filename << " created\n";
}

ChatBox::~ChatBox()
{
    std::cout << "Chat box " << this << " deleted\n";
}

void ChatBox::sendMsg(std::string& msg)
{
    m_messages.push_back(msg);
    //добавить запись в файл
    m_file << msg << "\n";
}

void ChatBox::sendMsgFromFile(std::string& msg)
{
    m_messages.push_back(msg);
}

void ChatBox::showChatAll() const
{
    if (m_messages.size() == 0) { std::cout << "No messages here...\n"; return; }
    for (size_t i = 0; i < m_messages.size(); ++i)
    {
        std::cout << m_messages[i] << "\n";
    }
}

void ChatBox::showChatLast() const
{
    if (m_messages.size() == 0) return;
    std::cout << m_messages[m_messages.size() - 1] << "\n";
}

void ChatBox::setChatStatus(bool val)
{
    active = val;
    //сюда добавляем открытие или закрытие соответствующего файла
    if (val == false)   //Если чат становится неактивным, то закрываем соответствующий файл
    {
        std::cout << "Chat " << filename << " closed!\n";
        m_file.close();
    }
    else if (m_toggle == false)
    {                   //иначе открывает файл на чтение и читаем из него всю историю
                        //НО ТОЛЬКО ЕСЛИ ОН ОТКРЫЛСЯ ПЕРВЫЙ РАЗ ЗА СЕССИЮ
        m_file.open(filename, std::ios::in | std::ios::out);
        if (m_file.is_open())     //если файл открылся - читаем
        {
            std::cout << "Chat " << filename << " opened(toggle == false)!\n";
            m_file.seekg(0, std::ios_base::beg);
            while (!m_file.eof())
            {
                std::string msg;
                //m_file >> msg;                     //пишем сообщение из файла, пробелы делают новую строку, не годится
                //msg = m_file.get();           //вообще читает посимвольно
                std::getline(m_file, msg);      //вот теперь как надо
                sendMsgFromFile(msg);        //добавляем сообщение в локальный массив сообщений
            }
            m_file.close();
        }
        m_toggle = true;        //выставляем флаг
        m_file.open(filename, std::ios::in | std::ios::out | std::ios::app);    //открываем файл на запись
    }
    else
    {
        std::cout << "Chat " << filename << " opened(toggle == true)!\n";
        m_file.open(filename, std::ios::in | std::ios::out | std::ios::app);    //открываем файл на запись
    }
}

bool ChatBox::getChatStatus() const
{
    return active;
}

void ChatBox::addUserToChatList(std::shared_ptr<User> User_ch)
{
    m_users_active.insert(std::make_pair(User_ch->GetName(), User_ch)); //добавляем пользователя в список пользователей этого чата
    setChatStatus(true);            //добавление хотя бы одного делает чат активным
}

void ChatBox::deleteUserFromChatList(std::shared_ptr<User> User_ch)
{
    if (m_users_active.empty()) return; //проверяем, не пуста ли хеш таблица, и если пуста, то ничего с ней делать уже не нужно
    //if (getChatStatus() == false) return;
    std::unordered_map<std::string, std::shared_ptr<User> >::iterator it = m_users_active.find(User_ch->GetName());
    if (it == m_users_active.end()) return;
    else
    {
        m_users_active.erase(User_ch->GetName());               //удаляем пользователя из хеш таблицы по ключу
        if (m_users_active.empty()) setChatStatus(false);       //если активных юзеров не осталось - закрываем чат
    }
}

std::vector<std::string>* ChatBox::getContainer()
{
    return &m_messages;
}

std::unordered_map<std::string, std::shared_ptr<User> >* ChatBox::getUmap()
{
    return &m_users_active;
}

