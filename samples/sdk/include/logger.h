#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>

#define FILENAME                                                          \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_LEVEL_DEBUG   0 /* Detailed point to analyze errors */
#define LOG_LEVEL_INFO    1 /* Info about process */
#define LOG_LEVEL_WARNING 2 /* Expected fail, not critical */
#define LOG_LEVEL_ERROR   3 /* Unexpected fail (recoverable) */

#if !defined(LOG_LEVEL_ENABLED)
#define LOG_LEVEL_ENABLED 1
#endif

#if LOG_LEVEL_ENABLED <= LOG_LEVEL_ERROR
#define LOG_ERR(fmt, ...)                                                     \
    do {                                                                      \
        printf("[%s:%d ERROR] " fmt, FILENAME, __LINE__, ##__VA_ARGS__);  \
        printf("\n");                                                         \
        fflush(stdout);                                                       \
    } while (0)
#else
#define LOG_ERR(fmt, ...)
#endif

#if LOG_LEVEL_ENABLED <= LOG_LEVEL_WARNING
#define LOG_WARN(fmt, ...)                                                    \
    do {                                                                      \
        printf("[%s:%d WARNING] " fmt, FILENAME, __LINE__,                \
               ##__VA_ARGS__);                                                \
        printf("\n");                                                         \
        fflush(stdout);                                                       \
    } while (0)
#else
#define LOG_WARN(fmt, ...)
#endif
#if LOG_LEVEL_ENABLED <= LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...)                                                    \
    do {                                                                      \
        printf("[%s:%d INFO] " fmt, FILENAME, __LINE__, ##__VA_ARGS__);   \
        printf("\n");                                                         \
        fflush(stdout);                                                       \
    } while (0)
#else
#define LOG_INFO(fmt, ...)
#endif
#if LOG_LEVEL_ENABLED <= LOG_LEVEL_DEBUG
#define LOG_DBG(fmt, ...)                                                     \
    do {                                                                      \
        printf("[%s:%d DEBUG] " fmt, FILENAME, __LINE__, ##__VA_ARGS__);  \
        printf("\n");                                                         \
        fflush(stdout);                                                       \
    } while (0)
#else
#define LOG_DBG(fmt, ...)
#endif

#endif
