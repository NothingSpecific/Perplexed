#ifndef PERPLEXED_FINDER_THREAD_H
#define PERPLEXED_FINDER_THREAD_H

#include <atomic>
#include <mutex>
#include <thread>

#include "editor_window.h"

namespace Perplexed{
	class finder_thread{
		protected:
			std::thread *thread = nullptr;
			std::mutex mtx;
			
		public:
			GUI::editor_window *editor;
			
			std::atomic_bool running;
			std::atomic_bool done;
			
			virtual void construct(GUI::editor_window *editor);
			
			virtual ~finder_thread();
			
			// Tells the thread to stop. To wait for the the thread to finish, either call `finish()`, or call `stop()` then `join()`
			virtual void stop();
			virtual void finish();
			virtual void join();
	};
}

#endif
