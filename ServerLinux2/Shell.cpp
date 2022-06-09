#include "Shell.h"

int User::ID_count = 0;

void send_advanced(int socket_descriptor, std::string& str, int flag)
{
    size_t message_size = str.length();
    send(socket_descriptor, (char*)&message_size, sizeof(int), flag);      //сначала посылаем размер строки
    send(socket_descriptor, str.c_str(), message_size, flag);              //затем саму строку
}

Shell::Shell()
{
    m_users.init();     //загружаем базу с пользователями из файла
    for (int i = 0; i < m_users.GetUsersNumber(); ++i)
    {
        m_links.AddUser();
    }

    //НАСТРОЙКА СЕРВЕРА
    FD_ZERO(&master);    // очищаем оба сета
    FD_ZERO(&read_fds);

    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(55555);
    server_addr.sin_family = AF_INET;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cout << "Error creating socket\n";
        exit(1);
    }

    bind_status = bind(server_socket, (sockaddr*)(&server_addr), sizeof(server_addr));
    if (bind_status == -1)
    {
        std::cout << "Error binding socket\n";
        exit(2);
    }

    connection_status = listen(server_socket, SOMAXCONN);
    if (connection_status == -1)
    {
        std::cout << "Error listening\n";
        exit(3);
    }
    else
    {
        std::cout << "Awaiting for connection\n";
    }
    // добавляем слушающий сокет в мастер-сет
    FD_SET(server_socket, &master);
    // следим за самым большим номером дескриптора
    fdmax = server_socket; // на данный момент это этот

    std::cout << "Shell " << this << " created\n";
}

Shell::~Shell()
{
#ifdef WIN32
    closesocket(server_socket);
    WSACleanup();
#else
    close(server_socket);
#endif
    std::cout << "Shell " << this << " deleted\n";
}

