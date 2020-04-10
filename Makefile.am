# Sources
bin_PROGRAMS = colorwandcastle

# -std=c11 - C version to use
# -O2 - Optimization level, 2 is common, 3 is more extreme
# -Os - Cache efficiency
# -Wall - Report all warnings
# -Wextra - Reprort extra warnings
# -Wpedantic - Report all stupid little warnings
# -march=native - Compile using the full feature set of the CPU
colorwandcastle_CPPFLAGS = -std=c11 -O2 -Os -Wall -Wextra -Wpedantic -march=native -DPKGDATADIR='"$(pkgdatadir)"'

colorwandcastle_SOURCES = \
  src/compiler.h \
  src/datafile.c \
  src/datafile.h \
  src/dgl_collision.c \
  src/dgl_collision.h \
  src/dgl_display.c \
  src/dgl_display.h \
  src/dgl_memory.c \
  src/dgl_memory.h \
  src/dgl_random.c \
  src/dgl_random.h \
  src/dgl_resources.c \
  src/dgl_resources.h \
  src/dgl_run.c \
  src/dgl_run.h \
  src/dgl_sound.c \
  src/dgl_sound.h \
  src/dgl_sprite.c \
  src/dgl_sprite.h \
  src/direction.c \
  src/direction.h \
  src/effects.c \
  src/effects.h \
  src/gamedata.c \
  src/gamedata.h \
  src/gameplay.c \
  src/gameplay.h \
  src/main.c \
  src/main.h \
  src/mask.c \
  src/mask.h \
  src/menu.c \
  src/menu.h \
  src/path.c \
  src/path.h \
  src/roomlist.c \
  src/roomlist.h

# Data - Images
imagesdatadir = $(pkgdatadir)/images
dist_imagesdata_DATA = \
  data/images/background-bedroom.png \
  data/images/effect-poof-1.png \
  data/images/effect-poof-2.png \
  data/images/effect-poof-3.png \
  data/images/effect-poof-4.png \
  data/images/enemy-bat-1.png \
  data/images/enemy-bat-2.png \
  data/images/enemy-bat-3.png \
  data/images/enemy-blocker-1.png \
  data/images/enemy-blocker-2.png \
  data/images/enemy-ghost-1.png \
  data/images/enemy-ghost-2.png \
  data/images/enemy-ghost-3.png \
  data/images/enemy-ghost-4.png \
  data/images/enemy-spider-1.png \
  data/images/enemy-spider-2.png \
  data/images/enemy-spider-3.png \
  data/images/enemy-spider-4.png \
  data/images/enemy-spider-5.png \
  data/images/enemy-spider-6.png \
  data/images/enemy-spider-7.png \
  data/images/enemy-tracer-1.png \
  data/images/enemy-tracer-2.png \
  data/images/enemy-tracer-3.png \
  data/images/enemy-tracer-4.png \
  data/images/enemy-tracer-5.png \
  data/images/enemy-tracer-6.png \
  data/images/enemy-tracer-7.png \
  data/images/enemy-tracer-8.png \
  data/images/farground-forest.png \
  data/images/hero-makayla-1.png \
  data/images/hero-makayla-2.png \
  data/images/hero-makayla-hurt-1.png \
  data/images/hero-makayla-hurt-2.png \
  data/images/hero-rawson-1.png \
  data/images/hero-rawson-2.png \
  data/images/hero-rawson-hurt-1.png \
  data/images/hero-rawson-hurt-2.png \
  data/images/icon.png \
  data/images/mask-block.png \
  data/images/mask-plasma-1.png \
  data/images/mask-plasma-2.png \
  data/images/mask-star-1.png \
  data/images/mask-star-2.png \
  data/images/powerup-laser-1.png \
  data/images/powerup-laser-2.png \
  data/images/powerup-rainbow-1.png \
  data/images/powerup-rainbow-2.png \
  data/images/powerup-speed-1.png \
  data/images/powerup-speed-2.png \
  data/images/texture-blue.png \
  data/images/texture-green.png \
  data/images/texture-orange.png \
  data/images/texture-purple.png \
  data/images/texture-red.png \
  data/images/texture-yellow.png \
  data/images/tile-bricks.png \
  data/images/tile-castle.png \
  data/images/tile-door.png \
  data/images/tile-general.png \
  data/images/tile-wood.png

# Data - Levels
levelsdatadir = $(pkgdatadir)/levels
dist_levelsdata_DATA = \
  data/levels/list-demo.dat \
  data/levels/room-demo-001.dat \
  data/levels/room-demo-002.dat \
  data/levels/room-demo-003.dat \
  data/levels/room-demo-004.dat \
  data/levels/room-demo-005.dat \
  data/levels/room-demo-006.dat \
  data/levels/room-demo-007.dat \
  data/levels/room-demo-008.dat \
  data/levels/room-demo-009.dat \
  data/levels/room-demo-010.dat \
  data/levels/room-demo-011.dat \
  data/levels/list-story.dat \
  data/levels/room-story-001.dat \
  data/levels/room-story-002.dat \
  data/levels/room-story-003.dat \
  data/levels/room-story-004.dat \
  data/levels/room-story-005.dat \
  data/levels/room-story-006.dat \
  data/levels/room-story-007.dat \
  data/levels/room-story-008.dat \
  data/levels/room-story-009.dat \
  data/levels/room-story-010.dat \
  data/levels/room-story-011.dat \
  data/levels/room-story-012.dat \
  data/levels/room-makayla.dat \
  data/levels/room-rawson.dat \
  data/levels/room-rawson-2.dat \
  data/levels/room-hard.dat \
  data/levels/room-test.dat \
  data/levels/texture-colored.dat \
  data/levels/texture-colored-2.dat \
  data/levels/texture-colored-3.dat \
  data/levels/tile-bricks.dat \
  data/levels/tile-castle.dat \
  data/levels/tile-general.dat \
  data/levels/tile-wood.dat

# Data - Sounds
soundsdatadir = $(pkgdatadir)/sounds
dist_soundsdata_DATA = \
  data/sounds/block-destroyed.wav \
  data/sounds/bullet-bounce.wav \
  data/sounds/bullet-disolve.wav \
  data/sounds/bullet-shoot.wav \
  data/sounds/hero-die.wav \
  data/sounds/hero-toggle.wav \
  data/sounds/room-cleared.wav

# Distribution files
EXTRA_DIST = \
  system/colorwandcastle.desktop \
  system/colorwandcastle.png
