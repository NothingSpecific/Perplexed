#ifndef PERPLEXED_GUI_H
#define PERPLEXED_GUI_H

#include "imgui.h"
#include "fonts/font.h"

#include <string>
#include <vector>

namespace Perplexed{
	namespace GUI{
		extern ImVec4 clear_color;
		extern ImGuiStyle *style;
		extern ImGuiIO *io;

//		extern std::string file;
		
		int run();
		int run(std::vector<const char*> *files);
		void open(const char *file);
		void open();
		void save();
		void save_as();
		void close();
		void find();
		void quit();
	}
}

#endif
