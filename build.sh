#!/bin/bash

function invalid_arg(){
	echo "Invalid argument: '$1'" >&2
	exit 1
}

out_bin="Perplexed"

CC=("gcc")
CXX=("g++")
LD=("g++")

imgui_sources=(imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl2.cpp ImGuiFileDialog/ImGuiFileDialog.cpp ImGuiColorTextEdit/TextEditor.cpp)

c_sources=()
cxx_sources=(main.cpp gui.cpp file_open_dialog.cpp fonts/font.cpp keyboard.cpp)

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

gcc_args_release=(-fomit-frame-pointer -fexpensive-optimizations -flto -O3 -s -fstack-protector-explicit)
gxx_args_release=("${gcc_args_release}")

ld_args_release=(-fomit-frame-pointer -fexpensive-optimizations -flto -O3 -s -fstack-protector-explicit)
ld_args_ext_release=(-s)

gcc_search_directories=( glfw/include/GLFW/ SDL/include ./ imgui/ imgui/backends/ ImGuiFileDialog/)
gxx_search_directories=("${gcc_search_directories[@]}")

link_libs=(SDL/build/.libs/libSDL2.a SDL/build/.libs/libSDL2.so SDL/build/.libs/libSDL2_test.a SDL/build/.libs/libSDL2main.a glfw/build/src/libglfw3.a)

# -Werror is usually a good idea, since compiler warnings usually have a purpose
# We won't assume we're smarter than the compiler by ignoring warnings

# In fact, let's have the compiler give us more warnings, to ensure we're writing the best possible code (-Wall)
gcc_args=(-fexceptions -pthread -ldl -lGL -Wall -Werror)
gxx_args=("${gcc_args[@]}" -fpermissive -std=gnu++17)

file_args=()

# ImGuiFileDialog/ImGuiFileDialog.cpp throws a ton of unknown pragma warnings, but their developers are aware
# Their CMakeLists.txt suppresses these warnings, so we'll do the same
file_args+=("ImGuiFileDialog/ImGuiFileDialog.cpp" -Wno-unknown-pragmas)
file_args+=("file_open_dialog.cpp" -Wno-unknown-pragmas) # `file_open_dialog.cpp` includes `ImGuiFileDialog.h`, so we need to suppress errors here too

# ImGuiColorTextEdit/TextEditor.cpp throws plenty of errors when compiled with with -Wall
# Suppress them so we can focus on what we have control over: our own code
# I can't be too critical, since the warnings only appear with -Wall. With default warnings, it compiles cleanly
file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-sign-compare) # This usually isn't an issue, but can cause undefined behavior in certain cases
file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-unused-variable) # This isn't really ever an issue
file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-reorder) # Ignoring this can result in operations on uninitialized variables. Any standards-compliant C99 compiler initializes to 0 anyway, but it's still not a good idea
file_args+=("ImGuiColorTextEdit/TextEditor.cpp" -Wno-sequence-point) # This warns of possible undefined behavior in side-effects. It should probably be addressed

ld_args=(-pthread -pthread -flto)
ld_args_ext=(-ldl -lGL)

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

mkdir -p build/obj/"$build"
mkdir -p bin/"$build"

case "$build" in
release)
	gcc_args+=("${gcc_args_release[@]}")
	gxx_args+=("${gxx_args_release[@]}")
	ld_args+=("${ld_args_release[@]}")
	ld_args_ext+=("${ld_args_ext_release[@]}")
	;;
debug)
	gcc_args+=("${gcc_args_dbg[@]}")
	gxx_args+=("${gcc_args_dbg[@]}")
	ld_args+=("${ld_args_dbg[@]}")
	ld_args_ext+=("${ld_args_ext_dbg[@]}")
	;;
tsan|debug_thread)
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

	cmd=("${CXX[@]}" "${gcc_args[@]}" -I"$dirname" "${pfa[@]}" -c "$f" -o build/obj/"$build"/"$basename".o)

	echo "${cmd[@]}"
	"${cmd[@]}"
done

cmd=("${LD[@]}" -o bin/"$build"/"$out_bin" "${ld_args[@]}" "${obj_files[@]}" "${ld_args_ext[@]}")
echo "${cmd[@]}"
"${cmd[@]}"
