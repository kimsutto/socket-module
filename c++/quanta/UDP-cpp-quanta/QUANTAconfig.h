/* include/QUANTA/QUANTAconfig.hxx.  Generated automatically by configure.  */
/* include/QUANTA/QUANTAconfig.hxx.in.  Generated automatically from configure.in by autoheader.  */

/* Define to 1 if configured with 32 bit binary type. */
#define HAVE_BINARY_TYPE_32 1

/* Define to 1 if configured with 64 bit binary type. */
/* #undef HAVE_BINARY_TYPE_64 */

/* Define to 1 if configured with n32 bit binary type. */
/* #undef HAVE_BINARY_TYPE_N32 */

/* Define if you have the <cygwin/socket.h> header file. */
/* #undef HAVE_CYGWIN_SOCKET_H */

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have the <fts.h> header file. */
/* #define HAVE_FTS_H 1 */

/* Define if you have the <ftw.h> header file. */
/* #undef HAVE_FTW_H */

/* Define if you have the <inttypes.h> header file. */
/* #undef HAVE_INTTYPES_H */

/* Define if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define if you have the `pthread' library (-lpthread). */
#define HAVE_LIBPTHREAD 1

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <math.h> header file. */
#define HAVE_MATH_H 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 0

/* Define if you have the `sleep' function. */
/* #define HAVE_SLEEP 1 */

/* Define to 1 if this system provides socklen_t. */
/* #undef HAVE_SOCKLEN_T */
#ifdef WIN32
#undef HAVE_SOCKLEN_T
#else
#define HAVE_SOCKLEN_T 1
#endif

/* Define if you have the <stdint.h> header file. */
/* #undef HAVE_STDINT_H */

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1


/* Define if you have the <unistd.h> header file. */
#ifndef WIN32
#define HAVE_UNISTD_H 1
#endif

/* Define if you have the `usleep' function. */
/* #define HAVE_USLEEP 1 */

/* Name of package */
#define PACKAGE "QUANTA"

/* Define to 1 if this system uses little endian binary format. */
/* #undef QUANTA_LITTLE_ENDIAN_FORMAT */

/* Define to 1 to compile QUANTA with thread safety. */
#define QUANTA_THREAD_SAFE 1

/* Define to 1 if compiling QUANTA with pthreads. */
#ifndef WIN32
#define QUANTA_USE_PTHREADS 1
#endif

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.0"

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
/* #define WORDS_BIGENDIAN 1 */
#define QUANTA_LITTLE_ENDIAN_FORMAT


#define QUANTA_USE_WINDOWSTHREADS 0

#define QUANTA_STD_NAMESPACE	1

#define QUANTA_ANSI_IOSTREAMS	1
