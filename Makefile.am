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
  src/drc_collision.c \
  src/drc_collision.h \
  src/drc_display.c \
  src/drc_display.h \
  src/drc_memory.c \
  src/drc_memory.h \
  src/drc_random.c \
  src/drc_random.h \
  src/drc_resources.c \
  src/drc_resources.h \
  src/drc_run.c \
  src/drc_run.h \
  src/drc_sound.c \
  src/drc_sound.h \
  src/drc_sprite.c \
  src/drc_sprite.h \
  src/drc_text.c \
  src/drc_text.h \
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
  data/images/boss-spider.png \
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
  data/images/menu-titlescreen.png \
  data/images/powerup-frame-1.png \
  data/images/powerup-frame-2.png \
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
  data/images/texture-strobe-square.png \
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
  data/levels/room-story-013.dat \
  data/levels/room-story-014.dat \
  data/levels/room-story-015.dat \
  data/levels/room-story-016.dat \
  data/levels/room-story-017.dat \
  data/levels/room-story-018-boss.dat \
  data/levels/room-story-019.dat \
  data/levels/room-makayla.dat \
  data/levels/room-rawson.dat \
  data/levels/room-rawson-2.dat \
  data/levels/room-hard.dat \
  data/levels/room-test.dat \
  data/levels/texture-boss-spider.dat \
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
