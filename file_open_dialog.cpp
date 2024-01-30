#include "file_open_dialog.h"

#include "gui.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

namespace Perplexed{
	namespace GUI{
		ImGuiFileDialog dialog;
		
		bool show_file_open_dialog(){
			ImGui::PushFont(font::icon_font_karla_regular_forkawesome);

			IGFD::FileDialogConfig config;

			ImVec2 max_size = ImVec2( // Half the display area
					(io->DisplaySize.x - (style->FramePadding.x * 2.0f) - (style->FrameBorderSize * 2.0f)),
					(io->DisplaySize.y - (style->FramePadding.y * 2.0f) - (style->FrameBorderSize * 2.0f))
				);
			ImVec2 min_size = max_size / 2.0f;

			config.flags =	ImGuiFileDialogFlags_DisableCreateDirectoryButton |
					ImGuiFileDialogFlags_DontShowHiddenFiles;

			dialog.OpenDialog("OpenFileDialog", "Open File", ".*", config);

			ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, ".", ImVec4(0.3f, 0.3f, 0.3f, 1), ICON_FK_CIRCLE);
			ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "..", ImVec4(0.8f, 0.8f, 0.8f, 1), ICON_FK_LEVEL_UP);

			if (dialog.Display("OpenFileDialog", ImGuiWindowFlags_NoCollapse, min_size, max_size)){
				if (dialog.IsOk()){
					open(dialog.GetFilePathName());
				}
	                        dialog.Close();
				ImGui::PopFont();
	                        return false;
                        }

			ImGui::PopFont();
                        return true;
		}
	}
}
