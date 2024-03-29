#!/bin/bash

function invalid_arg(){
	echo "Invalid argument: '$1'" >&2
	exit 1
}

out_bin="Perplexed"

CC=("gcc")
CXX=("g++")
LD=("g++")

imgui_sources=(imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp ImGuiFileDialog/ImGuiFileDialog.cpp ImGuiColorTextEdit/TextEditor.cpp imgui/misc/cpp/imgui_stdlib.cpp)

c_sources=()
cxx_sources=(src/main.cpp src/gui.cpp src/file_open_dialog.cpp src/file_save_dialog.cpp fonts/font.cpp src/keyboard.cpp src/window.cpp src/main_window.cpp src/editor_window.cpp src/filesystem.cpp src/finder_thread.cpp src/regex_finder_thread.cpp src/find_dialog.cpp)

cxx_sources+=("${imgui_sources[@]}")

# Debug builds will run with ASAN and UBSAN. This way we can address (almost) all sources of undefined behavior
# We'll also use LSAN to detect memory leaks
gcc_args_dbg=(-g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all -fstack-check)
gxx_args_dbg=("${gcc_args_dbg[@]}")

ld_args_dbg=(-g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope)
ld_args_ext_dbg=()

# We'll use a separate build for TSAN, since TSAN and ASAN/LSAN are mutually exclusive
gcc_args_dbg_thread=(-g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=thread -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all -fstack-check)
gxx_args_dbg_thread=("${gcc_args_dbg[@]}")

ld_args_dbg_thread=(-g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=thread -fsanitize-address-use-after-scope)
ld_args_ext_dbg_thread=()

gcc_args_release=(-fomit-frame-pointer -fexpensive-optimizations -flto -O3 -fstack-protector-explicit)
gxx_args_release=("${gcc_args_release}")

ld_args_release=(-fomit-frame-pointer -fexpensive-optimizations -flto -O3 -fstack-protector-explicit)
ld_args_ext_release=()

gcc_args_vg=(-g "${gcc_args_release[@]}")
gxx_args_vg=(-g "${gxx_args_release[@]}")

gcc_args_vgdbg=(-g)
gxx_args_vgdbg=(-g)

ld_args_vgdbg=(-g)
ld_args_ext_vgdbg=()
ld_args_vgdbg=(-g)
ld_args_ext_vgdbg=()

# Strip generated executables for release builds
gcc_args_release+=(-s)
gxx_args_release+=(-s)
ld_args_release+=(-s)
ld_args_ext_release+=(-s)

gcc_search_directories=( glfw/include/GLFW/ SDL/include ./ src/ imgui/ imgui/backends/ ImGuiFileDialog/)
gxx_search_directories=("${gcc_search_directories[@]}")

link_libs=(SDL/build/.libs/libSDL2.a SDL/build/.libs/libSDL2.so SDL/build/.libs/libSDL2_test.a SDL/build/.libs/libSDL2main.a glfw/build/src/libglfw3.a)

# -Werror is usually a good idea, since compiler warnings usually have a purpose
# We won't assume we're smarter than the compiler by ignoring warnings

# In fact, let's have the compiler give us more warnings, to ensure we're writing the best possible code (-Wall)
gcc_args=(-fexceptions -pthread -ldl -lGL -Wall -Werror -Wno-unknown-pragmas)
gxx_args=("${gcc_args[@]}" -fpermissive -std=gnu++17)

file_args=()

# ImGuiFileDialog/ImGuiFileDialog.cpp throws a ton of unknown pragma warnings, but their developers are aware
# Their CMakeLists.txt suppresses these warnings, so we'll do the same

# These are commented out since they're now redundant - `-Wno-unknown-pragmas` is now being set for all compiled files
#file_args+=("ImGuiFileDialog/ImGuiFileDialog.cpp" -Wno-unknown-pragmas)
#file_args+=("src/file_open_dialog.cpp" -Wno-unknown-pragmas) # `file_open_dialog.cpp` includes `ImGuiFileDialog.h`, so we need to suppress errors here too
#file_args+=("src/file_save_dialog.cpp" -Wno-unknown-pragmas)

# ImGuiColorTextEdit/TextEditor.cpp throws plenty of errors when compiled with with -Wall
# Suppress them so we can focus on what we have control over: our own code
# I can't be too critical, since the warnings only appear with -Wall. With default warnings, it compiles cleanly
file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-sign-compare) # This usually isn't an issue, but can cause undefined behavior in certain cases
#file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-unused-variable) # This isn't really ever an issue
#file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-reorder) # Ignoring this can result in operations on uninitialized variables. Any standards-compliant C99 compiler initializes to 0 anyway, but it's still not a good idea
#file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-sequence-point) # This warns of possible undefined behavior in side-effects. It should probably be addressed

ld_args=(-pthread -pthread -flto)
ld_args_ext=(-ldl -lGL)

owd="$PWD"

build=""
for arg in "$@"; do
	case "$arg" in
	--build=*)
		if [[ "$build" != "" ]]; then
			echo "Error: '--build=<...>' passed twice. Aborting." >&2
			exit
		fi
		build="$(echo -n "$arg" | cut -d= -f2-)"
		;;
	*)
		invalid_arg "$arg"
		;;
	esac
done
if [[ "$build" == "" ]]; then
	build=all
