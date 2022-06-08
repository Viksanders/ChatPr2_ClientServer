#pragma once
#include "ChatBox.h"
#include <memory>

class LinkStorage
{
public:
    LinkStorage();    //������������ �����������, ����������� � ������ �� ���������, �.�. "��������� ������������" �������������� ������ ���� �� ��� ���������
    ~LinkStorage();   //� ���������� ������� ����������� ��������� ������
    void AddUser();             //�������� ����� �������
    std::shared_ptr<ChatBox> Get(int ID1, int ID2) const;
    std::shared_ptr<ChatBox> GetMain() const;
    void PrintAddresses() const;      //������� �� ����� ������ ����������, ���������� � ������ ������ ������� ������ ��� �������
    //void PrintData();           //������� ������ �� ��������� �� ������ ������ ������ ��� �������
private:
    //std::vector<std::vector<T*> > m_storage;   //��������� ������, ���������� �� ������ ������ ��� �������� ���������� �� ���������������� ����
    //T* m_mainPtr;                              //��������� �� �������� ������ (� ������ ���� - �� ����� ���)
    std::vector<std::vector<std::shared_ptr<ChatBox> > > m_storage;
    std::shared_ptr<ChatBox> m_mainPtr;
    int m_count;                                //����� �������������
};


/*  //����� ��� ����� LinkStorage
struct staticInt
{
    static int i;
    int x = 0;
    staticInt() { x = i;  ++i; }
    friend std::ostream& operator<<(std::ostream& output, const staticInt& v);
};
int staticInt::i = 0;
std::ostream& operator<<(std::ostream& output, const staticInt& v)
{
    output << v.x;
    return output;
}
//*/
