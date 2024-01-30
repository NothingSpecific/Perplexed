#ifndef PERPLEXED_GUI_H
#define PERPLEXED_GUI_H

#include "imgui.h"
#include "fonts/font.h"
#include <string>

namespace Perplexed{
	namespace GUI{
		extern ImVec4 clear_color;
		extern ImGuiStyle *style;
		extern ImGuiIO *io;

//		extern std::string file;
		
		int run();
		void open(std::string file);
		void open();
		void save();
		void close();
	}
}

#endif
