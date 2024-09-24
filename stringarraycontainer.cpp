#include "stringarraycontainer.h"

StringArrayContainer::StringArrayContainer() :
	m_size {0},
	m_maxlen {0},
	m_value {new std::vector<char> {}}
{}

StringArrayContainer::~StringArrayContainer()
{
	delete m_value;
}

void StringArrayContainer::allocate(int size, int maxlen)
{
	m_size = size;
	m_maxlen = maxlen;

	m_value->assign(size * maxlen, 0);
}

int StringArrayContainer::size() const
{
	return m_size;
}

int StringArrayContainer::maxlen() const
{
	return m_maxlen;
}

std::string StringArrayContainer::value(int index) const
{
	std::vector<char> buffer;
	buffer.assign(m_maxlen + 1, 0);
	memcpy(buffer.data(), m_value->data() + m_maxlen * index, m_maxlen);

	return buffer.data();
}

void StringArrayContainer::setValue(int index, const std::string& v)
{
	for (int i = 0; i < m_maxlen; ++i) {
		*(m_value->data() + m_maxlen * index + i) = 0;
	}

	int copyLen = v.length();
	if (copyLen > m_maxlen - 1) {
		copyLen = m_maxlen - 1;
	}

	memcpy(m_value->data() + m_maxlen * index, v.data(), copyLen);
}

char* StringArrayContainer::pointer()
{
	return m_value->data();
}
