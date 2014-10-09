#ifndef MUD_DLLINFO_HPP
#define MUD_DLLINFO_HPP

#ifdef DO_DLL_EXPORTS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#endif
