#ifndef PERPLEXED_MAIN_WINDOW_H
#define PERPLEXED_MAIN_WINDOW_H

#include "window.h"
#include "editor_window.h"
#include "file_open_dialog.h"
#include "file_save_dialog.h"
#include "find_dialog.h"

namespace Perplexed{
	namespace GUI{	
		class main_window : window{
		private:
			bool show_open_dialog = false;
			bool show_save_dialog = false;
			bool show_find_dialog = false;
			
			editor_window *save_file = nullptr; // For keeping track of which editor was active when the save dialog was opened
			
			std::vector<editor_window*> editors;
			
			GUI::file_open_dialog open_dialog;
			GUI::file_save_dialog save_dialog;
			GUI::find_dialog find_dialog;
			
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
							ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
							ImGuiWindowFlags_NoSavedSettings;
			
		public:
			editor_window *editor = nullptr;
			
			main_window();
			virtual ~main_window();
			virtual bool setup();
			virtual bool render();
			
			// Returns true if the menu bar was rendered, false if no menu bar was rendered
			virtual bool render_menu_bar();
			// Returns true if the tab bar was rendered, false if no menu bar was rendered
			virtual bool render_tab_bar(); // A tab bar to list our open files
			
			virtual void open();
			virtual bool open(const char *file);
			virtual bool force_save();
			virtual void save();
			virtual void save_as();
			virtual void close();
			virtual void find();
			virtual const char *name();
			
			virtual bool is_open(const char *filename);
		};
	}
}

#endif
