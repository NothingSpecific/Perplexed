#ifndef PERPLEXED_REGEX_FINDER_THREAD_H
#define PERPLEXED_REGEX_FINDER_THREAD_H

#include "finder_thread.h"

#include <regex>

namespace Perplexed{
	class regex_finder_thread : public finder_thread{
		public:
			std::regex rex;
			regex_finder_thread(GUI::editor_window *editor, std::regex rex, bool repeat);
	};
	void find(regex_finder_thread *thread);
}

#endif
