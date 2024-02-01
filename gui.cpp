// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ				  https://dearimgui.com/faq
// - Getting Started	  https://dearimgui.com/getting-started
// - Documentation		https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "ImGuiColorTextEdit/TextEditor.h"

#include <fstream>
#include <iostream>

#include "gui.h"
#include "file_open_dialog.h"
#include "keyboard.h"

#include "fonts/font_intern.h" // For loading fonts

#include "main_window.h"

namespace Perplexed{
	namespace GUI{
		ImVec4 clear_color;
		ImGuiStyle *style;
		ImGuiIO *io;
		
		main_window *mw;
		bool done;

		int run(){
			done = false;
			// Setup SDL
			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
			{
				printf("Error: %s\n", SDL_GetError());
				return -1;
			}
		
			// Decide GL+GLSL versions
		#if defined(IMGUI_IMPL_OPENGL_ES2)
			// GL ES 2.0 + GLSL 100
			const char* glsl_version = "#version 100";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		#elif defined(__APPLE__)
			// GL 3.2 Core + GLSL 150
			const char* glsl_version = "#version 150";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		#else
			// GL 3.0 + GLSL 130
			const char* glsl_version = "#version 130";
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		#endif
		
			// From 2.0.18: Enable native IME.
		#ifdef SDL_HINT_IME_SHOW_UI
			SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
		#endif
		
			// Create window with graphics context
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
			SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
			SDL_Window* window = SDL_CreateWindow("Perplexed IDE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
			if (window == nullptr)
			{
				printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
				return -1;
			}
		
			SDL_GLContext gl_context = SDL_GL_CreateContext(window);
			SDL_GL_MakeCurrent(window, gl_context);
			SDL_GL_SetSwapInterval(1); // Enable vsync
		
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	  // Enable Gamepad Controls

			Perplexed::GUI::io = &io;

			font::_load(); // Load our fonts
		
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();
		
			// Setup Platform/Renderer backends
			ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
			ImGui_ImplOpenGL3_Init(glsl_version);
		
			// Load Fonts
			// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
			// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
			// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
			// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
			// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
			// - Read 'docs/FONTS.md' for more instructions and details.
			// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
			// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
			//io.Fonts->AddFontDefault();
			//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
			//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
			//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
			//IM_ASSERT(font != nullptr);
		
			// (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
			clear_color = ImColor(114, 144, 154);
			const ImVec2 origin(0.0f, 0.0f);
			ImGuiStyle style = ImGui::GetStyle();
			Perplexed::GUI::style = &style;
			
			mw = new main_window();
			mw->setup();

			// Main loop
			while (!done)
			{
				SDL_Event event;
				while (SDL_PollEvent(&event))
				{
					ImGui_ImplSDL2_ProcessEvent(&event);
					switch(event.type){
					case SDL_QUIT:
						done = true;
					case SDL_KEYDOWN:
						break;
					case SDL_KEYUP:
						break;
					default:
						if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
							done = true;
					break;
					}
				}
				
				// Start the Dear ImGui frame
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame();
				ImGui::NewFrame();

				keys::handle_inputs();


				ImGui::SetNextWindowSize(ImVec2(
						io.DisplaySize.x - (style.FramePadding.x * 2.0f) - (style.FrameBorderSize * 2.0f),
						io.DisplaySize.y - (style.FramePadding.y * 2.0f) - (style.FrameBorderSize * 2.0f)
					));
				ImGui::SetNextWindowPos(origin);
		
				// For now we'll just show a demo window based on https://github.com/BalazsJako/ColorTextEditorDemo
				// See: https://github.com/BalazsJako/ColorTextEditorDemo/blob/master/LICENSE
					
				mw->render();

				// Rendering
				ImGui::Render();
				glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
				glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
				glClear(GL_COLOR_BUFFER_BIT);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				SDL_GL_SwapWindow(window);
			}
			
			delete mw;

			// Cleanup
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
			
			SDL_GL_DeleteContext(gl_context);
			SDL_DestroyWindow(window);
			SDL_Quit();

			return 0;
		
		}

		void open(const char *file){
			mw->open(file);
		}
		void force_save(){
			mw->force_save();
		}
		void save(){
			mw->save();
		}
		void close(){
			mw->close();
		}
		void open(){
			mw->open();
		}
		void find(){
			mw->find();
		}
		void quit(){
			done = true;
		}
	}
}
