#include "filesystem.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

namespace filesystem{
	std::string dirname(std::string file){
	    size_t split = file.find_last_of("/\\");
	    if(split == std::string::npos)
		return ".";
	    return file.substr(0, split);
	}
	std::string dirpath(std::string file){
	    size_t split = file.find_last_of("/\\");
	    if(split == std::string::npos)
		return "./";
	    return file.substr(0, split + 1);
	}
	std::string basename(std::string file){
	    size_t split = file.find_last_of("/\\");
	    if(split == std::string::npos)
		return file;
	    return file.substr(split + 1);
	}

	void deleteDirectoryContents(const std::filesystem::path& dir)
	{
	    for (const auto& entry : std::filesystem::directory_iterator(dir))
		std::filesystem::remove_all(entry.path());
	}
}
