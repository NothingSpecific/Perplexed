#include "definitions.h"

#include "regex_finder_thread.h"

#ifdef FINDER_THREAD_FPS_LIMIT
#include <chrono>
#endif

namespace Perplexed{
	regex_finder_thread::regex_finder_thread(GUI::editor_window *editor, std::regex rex, bool repeat){		
		this->rex = rex;
		
		construct(editor, repeat);
		
		thread = new std::thread(find, this);
	}
	void find(regex_finder_thread *thread){
		const bool repeat = thread->should_repeat();
		do{
			#ifdef FINDER_THREAD_FPS_LIMIT
			const std::chrono::high_resolution_clock::time_point tp_begin = std::chrono::high_resolution_clock::now();
			#endif
			
			thread->editor->mtx.lock();
			for(int a=0; a<thread->editor->editor->GetTotalLines() && thread->running; ++a){
				std::string line = thread->editor->editor->GetTextLine(a);
				thread->editor->editor->ClearFindResults(a);
				
				std::smatch match;
				int position = 0;
				while(std::regex_search(line, match, thread->rex) && thread->running){
					
					const size_t start = position + match.position();
					const size_t length = match[0].length();
					
					thread->editor->editor->AddFindResult(TextEditor::FindResult{a, start, a, length});
					
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
				thread->editor->mtx.unlock();
				thread->editor->mtx.lock();
			}
			thread->editor->mtx.unlock();
			
			#ifdef FINDER_THREAD_FPS_LIMIT
			const std::chrono::high_resolution_clock::time_point tp_end = std::chrono::high_resolution_clock::now();
			const long millis = std::chrono::duration_cast<std::chrono::milliseconds>(tp_end - tp_begin).count();
			const long wait_for = (1000.0 / FINDER_THREAD_FPS_LIMIT) - millis;
			if(wait_for > 0)
				std::this_thread::sleep_for(std::chrono::milliseconds(wait_for));
			#endif
		} while(repeat && thread->running);
		
		if(thread->running)
			thread->done = true;
		thread->running = false;
	}
}
