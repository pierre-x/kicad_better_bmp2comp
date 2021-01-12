// Do not modify this file, it was automatically generated by CMake.

#ifndef CONFIG_H_
#define CONFIG_H_

#cmakedefine HAVE_STRCASECMP

#cmakedefine HAVE_STRNCASECMP

#cmakedefine HAVE_STRTOKR       // spelled odly to differ from wx's similar test

// Handle platform differences in math.h
#cmakedefine HAVE_MATH_H

// Handle platform differences in C++ cmath.
#cmakedefine HAVE_CXX_CMATH

#cmakedefine HAVE_CMATH_ASINH

#cmakedefine HAVE_CMATH_ACOSH

#cmakedefine HAVE_CMATH_ATANH

#cmakedefine HAVE_CMATH_ISINF

#if !defined( HAVE_CLOCK_GETTIME )
#cmakedefine HAVE_CLOCK_GETTIME
#endif

#cmakedefine HAVE_GETTIMEOFDAY_FUNC

#cmakedefine MALLOC_IN_STDLIB_H

#if !defined( MALLOC_IN_STDLIB_H )
#include <malloc.h>
#endif

#cmakedefine HAVE_ISO646_H

#if defined( HAVE_ISO646_H )
#include <iso646.h>
#endif

#if !defined( HAVE_STRCASECMP )
#define strcasecmp stricmp
#endif

#if !defined( HAVE_STRNCASECMP )
#define strncasecmp strnicmp
#endif

// Does the compiler support the -Wimplicit-int-float-conversion warning
#cmakedefine HAVE_WIMPLICIT_FLOAT_CONVERSION

// Use Posix getc_unlocked() instead of getc() when it's available.
#cmakedefine HAVE_FGETC_NOLOCK

// Warning!!!  Using wxGraphicContext for rendering is experimental.
#cmakedefine USE_WX_GRAPHICS_CONTEXT    1

/// The legacy file format revision of the *.brd file created by this build
#define LEGACY_BOARD_FILE_VERSION       2

/// The install prefix defined during CMake configuration or fall back to CMAKE_INSTALL_PREFIX.
#define DEFAULT_INSTALL_PATH            "@DEFAULT_INSTALL_PATH@"

/// The wxPython version found during configuration.
#if defined( KICAD_SCRIPTING_WXPYTHON )
#define WXPYTHON_VERSION                "@WXPYTHON_VERSION@"
#endif

/// A file extension with a leading '.' is a suffix, and this one is used on
/// top level program modules which implement the KIFACE.
#define KIFACE_SUFFIX                   "@KIFACE_SUFFIX@"
#define KIFACE_PREFIX                   "@KIFACE_PREFIX@"

/// Allows scripts install directory to be referenced by the program code.
#define PYTHON_DEST                     "@PYTHON_DEST@"

/// Allows scripts install directory to be referenced by the program code.
#define KICAD_DATA                     "@KICAD_DATA@"

/// ngspice version string detected by pkg-config when available.
#cmakedefine NGSPICE_BUILD_VERSION           "@NGSPICE_BUILD_VERSION@"

/// When pkg-config config is not available for ngspice, use ngspice/config.h for version.
#cmakedefine NGSPICE_HAVE_CONFIG_H

#endif  // CONFIG_H_
