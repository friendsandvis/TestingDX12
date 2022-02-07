#pragma once

template<class T>
class Queue
{
public:
	Queue(unsigned maxlength);
	~Queue();

	T Pop();
	void Push(T data);
	bool isFull() { return (m_front >= m_end); }


private:
	T* m_data;
	unsigned m_maxlength, m_front, m_end;
};

