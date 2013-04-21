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

# Define resource directories
pyglet.resource.path = ['res']
pyglet.resource.reindex()

# Put all sprites into a single batch to speed up drawing
batch = pyglet.graphics.Batch()

# Groups allow sprite to be drawn in the correct layer
bg_group = pyglet.graphics.OrderedGroup(0)
fg_group = pyglet.graphics.OrderedGroup(1)
act_group = pyglet.graphics.OrderedGroup(2)

def load_img(filename):
    '''Load the image named "filename" with the anchor centered.'''
    img = pyglet.resource.image(filename)
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2
    return img

class Bounds:

    def __init__(self, u=0, l=0, d=0, r=0):
        self.u, self.l, self.d, self.r = u, l, d, r

class Actor:
    '''An actor is a sprite with a body. The default bounds are set
       to the dimensions of the image.'''

    def __init__(self, sprite, bounds):
        self.sprite = sprite
            # A visual representation of the actor
        self.bounds = bounds
            # The collision area boundary

    @property
    def u(self):
        return self.sprite.y + self.bounds.u

    @u.setter
    def u(self, value):
        self.sprite.y = value - self.bounds.u

    @property
    def d(self):
        return self.sprite.y - self.bounds.d

    @d.setter
    def d(self, value):
        self.sprite.y = value + self.bounds.d

    @property
    def l(self):
        return self.sprite.x - self.bounds.l

    @l.setter
    def l(self, value):
        self.sprite.x = value + self.bounds.l

    @property
    def r(self):
        return self.sprite.x + self.bounds.r

    @r.setter
    def r(self, value):
        self.sprite.x = value - self.bounds.r

class Player:

    speed = 80

    def __init__(self):
        anim = Animation((
            AnimationFrame(load_img('makayla-01.png'), 1/8),
            AnimationFrame(load_img('makayla-02.png'), 1/8)))
        sprite = Sprite(anim, batch=batch, group=act_group)
        bounds = Bounds(u=5, l=10, d=15, r=10)
        self.actor = Actor(sprite, bounds)
        self.controller = None
        self.keys = key.KeyStateHandler()
        img = load_img('star-purple.png')
        anim = Animation((
            AnimationFrame(img, 1/4),
            AnimationFrame(img.get_transform(flip_y=True), 1/4)))
        self.star = Sprite(anim, batch=batch, group=act_group, x=self.x + Block.size, y=self.y)

    @property
    def x(self):
        return self.actor.sprite.x

    @x.setter
    def x(self, value):
        self.actor.sprite.x = value
        self.star.x = value + 25

    @property
    def y(self):
        return self.actor.sprite.y

    @y.setter
    def y(self, value):
        self.actor.sprite.y = value
        self.star.y = ((value // Block.size) * Block.size) + 10

    def update(self, dt):
        if self.controller:
            self.controller.update(dt)

class Block:

    size = 20
        # Square block width and height

    colors = ('red', 'orange', 'yellow', 'green', 'blue', 'purple')

    imgs = dict()
    for color in colors:
        imgs[color] = pyglet.resource.image('block-{}.png'.format(color))

    def __init__(self, color, x, y):
        if color not in Block.colors:
            raise
        self.sprite = Sprite(Block.imgs[color], x=x, y=y, batch=batch, group=fg_group)
        self.color = color

class Room:

    cols = WIDTH // Block.size
    rows = HEIGHT // Block.size

    def __init__(self, columns, colors):
        self._add_bg()
        self._add_walls()
        self._add_blocks(columns, colors)
        self.enemies = list()

    def _add_bg(self):
        self.bg = list()
        for c in range(Room.cols):
            for r in range(Room.rows):
                self.bg.append(Sprite(pyglet.resource.image('background.png'),
                    x=c * Block.size, y=r * Block.size, batch=batch, group=bg_group))

    def _add_walls(self):
        self.walls = list()
        for c in range(Room.cols):
            for r in range(Room.rows):
                if c == 0 or c == Room.cols - 1 or r == 0 or r == Room.rows - 1:
                    self.walls.append(Sprite(pyglet.resource.image('bricks.png'),
                        x=c * Block.size, y=r * Block.size, batch=batch, group=fg_group))

    def _add_blocks(self, columns, colors):
        self.blocks = dict()
            # Access with a (r, c) tuple
        for c in range(Room.cols - columns - 1, Room.cols - 1):
            for r in range(1, Room.rows - 1):
                color = random.choice(Block.colors[:colors])
                block = Block(color, c * Block.size, r * Block.size)
                self.blocks[(r, c)] = block

class KeyboardPlayerController:

    def __init__(self, player):
        self.player = player

    def update(self, dt):
        keys = self.player.keys
        if keys[key.UP]:
            player.y += Player.speed * dt
        if keys[key.DOWN]:
            player.y -= Player.speed * dt
        if keys[key.LEFT]:
            player.x -= Player.speed * dt
        if keys[key.RIGHT]:
            player.x += Player.speed * dt
        if keys[key.SPACE]:
            print('Pretending to shoot a star!')

# Create the window
window = pyglet.window.Window(width=WIDTH * 4, height=HEIGHT * 4, caption='Colorwand Castle')

# These arguments are x, y and z respectively
# This scales your window
glScalef(4.0, 4.0, 4.0)

player = Player()
player.x = WIDTH // 4
player.y = HEIGHT // 2
player.controller = KeyboardPlayerController(player)

# Allow the player to receive keyboard input
window.push_handlers(player.keys)

room = Room(columns=3, colors=4)

#fps_display = pyglet.clock.ClockDisplay()

@window.event
def on_draw():
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    batch.draw() # Draw all sprites
    #fps_display.draw() # Should be able to be toggled

def update(dt):
    player.update(dt)

# Update the game once every frame
pyglet.clock.schedule_interval(update, 1 / FPS)

# Start the game!
pyglet.app.run()

