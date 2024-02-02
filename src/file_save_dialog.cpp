#include "file_save_dialog.h"

#include "definitions.h"
#include "gui.h"
#include "editor_window.h"
#include "imgui.h"

// Adding these `#include`s here for easier staging of upstream merges
#include "icon_fonts/IconsForkAwesome.h"
#include "icon_fonts/IconsMaterialDesign.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"

#include <stdexcept>

namespace Perplexed{
	namespace GUI{
		file_save_dialog::~file_save_dialog(){
			if(dialog != nullptr){
				delete dialog;
				dialog = nullptr;
			}
		}
		bool file_save_dialog::setup(){
			if(is_setup) return true;
			
			dialog = new ImGuiFileDialog();

			IGFD::FileDialogConfig config;

			config.flags =	ImGuiFileDialogFlags_DisableCreateDirectoryButton |
					ImGuiFileDialogFlags_DontShowHiddenFiles |
					ImGuiFileDialogFlags_ConfirmOverwrite;

			dialog->OpenDialog(name(), name(), "((.*))", config);

			dialog->SetFileStyle(IGFD_FileStyleByFullName, ".", ImVec4(0.3f, 0.3f, 0.3f, 1), ICON_FK_CIRCLE);
			dialog->SetFileStyle(IGFD_FileStyleByFullName, "..", ImVec4(0.8f, 0.8f, 0.8f, 1), ICON_FK_LEVEL_UP);
			
			is_setup = true;
			return true;
		}
		bool file_save_dialog::render(){
			throw std::runtime_error("`render()` invalid for Perplexed::GUI::file_save_dialog");
			return false;
		}
		bool file_save_dialog::render(editor_window *save_file){
			ImVec2 max_size = ImVec2(
					(io->DisplaySize.x - (style->FramePadding.x * 2.0f) - (style->FrameBorderSize * 2.0f)),
					(io->DisplaySize.y - (style->FramePadding.y * 2.0f) - (style->FrameBorderSize * 2.0f))
				);
			ImVec2 min_size = max_size / 2.0f;
			
			if (dialog->Display(name(), ImGuiWindowFlags_NoCollapse, min_size, max_size)){
				if (dialog->IsOk()){
					save_file->set_file(dialog->GetFilePathName().c_str());
					save_file->save();
				}
	                        close();
	                        return false;
                        }
                        return true;
		}
		bool file_save_dialog::open(){
			return setup();
		}
		bool file_save_dialog::close(){
			if(is_setup){
				dialog->Close();
				delete dialog;
				dialog = nullptr;
				is_setup = false;
			}
			return true;
		}
		const char *file_save_dialog::name(){
			return "Save As";
		}
	}
}
