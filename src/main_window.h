#ifndef PERPLEXED_MAIN_WINDOW_H
#define PERPLEXED_MAIN_WINDOW_H

#include "window.h"
#include "file_open_dialog.h"

namespace Perplexed{
	namespace GUI{
		class editor_window;
	}
}

#include "editor_window.h"

namespace Perplexed{
	namespace GUI{
		class main_window : window{
		private:
			bool show_file_dialog = false;
			bool find_dialog = false;
			
			editor_window *editor = nullptr;
			std::vector<editor_window*> editors;
			
			file_open_dialog file_dialog;
			
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
							ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
							ImGuiWindowFlags_NoSavedSettings;
			
		public:
			main_window();
			virtual ~main_window();
			virtual bool setup();
			virtual bool render();
			
			// Returns true if the menu bar was rendered, false if no menu bar was rendered
			virtual bool render_menu_bar();
			// Returns true if the tab bar was rendered, false if no menu bar was rendered
			virtual bool render_tab_bar(); // A tab bar to list our open files
			
			virtual void open();
			virtual void open(const char *file);
			virtual void force_save();
			virtual void save();
			virtual void close();
			virtual void find();
			virtual const char *name();
			
			virtual bool is_open(const char *filename);
		};
	}
}

#endif
