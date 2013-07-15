#!/usr/bin/env python3

import pyglet
from pyglet.gl import *
from pyglet.image import Animation, AnimationFrame
from pyglet.sprite import Sprite
from pyglet.window import key
import random

WIDTH = 320
HEIGHT = 240
FPS = 120.0
BLOCK_SIZE = 20
COLORS = ('red', 'orange', 'yellow', 'green', 'blue', 'purple')

# Define resource directories
pyglet.resource.path = ['res']
pyglet.resource.reindex()

# Put all sprites into a single batch to speed up drawing
batch = pyglet.graphics.Batch()

# Groups allow sprite to be drawn in the correct layer
bg_group = pyglet.graphics.OrderedGroup(0)
fg_group = pyglet.graphics.OrderedGroup(1)
act_group = pyglet.graphics.OrderedGroup(2)
hero_group = pyglet.graphics.OrderedGroup(3)

def load_img(filename):
    '''Load the image named "filename" with the anchor centered.'''
    img = pyglet.resource.image(filename)
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2
    return img

def to_tile(pos):
    return pos // BLOCK_SIZE

def to_pos(tile):
    return tile * BLOCK_SIZE

class Actor (Sprite):
    '''An actor is a sprite with a body.'''

    def __init__(self, anim, radius, *args, **kwargs):
        Sprite.__init__(self, anim, *args, **kwargs)
        self.radius = radius # The hit radius

class Star (Actor):

    speed = 200

    imgs = dict()
    for color in COLORS:
        imgs[color] = load_img('star-{}.png'.format(color))
    imgs_flip = dict()
    for color in COLORS:
        imgs_flip[color] = imgs[color].get_transform(flip_y=True)

    def __init__(self, color, *args, **kwargs):
        if color not in COLORS:
            raise
        anim = Animation((
            AnimationFrame(Star.imgs[color], 1/4),
            AnimationFrame(Star.imgs_flip[color], 1/4)))
        Actor.__init__(self, anim, 5, batch=batch, group=act_group, *args, **kwargs)
        self.color = color

class Block (Sprite):

    imgs = dict()
    for color in COLORS:
        imgs[color] = pyglet.resource.image('block-{}.png'.format(color))

    def __init__(self, color, x, y, *args, **kwargs):
        if color not in COLORS:
            raise
        Sprite.__init__(self, Block.imgs[color], x=x, y=y, batch=batch, group=fg_group)
        self.color = color

def inc_color(colors, color):
    if color not in colors.keys():
        colors[color] = 1
    else:
        colors[color] += 1

def dec_color(colors, color):
    if color in colors.keys():
        colors[color] -= 1
        if colors[color] <= 0:
            del colors[color]

class Room:

    cols = WIDTH // BLOCK_SIZE
    rows = HEIGHT // BLOCK_SIZE

    def __init__(self, columns, colors):
        self.bg = self.create_bg()
        self.walls = self.create_walls()
        self.blocks = self.create_blocks(columns, colors)
        self.enemies = list()
        self.colors = colors
        self.columns = columns
        self.front_colors = self.create_front_colors_list()

    def create_front_colors_list(self):
        colors = dict()
        for coord in self.blocks.keys():
            if coord[1] == Room.cols - self.columns - 1:
                color = self.blocks[coord].color
                inc_color(colors, color)
        return colors

    def create_bg(self):
        bg = list()
        for c in range(Room.cols):
            for r in range(Room.rows):
                bg.append(Sprite(pyglet.resource.image('background.png'),
                    x=c * BLOCK_SIZE, y=r * BLOCK_SIZE, batch=batch, group=bg_group))
        return bg

    def create_walls(self):
        walls = dict()
        for c in range(Room.cols):
            for r in range(Room.rows):
                if c == 0 or c == Room.cols - 1 or r == 0 or r == Room.rows - 1:
                    sprite = Sprite(pyglet.resource.image('bricks.png'),
                        x=c * BLOCK_SIZE, y=r * BLOCK_SIZE, batch=batch, group=fg_group)
                    walls[(r, c)] = sprite
        return walls

    def create_blocks(self, columns, colors):
        blocks = dict()
            # Access with a (r, c) tuple
        for c in range(Room.cols - columns - 1, Room.cols - 1):
            for r in range(1, Room.rows - 1):
                color = random.choice(COLORS[:colors])
                block = Block(color, c * BLOCK_SIZE, r * BLOCK_SIZE)
                blocks[(r, c)] = block
        return blocks

scale = 3

# Create the window
window = pyglet.window.Window(width=WIDTH * scale, height=HEIGHT * scale, caption='Colorwand Castle')

# These arguments are x, y and z respectively
# This scales your window
glScalef(scale, scale, scale)

# Create the room
room = Room(columns=4, colors=6)

# Create the hero
#hero = Hero(x=WIDTH // 4, y=HEIGHT // 2, room=room)

# Allow the hero to receive keyboard input
#window.push_handlers(hero.keys)

#hero.control = KeyboardHeroControl(hero, room)

#fps_display = pyglet.clock.ClockDisplay()

star = Star('red', x=WIDTH // 2, y=HEIGHT // 2)

@window.event
def on_draw():
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    window.clear()
    batch.draw() # Draw all sprites
    #fps_display.draw() # Should be able to be toggled

def update(dt):
    #hero.update(dt)
    pass

# Update the game once every frame
pyglet.clock.schedule_interval(update, 1 / FPS)

# Start the game!
pyglet.app.run()

