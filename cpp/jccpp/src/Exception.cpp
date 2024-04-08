#include "jccpp/Exception.h"

std::ostream &operator<<(std::ostream &os, std::exception const &e)
{
	return os << e.what();
}
