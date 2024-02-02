#ifndef PERPLEXED_WINDOW_H
#define PERPLEXED_WINDOW_H

namespace Perplexed{
	namespace GUI{
		class window{
		private:
		public:
			virtual ~window();
			// Setup the window, return true if success, return false if error
			virtual bool setup();
			// Render the window, return true if success, return false if error
			virtual bool render() = 0;
			
			// `name()` should always return the window name as recognized by Dear ImGui
			// If no corresponding Dear ImGui window exists, return `nullptr`
			virtual const char *name() = 0;
		};
	}
}

#endif
