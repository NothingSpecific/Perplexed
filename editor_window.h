#ifndef PERPLEXED_EDITOR_WINDOW_H
#define PERPLEXED_EDITOR_WINDOW_H

#include "window.h"

#include "definitions.h"
#include "ImGuiColorTextEdit/TextEditor.h"

namespace Perplexed{
	namespace GUI{
		class main_window;
	}
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
							ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
			
		public:
			TextEditor *editor;
			TextEditor::LanguageDefinition *lang = nullptr;
			
			editor_window(main_window *parent);
			virtual ~editor_window();
			virtual bool setup();
			virtual bool render();
			
			virtual void open(const char *file);
			virtual void force_save();
			virtual void save();
			virtual void close();
			virtual const char *name();
			virtual const char *filename();
			virtual const char *basename();
		};
	}
}


#endif
