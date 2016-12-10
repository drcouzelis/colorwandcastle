AC_INIT([colorwandcastle], [0.1], [drcouzelis@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC

AC_CHECK_HEADER([allegro5/allegro.h], [], [AC_MSG_ERROR([allegro.h not found.])])
AC_CHECK_HEADER([allegro5/allegro_acodec.h], [], [AC_MSG_ERROR([allegro_acodec.h not found.])])
AC_CHECK_HEADER([allegro5/allegro_audio.h], [], [AC_MSG_ERROR([allegro_audio.h not found.])])
AC_CHECK_HEADER([allegro5/allegro_image.h], [], [AC_MSG_ERROR([allegro_image.h not found.])])

AC_SEARCH_LIBS([al_install_system], [allegro], [], [AC_MSG_ERROR([allegro not found.])])
AC_SEARCH_LIBS([al_init_acodec_addon], [allegro_acodec], [], [AC_MSG_ERROR([allegro not found.])])
AC_SEARCH_LIBS([al_install_audio], [allegro_audio], [], [AC_MSG_ERROR([allegro not found.])])
AC_SEARCH_LIBS([al_init_image_addon], [allegro_image], [], [AC_MSG_ERROR([allegro not found.])])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