void Shell::work()
{
    while (1)
    {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            std::cout << "Error calling select() function\n";
            exit(4);
        }
        // проходим через существующие соединения, ищем данные для чтения
        for (int i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_socket)
                {
                    // обрабатываем новые соединения
                    addrlen = sizeof(remoteaddr);
                    newfd = accept(server_socket, (struct sockaddr*)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        std::cout << "Connection accepting error\n";
                    }
                    else
                    {
                        std::cout << "Connection established. Socket descriptor: " << newfd << "\n";
                        FD_SET(newfd, &master); // добавляем в мастер-сет
                        if (newfd > fdmax) // продолжаем отслеживать самый большой номер дескиптора
                        {
                            fdmax = newfd;
                        }
                    }
                }
                else
                {
                    // обрабатываем данные клиента
                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
                    {
                        // получена ошибка или соединение закрыто клиентом
                        if (nbytes == 0)
                        {
                            // соединение закрыто
                            std::cout << "Connection closed. Socket descriptor: " << i << "\n";
                        }
                        else
                        {
                            std::cout << "Receiving error from " << i << "\n";
                        }

                        //необходимо загнать юзера оффлайн и открепить от него сокет
                        std::shared_ptr<User> User_to_offline = m_users.getUserBySocket(i);
                        if (User_to_offline != nullptr)     //если такой юзер есть онлайн - то отправляем его оффлайн
                        {
                            m_users.setChatByName(User_to_offline->GetName(), nullptr);
                            m_users.setStatusByName(User_to_offline->GetName(), false);
                            m_users.setSocketByName(User_to_offline->GetName(), -1);
                        }


                        //здесь закрываем сокет
                        #ifdef WIN32
                            closesocket(i);
                            WSACleanup();
                        #else
                            close(i);
                        #endif
                        FD_CLR(i, &master); // удаляем из мастер-сета
                    }
                    else
                    {
                        std::cout << "Msg from " << i << ":   " << buf << "\n";
                        // у нас есть какие-то данные от клиента
                        //переводим данные в строку
                        std::string msg;        //сообщение ОТ клиента
                        std::string msg_back;   //сообщение ДЛЯ клиента
                        for (int k = 0; k < bufsize; ++k)
                        {
                            if (buf[k] == '\0') break;
                            msg.push_back(buf[k]);
                        }
                        //проверим, привязан ли какой-то юзер к этому сокету?
                        std::shared_ptr<User> currentUser = m_users.getUserBySocket(i);
                        if (currentUser == nullptr)     //сокет не привязан ни к какому пользователю, т.е. клиент не залогинен
                        {
                            //здесь делаем либо логин, либо регистрацию
                            if ((msg[0] == '/') && (msg[1] == 'r') && (msg[2] == ' '))
                            {
                                size_t found = msg.find(' ', 3);
                                std::string name = msg.substr(3, found - 3);
                                std::string password = msg.substr(found);
                                if (m_users.IsLoginFree(name))
                                {
                                    m_users.AddUser(name, password);
                                    m_links.AddUser();
                                    msg_back = "User with name: " + name + " and password: " + password + " created!";
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию
                                }
                                else
                                {
                                    msg_back = "Login is already in use...";
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию
                                }
                            }
                            else if ((msg[0] == '/') && (msg[1] == 'l') && (msg[2] == ' '))
                            {
                                size_t found = msg.find(' ', 3);
                                std::string name = msg.substr(3, found - 3);
                                std::string password = msg.substr(found);
                                if (m_users.IsLoginFree(name))          //проверяем, есть ли вообще пользователь с таким именем
                                {
                                    msg_back = "No user with the name " + name;
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию

                                }
                                else
                                {
                                    if (m_users.isPasswordCorrect(name, password))   //проверяем пароль
                                    {
                                        if (m_users.getUser(name)->GetStatus()) //здесь делаем проверку на то, если юзер уже онлайн
                                        {
                                            msg_back = "User " + name + " is already online!";
                                            send_advanced(i, msg_back);         //заменил на функцию
                                        }
                                        else
                                        {
                                            msg_back = "Successfull login as " + name;
                                            //send(i, msg_back.c_str(), msg_back.length(), 0);
                                            send_advanced(i, msg_back);         //заменил на функцию

                                            //здесь надо привязать сокет юзеру, назначить ему чат и поменять статус
                                            currentUser = m_users.getUser(name);
                                            m_users.setChatByName(name, m_links.GetMain());
                                            m_users.setStatusByName(currentUser->GetName(), true);
                                            m_users.setSocketByName(currentUser->GetName(), i);
                                        }
                                    }
                                    else
                                    {
                                        msg_back = "Wrong password!";
                                        //send(i, msg_back.c_str(), msg_back.length(), 0);
                                        send_advanced(i, msg_back);         //заменил на функцию
                                    }
                                }
                            }
                            else
                            {
                                msg_back = "Unknown command. Only /r and /l are awaliable";
                                //send(i, msg_back.c_str(), msg_back.length(), 0);
                                send_advanced(i, msg_back);         //заменил на функцию
                            }
                        }
                        else        //юзеру привязан сокет, значит он залогинен
                        {
                            //здесь либо переключаем чаты, либо запрашиваем чатлог, либо делаем логаут, либо пишем сообщение в текущий активный чат
                            if ((msg[0] == '/'))            //обрабатываем команду
                            {
                                if ((msg[1] == 's') && (msg[2] == ' '))
                                {
                                    std::string name = msg.substr(3);
                                    msg_back = "Now you are writing to " + name;
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию

                                    //передаем пользователю currentUser новый чат с пользователем name
                                    //получаем указатель пользователя по имени, берем айдишник пользователя, находим нужный чат по ID текущего пользователя
                                    //и ID пользователя, имя которого было указано в name, находим нужный чат и ставим его текущему пользователю
                                    std::shared_ptr<ChatBox> ChatToWrite = m_links.Get(currentUser->GetID(), m_users.GetIDbyName(name));
                                    m_users.setChatByName(currentUser->GetName(), ChatToWrite);
                                }
                                else if ((msg[1] == 'a') && (msg[2] == 'l') && (msg[3] == 'l'))
                                {
                                    msg_back = "Now you are writing to main chat";
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию

                                    //делаем активный основной чат для данного пользователя
                                    std::shared_ptr<ChatBox> ChatToWrite = m_links.GetMain();
                                    //currentUser->SetChat(ChatToWrite);  //вот здесь был баг при переключении чатов в видео
                                    m_users.setChatByName(currentUser->GetName(), ChatToWrite);
                                }
                                else if ((msg[1] == 'g') && (msg[2] == 'e') && (msg[3] == 't'))
                                {
                                    msg_back = "CONTENTS OF CURRENT CHAT:\n";
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию

                                    //отправить весь текущий чат клиенту сообщение за сообщением
                                    std::vector<std::string>* localMessages = currentUser->GetChat()->getContainer();   //получили контейнер текущего чата
                                    for (size_t k = 0; k < localMessages->size(); ++k)  //теперь пробегаем по нему и отправляем каждое сообщение по одному юзеру
                                    {
                                        //send(i, localMessages->at(k).c_str(), localMessages->at(k).length(), 0);
                                        
                                        //int message_size = localMessages->at(k).length();
                                        //send(i, (char*)&message_size, sizeof(int), 0);      //сначала посылаем размер строки
                                        //send(i, localMessages->at(k).c_str(), localMessages->at(k).length(), 0);    //затем саму строку

                                        send_advanced(i, localMessages->at(k));         //заменил на функцию
                                        std::cout << localMessages->at(k) << "\n";
                                    }
                                    //msg_back = "/end";
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                }
                                else if ((msg[1] == 'l') && (msg[2] == 'o') && (msg[3] == 'u') && (msg[4] == 't'))
                                {
                                    msg_back = "Good bye!";
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию 
                                    
                                    //разлогиниться
                                    m_users.setChatByName(currentUser->GetName(), nullptr);
                                    m_users.setStatusByName(currentUser->GetName(), false);
                                    m_users.setSocketByName(currentUser->GetName(), -1);
                                }
                                else
                                {
                                    msg_back = "No such command! Only /lout, /s UserName, /get and /all are avaliable.\n";
                                    //send(i, msg_back.c_str(), msg_back.length(), 0);
                                    send_advanced(i, msg_back);         //заменил на функцию
                                }
                            }
                            else            //обрабатываем сообщение
                            {
                                std::shared_ptr<ChatBox> currentChat = currentUser->GetChat();  //добавляем сообщение в хранилище сообщений данного чата
                                msg = "[" + currentUser->GetName() + "]:" + " " + msg;
                                currentChat->sendMsg(msg);
                                msg_back = "Successfull sending!";
                                //send(i, msg_back.c_str(), msg_back.length(), 0);  //отправляем обратно сообщение тому, кто отправил, что всё норм отправилось
                                send_advanced(i, msg_back);         //заменил на функцию
                                //ДАЛЕЕ НУЖНО РАЗОСЛАТЬ СООБЩЕНИЕ ВСЕМ ПОЛЬЗОВАТЕЛЯМ ОНЛАЙН, И У КОГО ПРИ ЭТОМ АКТИВЕН ДАННЫЙ ЧАТ, Т.Е. currentChat
                                std::unordered_map<std::string, std::shared_ptr<User> >* umap = currentChat->getUmap();
                                for (auto it = umap->begin(); it != umap->end(); ++it)  //перебираем всех пользователей, которые находятся в данный момент в этом чате
                                {
                                    //и если пользователь НЕ ЯВЛЯЕТСЯ currentUser, то отправляем ему сообщение msg
                                    if (it->second != currentUser)
                                    {
                                        int newSocket = it->second->GetSocket();
                                        send_advanced(newSocket, msg);
                                    }
                                }
                            }
                        }
                    }
                } // Закончили обрабатывать данные от клиента
            } // Закончили обрабатывать новое входящее соединение
        } // Закончили цикл по дескрипторам
    }
}


