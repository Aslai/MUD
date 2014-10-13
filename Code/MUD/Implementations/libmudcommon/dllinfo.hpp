#ifndef MUD_DLLINFO_HPP
#define MUD_DLLINFO_HPP

#ifdef _WIN32
    #ifdef DO_DLL_EXPORTS
        #define DLL_EXPORT __declspec(dllexport)
    #else
        #define DLL_EXPORT __declspec(dllimport)
    #endif
    #define DLL_HIDE
#else

    #define DLL_EXPORT
    #define DLL_HIDE
#endif

#endif
