#define IMGUI_DEFINE_MATH_OPERATORS

// Each thread is framerate-limited separately
// The actual rendered framerate will always be limited by the main thread, which handles rendering
// Comment these out to remove framerate limitations
#define MAIN_THREAD_FPS_LIMIT 60
#define FINDER_THREAD_FPS_LIMIT 60
