#ifndef PCMANX_INTERNAL_UTIL
#define PCMANX_INTERNAL_UTIL

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef USE_DEBUG
#define DEBUG(format...) g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, format)
#define INFO_ON(format...) g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format)
#else
#define DEBUG(format...)
#define INFO_ON(format...)
#endif

#if 0
#define INFO(format...) printf(format);
#else
#define INFO(format...)
#endif

#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)) && defined(__ELF__)
#  define X_EXPORT __attribute__((visibility("default")))
#else
#  define X_EXPORT
#endif

#endif
