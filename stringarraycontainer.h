#ifndef STRINGARRAYCONTAINER_H
#define STRINGARRAYCONTAINER_H

#include "iriclib_global.h"

#include <string>
#include <vector>

class IRICLIBDLL StringArrayContainer
{
public:
	StringArrayContainer();
	~StringArrayContainer();

	void allocate(int size, int maxlen);
	int size() const;
	int maxlen() const;

	std::string value(int index) const;
	void setValue(int index, const std::string& v);
	char* pointer();

private:
	int m_size;
	int m_maxlen;
	std::vector<char>* m_value;
};

#endif // STRINGARRAYCONTAINER_H
