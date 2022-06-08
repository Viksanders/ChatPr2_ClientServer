#include "LinkStorage.h"

LinkStorage::LinkStorage()
{
    //m_mainPtr = new T;      //������ ����� ����� ��� ���� ������������� ��������� (����� ���)
    m_mainPtr = std::make_shared<ChatBox>("ChatMain.txt");
    m_count = 0;            //��� �������� ���������� ������������� ����� ����

    m_storage.resize(1);    //������ �������� ������ 1 �� 1
    m_storage[0].resize(1);

    m_storage[0][0] = nullptr;  //������ �������� ������ ���� ������� ���������, ��� ����� ���� � �� ���������

    std::cout << "Link storage " << this << " created\n";
}

LinkStorage::~LinkStorage()
{
    /*  //����� ���������� ����� ���������� ���������� ��� ������� ���� �� �����
    delete m_mainPtr;       //������� ����� ������ (����� ���)
    for (size_t i = 0; i < m_storage.size(); ++i)
    {
        delete m_storage[i][i];     //������� ��������� 2� �������, �.�. ������� ���� � ������ �����
        m_storage[i][i] = nullptr;
    }
    size_t ctr = 1;             //������� ��� �������� ������������� �������� ��� �������� �� ����� ������
    for (size_t i = 0; i < m_storage.size() - 1; ++i)
    {
        for (size_t j = ctr; j < m_storage.size(); ++j)
        {
            delete m_storage[i][j];
            m_storage[i][j] = nullptr; //��� ������ �� �����������, ������ ��� �����������
            m_storage[j][i] = nullptr; //����� ��� ��� ���������, ��� m_storage[i][j] � m_storage[j][i] ��� ���� � ��� �� ���
        }
        ++ctr;                     //������� ������� ������������� �������� ��� �������� �� ��������� ������
    }
    //*/
    std::cout << "Link storage " << this << " deleted\n";
}

void LinkStorage::AddUser()
{
    ++m_count;  //����������� ���������� ������������� �� ������� ��� ���������� ������ ������������ (� ���)

    if (m_count == 1) //���� ��� ������ ����������� ������������, �� ��������� ������ ��� � "����� �����", �.�. �������
    {
        //m_storage[0][0] = new T; 
        m_storage[0][0] = std::make_shared<ChatBox>("Chat00.txt");
        return;
    }
    else
    {
        //��������� +1 ������ � +1 �������
        m_storage.resize(m_count);
        for (size_t i = 0; i < m_storage.size(); ++i) { m_storage[i].resize(m_count); }

        //m_storage[m_storage.size() - 1][m_storage.size() - 1] = new T;    //������ ��� � ����� ����� ��� ��������������� ������������
        std::string chat_name = "Chat" + std::to_string(m_storage.size() - 1) + std::to_string(m_storage.size() - 1) + ".txt";
        m_storage[m_storage.size() - 1][m_storage.size() - 1] = std::make_shared<ChatBox>(chat_name);    //������ ��� � ����� ����� ��� ��������������� ������������
        for (size_t i = 0; i < m_storage.size() - 1; ++i)     //��������� ������ ��������� ������������ � ���������� ��������������
        {
            //m_storage[i][m_storage.size() - 1] = new T;       //������ ������ ��� ����� ������������� A � B;
            chat_name = "Chat" + std::to_string(i) + std::to_string(m_storage.size() - 1) + ".txt";
            m_storage[i][m_storage.size() - 1] = std::make_shared<ChatBox>(chat_name);       //������ ������ ��� ����� ������������� A � B;
            m_storage[m_storage.size() - 1][i] = m_storage[i][m_storage.size() - 1];    //��� ����� ������������� B � A ��� ��� �� ����� ���, ��� ����� A � B
        }
    }
}

void LinkStorage::PrintAddresses() const
{
    if (m_count == 0) { std::cout << "��������� ����\n"; return; }

    for (size_t i = 0; i < m_storage.size(); ++i)
    {
        for (size_t j = 0; j < m_storage.size(); ++j)
        {
            std::cout << m_storage[i][j] << "\t";
        }
        std::cout << "\n";
    }
}

/*
void LinkStorage::PrintData()
{
    if (m_count == 0) { std::cout << "��������� ����\n"; return; }

    for (size_t i = 0; i < m_storage.size(); ++i)
    {
        for (size_t j = 0; j < m_storage.size(); ++j)
        {
            std::cout << *m_storage[i][j] << "\t";
        }
        std::cout << "\n";
    }
}
*/

std::shared_ptr<ChatBox> LinkStorage::Get(int ID1, int ID2) const
{
    return m_storage[ID1][ID2];
}

std::shared_ptr<ChatBox> LinkStorage::GetMain() const
{
    return m_mainPtr;
}
