#!/usr/bin/env python3

import pyglet
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

class Controller:

    def __init__(self, puppet):
        self.puppet = puppet
        # Use an update method to control the puppet

class Player:

    speed = 300

    def __init__(self):
        anim = Animation((
            AnimationFrame(load_img('makayla-01.png'), 1/4),
            AnimationFrame(load_img('makayla-02.png'), 1/4)))
        sprite = Sprite(anim, batch=batch, group=fg_group)
        bounds = Bounds(u=5, l=10, d=15, r=10)
        self.actor = Actor(sprite, bounds)
        self.controller = None
        self.keys = key.KeyStateHandler()

    @property
    def x(self):
        return self.actor.sprite.x

    @x.setter
    def x(self, value):
        self.actor.sprite.x = value

    @property
    def y(self):
        return self.actor.sprite.y

    @y.setter
    def y(self, value):
        self.actor.sprite.y = value

    def update(self, dt):
        if self.controller:
            self.controller.update(dt)

class Room:

    def __init__(self, columns, colors):
        self.blocks = dict()
            # Access with a (r, c) tuple
        self.enemies = list()

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
        self.sprite = Sprite(imgs[color], x=x, y=y, batch=batch, group=fg_group)
        self.color = color

class KeyboardPlayerController:

    def __init__(self, player):
        self.player = player

    def update(self, dt):
        sprite = self.player.sprite
        keys = self.player.keys
        if keys[key.UP]:
            sprite.y += Player.speed * dt
        if keys[key.DOWN]:
            sprite.y -= Player.speed * dt
        if keys[key.LEFT]:
            sprite.x -= Player.speed * dt
        if keys[key.RIGHT]:
            sprite.x += Player.speed * dt

# Create the window
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Colorwand Castle')

player = Player()
player.x = WIDTH // 4
player.y = HEIGHT // 2
player.controller = KeyboardPlayerController(player)

# Allow the player to receive keyboard input
window.push_handlers(player.keys)

room = Room(columns=3, colors=4)

fps_display = pyglet.clock.ClockDisplay()

# Create the backgrounds
#background_img = pyglet.resource.image('background.png')
#background = Sprite(background_img, batch=batch, group=bg_group)
#walls_img = pyglet.resource.image('walls.png')
#walls = Sprite(walls_img, batch=batch, group=fg_group)

@window.event
def on_draw():
    batch.draw() # Draw all sprites
    fps_display.draw() # Should be able to be toggled

def update(dt):
    player.update(dt)

# Update the game once every frame
pyglet.clock.schedule_interval(update, 1 / FPS)

# Start the game!
pyglet.app.run()

