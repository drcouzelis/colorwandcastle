# Run "autoreconf" to create the "configure" script.

# Initialize autoconf with the name of the program,
# the version number, and the contact information.
AC_INIT([colorwandcastle], [0.1], [drcouzelis@gmail.com])

# Initialize automake with all warnings.
AM_INIT_AUTOMAKE([-Wall -Werror])

# Mark the source code directory.
AC_CONFIG_SRCDIR([src/colorwandcastle.h])

# Rebuild these header files.
AC_CONFIG_HEADERS([config.h])

# Check for the compiler.
AC_PROG_CXX
AM_PROG_CC_C_O

# Check for libraries.
AC_CHECK_LIB([alleg], [install_allegro], [],
             [AC_MSG_ERROR([liballeg not found.])])

# A list of the Makefiles that need to be created.
AC_CONFIG_FILES([Makefile
                 data/Makefile
                 data/images/Makefile
                 data/sounds/Makefile
                 src/Makefile
                 system/Makefile])

# Finish up by writing the new configuration files.
AC_OUTPUT