fi
if [[ "$build" == "all" ]]; then
	build=release

	# Build dependencies	
	echo "Building GLFW..."
	bash build_glfw.sh
	echo "Done."

	echo "Building SDL..."
	bash build_sdl.sh
	echo "Done."
fi
depbuild=false

submodules_forked=(ImGuiFileDialog ImGuiColorTextEdit imgui)

case "$build" in
reldeps|dbgdeps|tsandeps|vgdeps)
	depbuild=true
	for module in "${submodules_forked[@]}"; do
		cp -r ~/git/"$module"/* "$module"/ ||
		echo "Failed to pull submodule '$module'"
	done
	;;
esac

case "$build" in
release|reldeps)
	build=release
	gcc_args+=("${gcc_args_release[@]}")
	gxx_args+=("${gxx_args_release[@]}")
	ld_args+=("${ld_args_release[@]}")
	ld_args_ext+=("${ld_args_ext_release[@]}")
	;;
valgrind|vgrel|vg)
	build=valgrind
	gcc_args+=("${gcc_args_vg[@]}")
	gxx_args+=("${gcc_args_vg[@]}")
	ld_args+=("${ld_args_vg[@]}")
	ld_args_ext+=("${ld_args_ext_vg[@]}")
	;;
debug|dbgdeps)
	build=debug
	gcc_args+=("${gcc_args_dbg[@]}")
	gxx_args+=("${gcc_args_dbg[@]}")
	ld_args+=("${ld_args_dbg[@]}")
	ld_args_ext+=("${ld_args_ext_dbg[@]}")
	;;
vgdbg|vgdbgdeps)
	build=vgdbg
	gcc_args+=("${gcc_args_vgdbg[@]}")
	gxx_args+=("${gcc_args_vgdbg[@]}")
	ld_args+=("${ld_args_vgdbg[@]}")
	ld_args_ext+=("${ld_args_ext_vgdbg[@]}")
	;;
tsan|debug_thread|tsandeps)
	# Standardize build directory
	build=tsan
	
	gcc_args+=("${gcc_args_dbg_thread[@]}")
	gxx_args+=("${gcc_args_dbg_thread[@]}")
	ld_args+=("${ld_args_dbg_thread[@]}")
	ld_args_ext+=("${ld_args_ext_dbg_thread[@]}")
	;;
clean)
	rm -rf build/
	rm -rf bin/
	exit
	;;
install)
	if [[ "$UID" != 0 ]]; then
		echo "action 'install' must be run as root..." >&2
		exit
	fi
	cat bin/release/Perplexed > /usr/local/bin/perplexed
	chmod u+rx,g+rx,o+rx /usr/local/bin/perplexed
	exit
	;;
*)
	echo "Invalid build: '$build'" >&2
	exit 1
	;;
esac

mkdir -p build/obj/"$build"
mkdir -p bin/"$build"

for d in "${gcc_search_directories[@]}"; do
	gcc_args+=(-I"$d")
done
for d in "${gxx_search_directories[@]}"; do
	gxx_args+=(-I"$d")
done

ld_args_ext+=()
for d in "${link_libs[@]}"; do
	ld_args_ext+=("$d")
done

obj_files=()

for f in "${c_sources[@]}"; do
	pfa=()
	ix=0
	while [[ "$ix" -lt "${#file_args[@]}" ]]; do
		file="${file_args["$ix"]}"
		if [[ "$file" == "$f" ]]; then
			arg="${file_args["$((ix+1))"]}"
			pfa+=("$arg")
		fi
		ix="$((ix+2))"
	done
	ext=".${f##*.}"
	basename="$(basename "$f" "$ext")"
	dirname="$(dirname "$f")"
	obj_files+=(build/obj/"$build"/"$basename".o)

	cmd=("${CC[@]}" "${gcc_args[@]}" -I"$dirname" "${pfa[@]}" -c "$f" -o build/obj/"$build"/"$basename".o)

	echo "${cmd[@]}"
	"${cmd[@]}"
done
for f in "${cxx_sources[@]}"; do
	pfa=()
	ix=0
	while [[ "$ix" -lt "${#file_args[@]}" ]]; do
		file="${file_args["$ix"]}"
		if [[ "$file" == "$f" ]]; then
			arg="${file_args["$((ix+1))"]}"
			pfa+=("$arg")
		fi
		ix="$((ix+2))"
	done
	ext=".${f##*.}"
	basename="$(basename "$f" "$ext")"
	dirname="$(dirname "$f")"
	obj_files+=(build/obj/"$build"/"$basename".o)

	cmd=("${CXX[@]}" "${gxx_args[@]}" -I"$dirname" "${pfa[@]}" -c "$f" -o build/obj/"$build"/"$basename".o)

	echo "${cmd[@]}"
	"${cmd[@]}"
done

cmd=("${LD[@]}" -o bin/"$build"/"$out_bin" "${ld_args[@]}" "${obj_files[@]}" "${ld_args_ext[@]}")
echo "${cmd[@]}"
"${cmd[@]}"

if [[ "$depbuild" == true ]]; then
	for module in "${submodules_forked[@]}"; do
		success=false
		cd "$owd" &&
		cd "$module"/ &&
		git stash &&
		success=true

		if [[ "$success" != true ]]; then
			echo "Failed to stash submodule '$module'"
		fi
	done
	cd "$owd"
fi
