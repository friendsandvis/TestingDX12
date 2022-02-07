#include"Utils.h"

template<class T>
Queue<T>::Queue(unsigned maxlength)
	:m_maxlength(maxlength),
	m_front(0),
	m_end(0)
	
{
	m_data = new T[maxlength];
	m_end = maxlength-1;

}
template<class T>
Queue<T>::~Queue()
{
	delete[] m_data;
	m_data = nullptr;

}

template<class T>
T Pop()
{

}
template<class T>
void Push(T data)
{

}