/* Release entry point.
 * The original C implementation remains in Submitables/ for academic reference.
 * This wrapper makes the application independent of the caller's working directory.
 */
#define main academic_portal_main
#include "../Submitables/AcademicSystemPortal.c"
#undef main

#include <direct.h>
#include <windows.h>

static void setApplicationDirectory(void) {
    char path[MAX_PATH];
    char *slash;

    if (!GetModuleFileNameA(NULL, path, MAX_PATH))
        return;

    slash = strrchr(path, '\\');
    if (slash) *slash = '\0';

    SetCurrentDirectoryA(path);
    _mkdir("data");
    SetCurrentDirectoryA("data");
}

int main(void) {
    setApplicationDirectory();
    return academic_portal_main();
}
