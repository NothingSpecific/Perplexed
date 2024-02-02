#ifndef PERPLEXED_FILE_OPEN_DIALOG_H
#define PERPLEXED_FILE_OPEN_DIALOG_H

#include "window.h"

namespace IGFD{
	class FileDialog;
}


namespace Perplexed{
	namespace GUI{	
		class file_open_dialog : window{
		private:
			IGFD::FileDialog *dialog;
			bool is_setup = false;
			
			virtual bool setup();
		public:
			virtual ~file_open_dialog();
			
			// `render()` within `file_open_dialog` works differently than other windows. Instead of returning `false` on error, it returns `false` when the dialog is closed
			virtual bool render();
			
			// Sets up the dialog if it's not already setup.
			virtual bool open();
			virtual bool close();
			virtual const char *name();
		};
	}
}

#endif
