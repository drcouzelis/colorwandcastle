# Sources
bin_PROGRAMS = colorwandcastle
colorwandcastle_CPPFLAGS = -Wall -Werror -DPKGDATADIR='"$(pkgdatadir)"'
colorwandcastle_SOURCES = \
  src/datafile.c \
  src/datafile.h \
  src/direction.c \
  src/direction.h \
  src/display.c \
  src/display.h \
  src/gamedata.c \
  src/gamedata.h \
  src/gameplay.c \
  src/gameplay.h \
  src/main.c \
  src/main.h \
  src/mask.c \
  src/mask.h \
  src/memory.c \
  src/memory.h \
  src/menu.c \
  src/menu.h \
  src/path.c \
  src/path.h \
  src/resources.c \
  src/resources.h \
  src/run.c \
  src/run.h \
  src/sound.c \
  src/sound.h \
  src/sprite.c \
  src/sprite.h \
  src/utilities.c \
  src/utilities.h

# Data - Images
imagesdatadir = $(pkgdatadir)/images
dist_imagesdata_DATA = \
  data/images/effect-poof-1.png \
  data/images/effect-poof-2.png \
  data/images/effect-poof-3.png \
  data/images/effect-poof-4.png \
  data/images/enemy-bat-1.png \
  data/images/enemy-bat-2.png \
  data/images/enemy-bat-3.png \
  data/images/enemy-spider-1.png \
  data/images/enemy-spider-2.png \
  data/images/enemy-spider-3.png \
  data/images/enemy-spider-4.png \
  data/images/enemy-spider-5.png \
  data/images/enemy-spider-6.png \
  data/images/enemy-spider-7.png \
  data/images/hero-makayla-01.png \
  data/images/hero-makayla-02.png \
  data/images/hero-makayla-hurt-01.png \
  data/images/hero-makayla-hurt-02.png \
  data/images/hero-rawson-01.png \
  data/images/hero-rawson-02.png \
  data/images/hero-rawson-hurt-01.png \
  data/images/hero-rawson-hurt-02.png \
  data/images/icon.png \
  data/images/mask-block.png \
  data/images/mask-plasma-1.png \
  data/images/mask-plasma-2.png \
  data/images/mask-star-1.png \
  data/images/mask-star-2.png \
  data/images/texture-blue.png \
  data/images/texture-green.png \
  data/images/texture-orange.png \
  data/images/texture-purple.png \
  data/images/texture-red.png \
  data/images/texture-yellow.png \
  data/images/tile-bricks.png \
  data/images/tile-door.png \
  data/images/tile-gray-wall.png

# Data - Levels
levelsdatadir = $(pkgdatadir)/levels
dist_levelsdata_DATA = \
  data/levels/list-story.dat \
  data/levels/room-01.dat \
  data/levels/room-02.dat \
  data/levels/room-03-hard.dat \
  data/levels/room-story-01.dat \
  data/levels/room-story-01-down.dat \
  data/levels/room-story-01-left.dat \
  data/levels/room-story-01-up.dat \
  data/levels/room-story-02.dat \
  data/levels/textures-colored.dat \
  data/levels/tiles-castle.dat

# Data - Sounds
soundsdatadir = $(pkgdatadir)/sounds
dist_soundsdata_DATA = \
  data/sounds/star_disolve.wav \
  data/sounds/star_hit.wav

# Distribution files
EXTRA_DIST = \
  system/colorwandcastle.desktop \
  system/colorwandcastle.png
