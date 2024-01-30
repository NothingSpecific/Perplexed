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

namespace Perplexed{
	namespace GUI{
		ImVec4 clear_color;
		ImGuiStyle *style;
		ImGuiIO *io;

		const char *file = nullptr;

		TextEditor editor;
		bool file_open_dialog = false;

		int run(){
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
		
			///////////////////////////////////////////////////////////////////////
			// TEXT EDITOR SAMPLE
			auto lang = TextEditor::LanguageDefinition::CPlusPlus();
		
			// set your own known preprocessor symbols...
			static const char* ppnames[] = { "NULL", "PM_REMOVE",
				"ZeroMemory", "DXGI_SWAP_EFFECT_DISCARD", "D3D_FEATURE_LEVEL", "D3D_DRIVER_TYPE_HARDWARE", "WINAPI","D3D11_SDK_VERSION", "assert" };
			// ... and their corresponding values
			static const char* ppvalues[] = { 
				"#define NULL ((void*)0)", 
				"#define PM_REMOVE (0x0001)",
				"Microsoft's own memory zapper function\n(which is a macro actually)\nvoid ZeroMemory(\n\t[in] PVOID  Destination,\n\t[in] SIZE_T Length\n); ", 
				"enum DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD = 0", 
				"enum D3D_FEATURE_LEVEL", 
				"enum D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE  = ( D3D_DRIVER_TYPE_UNKNOWN + 1 )",
				"#define WINAPI __stdcall",
				"#define D3D11_SDK_VERSION (7)",
				" #define assert(expression) (void)(												  \n"
				"	(!!(expression)) ||															  \n"
				"	(_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \n"
				" )"
				};
		
			for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
			{
				TextEditor::Identifier id;
				id.mDeclaration = ppvalues[i];
				lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
			}
		
			// set your own identifiers
			static const char* identifiers[] = {
				"HWND", "HRESULT", "LPRESULT","D3D11_RENDER_TARGET_VIEW_DESC", "DXGI_SWAP_CHAIN_DESC","MSG","LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
				"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
				"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
				"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "TextEditor" };
			static const char* idecls[] = 
			{
				"typedef HWND_* HWND", "typedef long HRESULT", "typedef long* LPRESULT", "struct D3D11_RENDER_TARGET_VIEW_DESC", "struct DXGI_SWAP_CHAIN_DESC",
				"typedef tagMSG MSG\n * Message structure","typedef LONG_PTR LRESULT","WPARAM", "LPARAM","UINT","LPVOID",
				"ID3D11Device", "ID3D11DeviceContext", "ID3D11Buffer", "ID3D11Buffer", "ID3D10Blob", "ID3D11VertexShader", "ID3D11InputLayout", "ID3D11Buffer",
				"ID3D10Blob", "ID3D11PixelShader", "ID3D11SamplerState", "ID3D11ShaderResourceView", "ID3D11RasterizerState", "ID3D11BlendState", "ID3D11DepthStencilState",
				"IDXGISwapChain", "ID3D11RenderTargetView", "ID3D11Texture2D", "class TextEditor" };
			for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
			{
				TextEditor::Identifier id;
				id.mDeclaration = std::string(idecls[i]);
				lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
			}
			editor.SetLanguageDefinition(lang);
			//editor.SetPalette(TextEditor::GetLightPalette());
		
			// error markers
			TextEditor::ErrorMarkers markers;
//			markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
//			markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
			editor.SetErrorMarkers(markers);
		
			// "breakpoint" markers
			//TextEditor::Breakpoints bpts;
			//bpts.insert(24);
			//bpts.insert(47);
			//editor.SetBreakpoints(bpts);
		
			// Main loop
			bool done = false;
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

				ImGui::PushFont(font::icon_font_karla_regular_forkawesome);


				ImGui::SetNextWindowSize(ImVec2(
						io.DisplaySize.x - (style.FramePadding.x * 2.0f) - (style.FrameBorderSize * 2.0f),
						io.DisplaySize.y - (style.FramePadding.y * 2.0f) - (style.FrameBorderSize * 2.0f)
					));
				ImGui::SetNextWindowPos(origin);
		
				// For now we'll just show a demo window based on https://github.com/BalazsJako/ColorTextEditorDemo
				// See: https://github.com/BalazsJako/ColorTextEditorDemo/blob/master/LICENSE

				auto cpos = editor.GetCursorPosition();
				ImGui::Begin("Perplexed IDE", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

				//ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Save", "Ctrl-S"))
						{
							save();
						}
						if (ImGui::MenuItem("Open", "Ctrl-O"))
						{
							open();
						}
						if (ImGui::MenuItem("Quit", "Alt-F4"))
							break;
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Edit"))
					{
						bool ro = editor.IsReadOnly();
						if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
							editor.SetReadOnly(ro);
						ImGui::Separator();
		
						if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, !ro && editor.CanUndo()))
							editor.Undo();
						if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
							editor.Redo();
		
						ImGui::Separator();
		
						if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
							editor.Copy();
						if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
							editor.Cut();
						if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
							editor.Delete();
						if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
							editor.Paste();
		
						ImGui::Separator();
		
						if (ImGui::MenuItem("Select all", nullptr, nullptr))
							editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));
		
						ImGui::EndMenu();
					}
		
					if (ImGui::BeginMenu("View"))
					{
						if (ImGui::MenuItem("Dark palette"))
							editor.SetPalette(TextEditor::GetDarkPalette());
						if (ImGui::MenuItem("Light palette"))
							editor.SetPalette(TextEditor::GetLightPalette());
						if (ImGui::MenuItem("Retro blue palette"))
							editor.SetPalette(TextEditor::GetRetroBluePalette());
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
		
				ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
					editor.IsOverwrite() ? "Ovr" : "Ins",
					!editor.IsSaved() ? "*" : " ",
					editor.GetLanguageDefinition().mName.c_str(), file);
		
				editor.Render("TextEditor");
				ImGui::PopFont();
				ImGui::End();

				if(file_open_dialog){
					file_open_dialog = Perplexed::GUI::show_file_open_dialog();
				}
					

				// Rendering
				ImGui::Render();
				glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
				glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
				glClear(GL_COLOR_BUFFER_BIT);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				SDL_GL_SwapWindow(window);
			}

			// Cleanup
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
			
			SDL_GL_DeleteContext(gl_context);
			SDL_DestroyWindow(window);
			SDL_Quit();

			return 0;
		
		}

		void open(std::string file){
			free((void*) Perplexed::GUI::file);
			Perplexed::GUI::file = strdup(file.c_str());
			std::ifstream t(file);
			if (t.good())
			{
				std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
				editor.SetText(str);
				editor.MarkSaved();
			}
		}
		void force_save(){
			std::ofstream t(file);
			if(t.good()){
				t << editor.GetText();
				t.close();
				editor.MarkSaved();
			}
		}
		void save(){
			if(editor.IsReadOnly()) return;
			force_save();
		}
		void close(){
			free((void*) file);
			file = nullptr;
		}
		void open(){
			file_open_dialog = true;
		}
	}
}
