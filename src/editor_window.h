#ifndef PERPLEXED_EDITOR_WINDOW_H
#define PERPLEXED_EDITOR_WINDOW_H

#include "window.h"

#include "definitions.h"
#include "ImGuiColorTextEdit/TextEditor.h"

#include <mutex>

namespace Perplexed{
	namespace GUI{
		class main_window;
	}
	class finder_thread;
}
#include "main_window.h"
		
namespace Perplexed{
	namespace GUI{
		class editor_window : window{
		private:
			main_window *parent;
			
			const char *file = nullptr;
			const char *file_basename = nullptr;
			char *window_name = nullptr;
			ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar |
							ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
							ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus |
							ImGuiWindowFlags_NoSavedSettings;
							
			finder_thread *finder = nullptr;
			
		public:
			TextEditor *editor;
			TextEditor::LanguageDefinition *lang = nullptr;
			
			std::mutex mtx;
			
			editor_window(main_window *parent);
			virtual ~editor_window();
			virtual bool setup();
			virtual bool render();
			
			virtual bool open(const char *file);
			virtual bool force_save();
			virtual bool save();
			virtual void close();
			virtual const char *name();
			virtual const char *filename();
			virtual const char *basename();
			
			// Set the file without modifying the editor
			virtual void set_file(const char *file);
			
			virtual void find(std::regex rex);
		};
	}
}

#include "finder_thread.h"

#endif
