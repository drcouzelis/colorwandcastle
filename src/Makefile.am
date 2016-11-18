# The name of the program to be compiled.
# The "bin" prefix will install it in the "bin" directory.
# The "PROGRAMS" suffix marks it as a program to be compiled.
#
# https://www.gnu.org/software/automake/manual/html_node/Program-Sources.html#Program-Sources
bin_PROGRAMS = colorwandcastle

# A list of all of the source code files.
# The "SOURCES" suffix marks it as source code.
#
# https://www.gnu.org/software/automake/manual/html_node/Program-Sources.html#Program-Sources
colorwandcastle_SOURCES = \
  datafile.c \
  datafile.h \
  display.c \
  display.h \
  gamedata.c \
  gamedata.h \
  gameplay.c \
  gameplay.h \
  main.c \
  main.h \
  mask.c \
  mask.h \
  memory.c \
  memory.h \
  menu.c \
  menu.h \
  resources.c \
  resources.h \
  run.c \
  run.h \
  sound.c \
  sound.h \
  sprite.c \
  sprite.h \
  utilities.c \
  utilities.h

# The "-D" option provides the compiler with a variable
# that points to the location of the data files.
#
# NOTE: "CPP" stands for "C Pre-Processor".
#
# https://www.gnu.org/software/automake/manual/html_node/Program-and-Library-Variables.html
colorwandcastle_CPPFLAGS = -Wall -Werror -DPKGDATADIR='"$(pkgdatadir)"'
