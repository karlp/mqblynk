#include <iostream>
#include "TopicMaps.h"

OutputMap::~OutputMap()
{
	std::cout << "OutputMap destructor called" << std::endl;
	if (this->_jp) {
		jp_free(this->_jp);
	}
	if (this->topic) {
		free((void*)this->topic);
	}
}

void OutputMap::dump()
{
	std::cout << "Pin: " << this->pin << " JSONPATH: (fixme)"
		<< "Topic: " << this->topic << std::endl;
}

InputMap::~InputMap()
{
	std::cout << "InputMap destructor called" << std::endl;
	if (this->topic) {
		free((void*)this->topic);
	}
}

void InputMap::dump()
{
	std::cout << "Pin: " << this->pin << " Topic: " << this->topic << std::endl;
}
