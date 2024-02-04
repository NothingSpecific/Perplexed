#include "regex_finder_thread.h"

namespace Perplexed{
	regex_finder_thread::regex_finder_thread(GUI::editor_window *editor, std::regex rex){		
		this->rex = rex;
		
		construct(editor);
		
		thread = new std::thread(find, this);
	}
	void find(regex_finder_thread *thread){
		thread->editor->mtx.lock();
		for(int a=0; a<thread->editor->editor->GetTotalLines() && thread->running; ++a){
			std::string line = thread->editor->editor->GetTextLine(a);
			thread->editor->editor->ClearFindResults(a+1);
			thread->editor->mtx.unlock();
			
			std::smatch match;
			int position = 0;
			while(std::regex_search(line, match, thread->rex) && thread->running){
				
				size_t start = position + match.position();
				size_t length = match[0].length();
				
				thread->editor->mtx.lock();
				thread->editor->editor->AddFindResult(TextEditor::FindResult{a+1, start, a+1, length});
				thread->editor->mtx.unlock();
				
				// Add sub-matches, if applicable
				/*
				for(size_t b=1; b<match.size(); ++b){
					std::ssub_match sub_match = match[b];
					int end_position = position + sub_match.length();
					thread->editor->editor->AddFindResult(TextEditor::FindResult{a, position, a, end_position});
					position = end_position;
				}
				*/
				line = match.suffix().str();
				position = start + length;
			}
			thread->editor->mtx.lock();
		}
		thread->editor->mtx.unlock();
		
		thread->running = false;
		thread->done = true;
	}
}
