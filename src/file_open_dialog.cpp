#include "file_open_dialog.h"

#include "definitions.h"
#include "gui.h"
#include "imgui.h"

// Adding these `#include`s here for easier staging of upstream merges
#include "icon_fonts/IconsForkAwesome.h"
#include "icon_fonts/IconsMaterialDesign.h"

namespace Perplexed{
	namespace GUI{
		bool file_open_dialog::setup(){
			if(is_setup) return true;

			IGFD::FileDialogConfig config;

			config.flags =	ImGuiFileDialogFlags_DisableCreateDirectoryButton |
					ImGuiFileDialogFlags_DontShowHiddenFiles;

			dialog.OpenDialog(name(), name(), "((.*))", config);

			dialog.SetFileStyle(IGFD_FileStyleByFullName, ".", ImVec4(0.3f, 0.3f, 0.3f, 1), ICON_FK_CIRCLE);
			dialog.SetFileStyle(IGFD_FileStyleByFullName, "..", ImVec4(0.8f, 0.8f, 0.8f, 1), ICON_FK_LEVEL_UP);
			
			is_setup = true;
			return true;
		}
		bool file_open_dialog::render(){
			ImVec2 max_size = ImVec2(
					(io->DisplaySize.x - (style->FramePadding.x * 2.0f) - (style->FrameBorderSize * 2.0f)),
					(io->DisplaySize.y - (style->FramePadding.y * 2.0f) - (style->FrameBorderSize * 2.0f))
				);
			ImVec2 min_size = max_size / 2.0f;
			
			if (dialog.Display(name(), ImGuiWindowFlags_NoCollapse, min_size, max_size)){
				if (dialog.IsOk()){
					Perplexed::GUI::open(dialog.GetFilePathName().c_str());
				}
	                        close();
	                        return false;
                        }
                        return true;
		}
		bool file_open_dialog::open(){
			return setup();
		}
		bool file_open_dialog::close(){
			if(is_setup){
				dialog.Close();
				is_setup = false;
			}
			return true;
		}
		const char *file_open_dialog::name(){
			return "Open File";
		}
	}
}
