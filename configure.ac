# Run "autoreconf" to create the "configure" script.

# Initialize autoconf with the name of the program,
# the version number, and the contact information.
AC_INIT([colorwandcastle], [0.1], [drcouzelis@gmail.com])

# Initialize automake with all warnings.
AM_INIT_AUTOMAKE([-Wall -Werror])

# Mark the source code directory.
AC_CONFIG_SRCDIR([src/main.h])

# Rebuild these header files.
AC_CONFIG_HEADERS([config.h])

# Check for the compiler.
AC_PROG_CXX
AM_PROG_CC_C_O

# Check for libraries.
AC_CHECK_LIB([allegro], [al_install_system], [],
             [AC_MSG_ERROR([liballegro not found.])])

AC_CHECK_LIB([allegro_acodec], [al_init_acodec_addon], [],
             [AC_MSG_ERROR([liballegro_acodec not found.])])

AC_CHECK_LIB([allegro_audio], [al_install_audio], [],
             [AC_MSG_ERROR([liballegro_audio not found.])])

AC_CHECK_LIB([allegro_image], [al_init_image_addon], [],
             [AC_MSG_ERROR([liballegro_image not found.])])

# Check for SDL
AC_CHECK_LIB([SDL2], [SDL_Init], [],
             [AC_MSG_ERROR([libSDL2 not found.])])

#SDL_VERSION=2.0.4
#AM_PATH_SDL2($SDL_VERSION,
#          :,
#          AC_MSG_ERROR([SDL version $SDL_VERSION not found!])
#)
#CFLAGS="$CFLAGS $SDL_CFLAGS"
#LIBS="$LIBS $SDL_LIBS"

# A list of the Makefiles that need to be created.
AC_CONFIG_FILES([Makefile
                 data/Makefile
                 data/images/Makefile
                 data/sounds/Makefile
                 src/Makefile
                 system/Makefile])

# Finish up by writing the new configuration files.
AC_OUTPUT
