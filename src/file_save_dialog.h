#ifndef PERPLEXED_FILE_SAVE_DIALOG_H
#define PERPLEXED_FILE_SAVE_DIALOG_H

#include "window.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace Perplexed{
	namespace GUI{
		class editor_window;
		
		class file_save_dialog : window{
		private:
			IGFD::FileDialog dialog;
			bool is_setup = false;
			
			virtual bool setup();
		public:
			virtual bool render();
			
			// `render()` within `file_open_dialog` works differently than other windows. Instead of returning `false` on error, it returns `false` when the dialog is closed
			virtual bool render(editor_window *save_file);
			
			// Sets up the dialog if it's not already setup.
			virtual bool open();
			virtual bool close();
			virtual const char *name();
		};
	}
}

#endif
