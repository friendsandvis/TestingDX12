#pragma once
#include<vector>

using namespace std;
template<class T>
class Set
{
public:
	Set() {}
	Set(T* invalues, size_t valuecount);
	~Set() {}
	size_t GetSize() { return m_data.size(); }
	T* GetDataptr() { return m_data.data(); }
	inline void Push(T val) { m_data.push_back(val); }
	void PushUnique(T val);
	void Remove(T val);

	Set GetIntersection(Set otherset);
	Set Minus(Set otherset);
	size_t Find(T valuetofind);
	size_t Find(T valuetofind, bool& out_elementfound);


private:
	//held sequentially
	vector<T> m_data;
};



template<class T>
size_t Set<T>::Find(T valuetofind)
{
	size_t idx = 0;
	for (idx = 0; idx < m_data.size(); idx++)
	{
		if (m_data[idx] == valuetofind)
			return idx;
	}
	return -1;

}

template<class T>
size_t Set<T>::Find(T valuetofind, bool& out_elementfound)
{
	size_t idx = 0;
	for (idx = 0; idx < m_data.size(); idx++)
	{
		if (m_data[idx] == valuetofind)
		{
			out_elementfound = true;
			return idx;
		}
	}
	out_elementfound = false;
	return -1;

}
template<typename T>
Set<T> Set<T>::GetIntersection(Set<T> otherset)
{
	Set<T> intersectionset;

	for (size_t i = 0; i < m_data.size(); i++)
	{
		bool valfound;
		otherset.Find(m_data[i], valfound);
		if (valfound)
		{
			intersectionset.Push(m_data[i]);
		}
	}
	return intersectionset;

}

template<typename T>
Set<T>  Set<T>::Minus(Set<T> otherset)
{
	Set<T> result;
	for (size_t i = 0; i < m_data.size(); i++)
	{
		bool foundvalue;
		otherset.Find(m_data[i], foundvalue);
		if (!foundvalue)
		{
			result.Push(m_data[i]);
		}
	}
	return result;
}

template<typename T>
Set<T>::Set(T* invalues, size_t valuecount)
{
	for (size_t i = 0; i < valuecount; i++)
	{
		m_data.push_back(invalues[i]);
	}
}

template<typename T>
void Set<T>::PushUnique(T val)
{
	bool elementfound;
	Find(val, elementfound);

	if (!elementfound)
	{
		m_data.push_back(val);
	}
}

template<typename T>
void Set<T>::Remove(T val)
{
	vector<T> newdata;

	for (size_t i = 0; i < m_data.size(); i++)
	{
		if (m_data[i] != val)
		{
			newdata.push_back(m_data[i]);
		}
	}
	m_data = newdata;

}


