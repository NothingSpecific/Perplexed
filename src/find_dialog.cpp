#include "find_dialog.h"

#include <regex>
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "gui.h"

namespace Perplexed{
	namespace GUI{
		void find_dialog::setup(main_window *mw, editor_window *editor){
			this->mw = mw;
			this->editor = editor;
		}
		bool find_dialog::render(){
			bool opened = true;
			ImGui::Begin(name(), &opened, flags);
			
			if(ImGui::InputText("RegEx", &search_string)){
				if(!search_string.empty())
					try{
						mw->editor->find(std::regex(search_string));
					} catch(std::regex_error &){
						mw->editor->mtx.lock();
						mw->editor->editor->ClearFindResults();				
						mw->editor->mtx.unlock();
					}
				else{
					mw->editor->mtx.lock();
					mw->editor->editor->ClearFindResults();				
					mw->editor->mtx.unlock();
				}
				ImGui::End();
				return true;
			}
			ImGui::End();
			return opened;
		}
		const char *find_dialog::name(){
			return "Find";
		}
	}
}
