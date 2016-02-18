#include <stdio.h>
#include "TopicMaps.h"

OutputMap::~OutputMap()
{
	printf("OutputMap destructor called\n");
	if (this->_jp) {
		jp_free(this->_jp);
	}
}

