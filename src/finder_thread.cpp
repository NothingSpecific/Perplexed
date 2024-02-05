#include "finder_thread.h"

namespace Perplexed{
	void finder_thread::construct(GUI::editor_window *editor, bool repeat){
		this->editor = editor;
		this->repeat = repeat;
		running = true;
		done = false;
	}
	finder_thread::~finder_thread(){
		finish();
	}
	void finder_thread::stop(){
		running = false;
	}
	void finder_thread::finish(){
		running = false;
		mtx.lock();
		if(thread != nullptr){
			thread->join();
			delete thread;
			thread = nullptr;
		}
		mtx.unlock();
	}
	void finder_thread::join(){
		mtx.lock();
		if(thread != nullptr)
			thread->join();
		mtx.unlock();
	}
	bool finder_thread::should_repeat(){
		return repeat;
	}
}
