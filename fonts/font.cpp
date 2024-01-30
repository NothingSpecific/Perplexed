
// Include our embedded fonts
#include "fonts/Cousine-Regular.h"
#include "fonts/DroidSans.h"
#include "fonts/Karla-Regular.h"
#include "fonts/ProggyClean.h"
#include "fonts/ProggyTiny.h"
#include "fonts/Roboto-Medium.h"

#include "icon_fonts/forkawesome_webfont.h"
#include "icon_fonts/MaterialIcons_Regular.h"
#include "icon_fonts/IconsForkAwesome.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

#include "gui.h"

namespace Perplexed{
	namespace GUI{
		namespace font{
			static const ImWchar icon_ranges_forkawesome[] = { ICON_MIN_FK, ICON_MAX_FK, 0 };
	
			ImFont* icon_font_default_forkawesome;
			ImFont* font_karla_regular;
			ImFont* font_cousine_regular;
			ImFont* font_roboto_medium;
			ImFont* font_proggy_clean;
			ImFont* font_proggy_tiny;
			ImFont* font_droid_sans;
			ImFont* font_karla_title;
			ImFont* font_cousine_title;
			ImFont* font_roboto_title;
			ImFont* font_proggy_title;
			ImFont* font_proggy_tiny_title;
			ImFont* font_droid_title;
			ImFont* icon_font_material_icons_regular;
			ImFont* icon_font_forkawesome_regular;
			ImFont* icon_font_karla_regular_forkawesome;

			int font_size = 18;
			int title_font_size = 18;
			int icon_font_size = 12;

			bool reload = false;

			void _load(){
				/* Set title to same size as main font */
				title_font_size = font_size;

				ImFontConfig config_merge;
				config_merge.MergeMode = true;
				config_merge.FontDataOwnedByAtlas = false;
				
				ImFontConfig font_config;
				font_config.FontDataOwnedByAtlas = false;

				io->Fonts->AddFontDefault();
				icon_font_default_forkawesome = io->Fonts->AddFontFromMemoryCompressedTTF(forkawesome_webfont_compressed_data, forkawesome_webfont_compressed_size, icon_font_size, &config_merge, icon_ranges_forkawesome);
				IM_ASSERT(icon_font_default_forkawesome != nullptr);

				font_karla_regular = io->Fonts->AddFontFromMemoryCompressedTTF(Karla_Regular_compressed_data, Karla_Regular_compressed_size, font_size, &font_config);
				IM_ASSERT(font_karla_regular != nullptr);
				font_cousine_regular = io->Fonts->AddFontFromMemoryCompressedTTF(Cousine_Regular_compressed_data, Cousine_Regular_compressed_size, font_size, &font_config);
				IM_ASSERT(font_cousine_regular != nullptr);
				font_roboto_medium = io->Fonts->AddFontFromMemoryCompressedTTF(Roboto_Medium_compressed_data, Roboto_Medium_compressed_size, font_size, &font_config);
				IM_ASSERT(font_roboto_medium != nullptr);
				font_proggy_clean = io->Fonts->AddFontFromMemoryCompressedTTF(ProggyClean_compressed_data, ProggyClean_compressed_size, font_size, &font_config);
				IM_ASSERT(font_proggy_clean != nullptr);
				font_proggy_tiny = io->Fonts->AddFontFromMemoryCompressedTTF(ProggyTiny_compressed_data, ProggyTiny_compressed_size, font_size, &font_config);
				IM_ASSERT(font_proggy_tiny != nullptr);
				font_droid_sans = io->Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, font_size, &font_config);
				IM_ASSERT(font_droid_sans != nullptr);
				font_karla_title = io->Fonts->AddFontFromMemoryCompressedTTF(Karla_Regular_compressed_data, Karla_Regular_compressed_size, title_font_size, &font_config);
				IM_ASSERT(font_karla_title != nullptr);
				font_cousine_title = io->Fonts->AddFontFromMemoryCompressedTTF(Cousine_Regular_compressed_data, Cousine_Regular_compressed_size, title_font_size, &font_config);
				IM_ASSERT(font_cousine_title != nullptr);
				font_roboto_title = io->Fonts->AddFontFromMemoryCompressedTTF(Roboto_Medium_compressed_data, Roboto_Medium_compressed_size, title_font_size, &font_config);
				IM_ASSERT(font_roboto_title != nullptr);
				font_proggy_title = io->Fonts->AddFontFromMemoryCompressedTTF(ProggyClean_compressed_data, ProggyClean_compressed_size, title_font_size, &font_config);
				IM_ASSERT(font_proggy_title != nullptr);
				font_proggy_tiny_title = io->Fonts->AddFontFromMemoryCompressedTTF(ProggyTiny_compressed_data, ProggyTiny_compressed_size, title_font_size, &font_config);
				IM_ASSERT(font_proggy_tiny_title != nullptr);
				font_droid_title = io->Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, title_font_size, &font_config);
				IM_ASSERT(font_droid_title != nullptr);
				/* Icon fonts */
				icon_font_material_icons_regular = io->Fonts->AddFontFromMemoryCompressedTTF(MaterialIcons_Regular_compressed_data, MaterialIcons_Regular_compressed_size, icon_font_size, &font_config);
				IM_ASSERT(icon_font_material_icons_regular != nullptr);
				icon_font_forkawesome_regular = io->Fonts->AddFontFromMemoryCompressedTTF(forkawesome_webfont_compressed_data, forkawesome_webfont_compressed_size, icon_font_size, &font_config);
				IM_ASSERT(icon_font_forkawesome_regular != nullptr);
				/* Merge icons into default Karla Icons */
				icon_font_karla_regular_forkawesome = io->Fonts->AddFontFromMemoryCompressedTTF(Karla_Regular_compressed_data, Karla_Regular_compressed_size, font_size, &font_config);
				IM_ASSERT(icon_font_karla_regular_forkawesome != nullptr);
				icon_font_karla_regular_forkawesome = io->Fonts->AddFontFromMemoryCompressedTTF(forkawesome_webfont_compressed_data, forkawesome_webfont_compressed_size, font_size, &config_merge, icon_ranges_forkawesome);
				IM_ASSERT(icon_font_karla_regular_forkawesome != nullptr);
			}
			void _reload(){
				io->Fonts->Clear();
				_load();
				ImGui_ImplOpenGL3_CreateFontsTexture();
			}
			void set_font_size(int font_size){
				Perplexed::GUI::font::font_size = font_size;
				reload = true;
			}
		}
	}
}
