#ifndef FILESYSTEM_H_INCLUDED
#define FILESYSTEM_H_INCLUDED

#include <string>

namespace filesystem{
	std::string dirname(std::string file);
	std::string dirpath(std::string file);
	std::string basename(std::string file);
}

#endif // FILESYSTEM_H_INCLUDED

