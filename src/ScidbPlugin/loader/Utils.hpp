#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <string>

#ifndef BUILD_PATH
#  define BUILD_PATH "/usr/lib64/scidbroot/"
#endif
inline std::string getHandlerLibraryPath(void)
{
    const char* envPath = getenv("HANDLER_LIBRARY_PATH");
    if (envPath)
        return envPath;
    return std::string(BUILD_PATH);
}

#endif // __UTILS_HPP
