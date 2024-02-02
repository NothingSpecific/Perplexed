
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
				bool ctrl = input[SDL_SCANCODE_LCTRL] || input[SDL_SCANCODE_RCTRL];
				bool shift = input[SDL_SCANCODE_LSHIFT] || input[SDL_SCANCODE_RSHIFT];
				bool alt = input[SDL_SCANCODE_LALT] || input[SDL_SCANCODE_RALT];
				
				if(ctrl){
					if(!shift){
						if(!alt){
							if(input[SDL_SCANCODE_O]) open();
							else if(input[SDL_SCANCODE_S]) save();
							else if(input[SDL_SCANCODE_F]) find();
						}
					} else{
						if(!alt){
							if(input[SDL_SCANCODE_S]) save_as();
						}
					}
				}
			}
		}
	}
}
