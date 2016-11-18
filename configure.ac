# Run "autoreconf" to create the "configure" script.

# See here for an explanation of a "configure.ac" file:
#
# https://www.gnu.org/software/automake/manual/html_node/amhello_0027s-configure_002eac-Setup-Explained.html
#
# And here for the entire manual:
#
# https://www.gnu.org/software/autoconf/manual/autoconf.html

# Initialize autoconf with the name of the program,
# the version number, and the contact information.
#
# https://www.gnu.org/software/autoconf/manual/autoconf.html#Initializing-configure
AC_INIT([colorwandcastle], [0.1], [drcouzelis@gmail.com])

# Initialize automake with all warnings.
#
# https://www.gnu.org/software/automake/manual/html_node/Public-Macros.html
#
# "foreign" will ignore any missing files (README, AUTHORS...).
#
# https://www.gnu.org/software/automake/manual/html_node/Strictness.html
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for a compiler.
#
# https://www.gnu.org/software/autoconf/manual/autoconf.html#C-Compiler
AC_PROG_CC

# Check for libraries.
#
# https://www.gnu.org/software/autoconf/manual/autoconf.html#Libraries
AC_CHECK_LIB([allegro], [al_install_system], [],
             [AC_MSG_ERROR([liballegro not found.])])

AC_CHECK_LIB([allegro_acodec], [al_init_acodec_addon], [],
             [AC_MSG_ERROR([liballegro_acodec not found.])])

AC_CHECK_LIB([allegro_audio], [al_install_audio], [],
             [AC_MSG_ERROR([liballegro_audio not found.])])

AC_CHECK_LIB([allegro_image], [al_init_image_addon], [],
             [AC_MSG_ERROR([liballegro_image not found.])])

# Rebuild these header files.
#
# This header file can be used in the project. It contains
# useful strings provided by Autotools.
#
# https://www.gnu.org/software/autoconf/manual/autoconf.html#Configuration-Headers
AC_CONFIG_HEADERS([config.h])

# A list of the Makefiles that need to be created.
#
# https://www.gnu.org/software/autoconf/manual/autoconf.html#Configuration-Files
AC_CONFIG_FILES([Makefile
                 data/Makefile
                 data/images/Makefile
                 data/sounds/Makefile
                 data/levels/Makefile
                 src/Makefile
                 system/Makefile])

# Finish up by writing the new configuration files.
#
# https://www.gnu.org/software/autoconf/manual/autoconf.html#Output
AC_OUTPUT
