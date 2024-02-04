#ifndef PERPLEXED_FIND_DIALOG_H
#define PERPLEXED_FIND_DIALOG_H

#include "imgui.h"
#include "window.h"

#include <string>

namespace Perplexed{
	namespace GUI{
		class main_window;
		class editor_window;
		
		class find_dialog : window{
		private:
			main_window *mw;
			editor_window *editor;
			std::string search_string;
			
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
							
		public:
			virtual void setup(main_window *mw, editor_window *editor);
			
			virtual bool render();
			virtual const char *name();
		};
	}
}

#include "main_window.h"
#include "editor_window.h"

#endif
