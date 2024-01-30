
#include "gui.h"

#include <SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

namespace Perplexed{
	namespace GUI{
		namespace keys{
			int n_keys = 0;
			const uint8_t *input;
			void handle_inputs(){
				input = SDL_GetKeyboardState(&n_keys);
//				if(n_keys == 2){
					if(input[SDL_SCANCODE_LCTRL] || input[SDL_SCANCODE_RCTRL]){
						if(input[SDL_SCANCODE_O]) open();
						else if(input[SDL_SCANCODE_S]) save();
					}
//				}
			}
		}
	}
}
