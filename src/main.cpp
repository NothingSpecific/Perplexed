#include "definitions.h"
#include "gui.h"

#include <vector>

int main(int argc, char **argv){
	std::vector<const char*> files;
	for(int a=1; a<argc; ++a){
		files.push_back(argv[a]);
	}
	return Perplexed::GUI::run(&files);
}
