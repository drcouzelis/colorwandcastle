# The name of the program to be compiled.
# The "bin" prefix will install it in the "bin" directory.
# The "PROGRAMS" suffix marks it as a program to be compiled.
bin_PROGRAMS = colorwandcastle

# A list of all of the source code files.
# The "SOURCES" suffix marks it as source code.
colorwandcastle_SOURCES = \
  display.c \
  display.h \
  gameplay.c \
  gameplay.h \
  main.c \
  main.h \
  memory.c \
  memory.h \
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

# This provides the compiler with a variable that points to the
# location of the data files.
# NOTE: "CPP" stands for "C Pre-Processor".
colorwandcastle_CPPFLAGS = -Wall -Werror -DPKGDATADIR='"$(pkgdatadir)"'
