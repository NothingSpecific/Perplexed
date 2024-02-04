#include "main_window.h"


#include "definitions.h"

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
#include "keyboard.h"

#include "fonts/font_intern.h" // For loading fonts

namespace Perplexed{
	namespace GUI{
		main_window::main_window(){}
		main_window::~main_window(){
			bool deleted_default_editor = false;
			for(editor_window *e : editors){
				if(e == editor)
					deleted_default_editor = true;
				delete e;
			}
			if(!deleted_default_editor)
				delete editor;
		}
		bool main_window::setup(){
			if(editor != nullptr)
				return true;
			
			editor = new editor_window(this);
			return editor->setup();
		}
		bool main_window::render(){
			const ImVec2 origin(0, 0);
			
			ImGui::PushFont(font::icon_font_karla_regular_forkawesome);
			
			ImGui::SetNextWindowSize(io->DisplaySize, ImGuiCond_Once);
			ImGui::SetNextWindowPos(origin);
			
			// Create our main window as a background for all other windows
			ImGui::Begin(name(), nullptr, flags);
			
			render_menu_bar(); // We want our menu bar to be a part of our main window
			render_tab_bar(); // We want our tab bar to be a part of our main window				
			
			// Use doubles for intermediate calculations, since I've had problems in the past with floating-point imprecision leading to elements being visibly misplaced
//			const double display_width = io->DisplaySize.x;
//			const double display_height = io->DisplaySize.y;
			const double frame_width = style->FrameBorderSize;
			const double frame_height = ImGui::GetCursorPos().y + (double) style->FramePadding.y * (double) 2.0;
			
			const ImVec2 main_window_size = ImVec2(io->DisplaySize.x, frame_height);
			ImGui::SetWindowSize(main_window_size);
			
			ImGui::End();
			
			const ImVec2 display_origin(frame_width, frame_height);
			const ImVec2 display_internal_size(io->DisplaySize.x - frame_width, io->DisplaySize.y - frame_height);
			
			if(show_open_dialog){
				open_dialog.open();
				show_open_dialog = open_dialog.render();
			}
			if(show_save_dialog){
				save_dialog.open();
				show_save_dialog = save_dialog.render(save_file);
			}
			if(show_find_dialog){
				show_find_dialog = find_dialog.render();
			}
			ImGui::PopFont();
			
			ImGui::SetNextWindowPos(display_origin);
			ImGui::SetNextWindowSize(display_internal_size);
			
			editor->render();
			
			return true;
		}
		bool main_window::render_tab_bar(){
			for(editor_window *e : editors){
				if(e == editor){
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4, 0.2, 0.2, 1.0));
					if(ImGui::Button(e->basename()))
						open(e->filename());
					ImGui::PopStyleColor();
				} else
					if(ImGui::Button(e->basename()))
						open(e->filename());
				ImGui::SameLine();
			}
			ImGui::NewLine();
			return true;
		}
		bool main_window::render_menu_bar(){
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save", "Ctrl-S"))
					{
						save();
					}
					if (ImGui::MenuItem("Save As", "Ctrl-Shift-S"))
					{
						save_as();
					}
					if (ImGui::MenuItem("Open", "Ctrl-O"))
					{
						open();
					}
					if (ImGui::MenuItem("Quit", "Alt-F4"))
						Perplexed::GUI::quit();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = editor->editor->IsReadOnly();
					if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
						editor->editor->SetReadOnly(ro);
					ImGui::Separator();

					if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, !ro && editor->editor->CanUndo()))
						editor->editor->Undo();
					if (ImGui::MenuItem("Redo", "Ctrl-Y | Ctrl-Shift-Z", nullptr, !ro && editor->editor->CanRedo()))
						editor->editor->Redo();

					ImGui::Separator();

					if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor->editor->HasSelection()))
						editor->editor->Copy();
					if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor->editor->HasSelection()))
						editor->editor->Cut();
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor->editor->HasSelection()))
						editor->editor->Delete();
					if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
						editor->editor->Paste();

					ImGui::Separator();

					if (ImGui::MenuItem("Select all", nullptr, nullptr))
						editor->editor->SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor->editor->GetTotalLines(), 0));

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("View"))
				{
					if (ImGui::MenuItem("Dark palette"))
						editor->editor->SetPalette(TextEditor::GetDarkPalette());
					if (ImGui::MenuItem("Light palette"))
						editor->editor->SetPalette(TextEditor::GetLightPalette());
					if (ImGui::MenuItem("Retro blue palette"))
						editor->editor->SetPalette(TextEditor::GetRetroBluePalette());
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
				return true;
			}
			return false;
		}
		
		bool main_window::open(const char *file){
			if(!is_open(file)){
				editor_window *e = new editor_window(this);
				if(e->open(file)){
					if(editors.empty() && editor != nullptr)
						delete editor;
					e->setup();
					editors.push_back(e);
					editor = e;
					return true;
				} else{
					delete e;
					return false;
				}
				
			} else
				for(editor_window *e : editors)
					if(!strcmp(e->filename(), file)){
						editor = e;
						return true;
					}
			return false;
		}
		bool main_window::force_save(){
			return editor->force_save();
		}
		void main_window::save(){
			if(editor->filename() == nullptr){
				save_as();
			} else
				editor->save();
		}
		void main_window::save_as(){
			show_save_dialog = true;
			save_file = editor;
		}
		void main_window::close(){
			editor->close();
		}
		void main_window::open(){
			show_open_dialog = true;
		}
		void main_window::find(){
			find_dialog.setup(this, editor);
			show_find_dialog = true;
		}
		const char *main_window::name(){
			return "Perplexed IDE";
		}
		bool main_window::is_open(const char *file){
			for(editor_window *e : editors){
				if(!strcmp(e->filename(), file))
					return true;
			}
			return false;
		}
	}
}
