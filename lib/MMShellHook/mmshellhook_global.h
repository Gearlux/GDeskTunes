#ifndef MMSHELLHOOK_GLOBAL_H
#define MMSHELLHOOK_GLOBAL_H

#  define Q_DECL_EXPORT __declspec(dllexport)
#  define Q_DECL_IMPORT __declspec(dllimport)

#if defined(MMSHELLHOOK_LIBRARY)
#  define MMSHELLHOOKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MMSHELLHOOKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MMSHELLHOOK_GLOBAL_H
