#include "editor_window.h"

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


#include <fstream>
#include <iostream>

#include "gui.h"
#include "file_open_dialog.h"
#include "keyboard.h"

#include "fonts/font_intern.h" // For loading fonts
#include "filesystem.h"

#include <filesystem>

namespace Perplexed{
	namespace GUI{
		editor_window::editor_window(main_window *parent){
			this->parent = parent;
			editor = new TextEditor();
		}
		editor_window::~editor_window(){
			free((void*) file);
			free((void*) file_basename);
			free(window_name);
			delete editor;
			delete lang;
		}
		bool editor_window::setup(){
			lang = (TextEditor::LanguageDefinition*) TextEditor::LanguageDefinition::CPlusPlus();
		
			// set your own known preprocessor symbols...
			static const char* ppnames[] = { "NULL", "PM_REMOVE", "ZeroMemory", "DXGI_SWAP_EFFECT_DISCARD", "D3D_FEATURE_LEVEL", "D3D_DRIVER_TYPE_HARDWARE", "WINAPI","D3D11_SDK_VERSION", "assert" };
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
		
			for (unsigned long i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
			{
				TextEditor::Identifier id;
				id.mDeclaration = ppvalues[i];
				lang->mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
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
			for (unsigned long i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
			{
				TextEditor::Identifier id;
				id.mDeclaration = std::string(idecls[i]);
				lang->mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
			}
			editor->SetLanguageDefinition(lang);
			//editor->SetPalette(TextEditor::GetLightPalette());
		
			// error markers
			TextEditor::ErrorMarkers markers;
//			markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"Texteditor->h\""));
//			markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
			editor->SetErrorMarkers(markers);
		
			// "breakpoint" markers
			//TextEditor::Breakpoints bpts;
			//bpts.insert(24);
			//bpts.insert(47);
			//editor->SetBreakpoints(bpts);
			
			return true;
		}
		bool editor_window::render(){
			auto cpos = editor->GetCursorPosition();
			ImGui::Begin(name(), nullptr, flags);
			flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			
			ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor->GetTotalLines(),
				editor->IsOverwrite() ? "Ovr" : "Ins",
				!editor->IsSaved() ? "*" : " ",
				editor->GetLanguageDefinition()->mName.c_str(), file);

			editor->Render("TextEditor");
			ImGui::End();
			return true;
		}
		const char *editor_window::name(){
			return window_name != nullptr ? window_name : "editor_window {null}";
		}
		const char *editor_window::basename(){
			return file_basename;
		}
		const char *editor_window::filename(){
			return file;
		}
		
		bool editor_window::open(const char *file){
			set_file(file);
			
			if(std::filesystem::is_directory(file)){
				return false;
			}
			
			std::ifstream t(file);
			if (t.good())
			{
				std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
				editor->SetText(str);
				editor->MarkSaved();
				return true;
			}
			return false;
		}
		void editor_window::set_file(const char *file){
			free((void*) this->file);
			free((void*) this->file_basename);
			
			this->file = strdup(file);
			std::string bn = filesystem::basename(file);
			this->file_basename = strdup(bn.c_str());
			
			free(window_name);
			window_name = (char*) calloc(strlen(this->file) + 17, sizeof(char));
			strcpy(window_name, "editor_window [");
			window_name = strcat(window_name, this->file);
			window_name = strcat(window_name, "]");
		}
		bool editor_window::force_save(){
			std::ofstream t(file);
			if(t.good()){
				t << editor->GetText();
				t.close();
				editor->MarkSaved();
				return true;
			}
			return false;
		}
		bool editor_window::save(){
			if(editor->IsReadOnly()) return false;
			return force_save();
		}
		void editor_window::close(){
			free((void*) file);
			file = nullptr;
			file_basename = nullptr;
		}
	}
}
