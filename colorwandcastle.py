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

class BoundSprite(Sprite):

    def __init__(self, img, bounds, *args, **kwargs):
        super().__init__(img, *args, **kwargs)
        self._bounds = bounds

    @property
    def u(self):
        return self.y + self._bounds.u

    @u.setter
    def u(self, value):
        self.y = value - self._bounds.u

    @property
    def d(self):
        return self.y - self._bounds.d

    @d.setter
    def d(self, value):
        self.y = value + self._bounds.d

    @property
    def l(self):
        return self.x - self._bounds.l

    @l.setter
    def l(self, value):
        self.x = value + self._bounds.l

    @property
    def r(self):
        return self.x + self._bounds.r

    @r.setter
    def r(self, value):
        self.x = value - self._bounds.r

class Player(BoundSprite):
    pass

class Room:
    pass

class Block(Sprite):
    pass

#class Bounds:
#    def __init__(self, u=0, l=0, d=0, r=0):
#        self.u, self.l, self.d, self.r = u, l, d, r

#class Block(Sprite):
#
#    size = 20
#
#    colors = ('red', 'orange', 'yellow', 'green', 'blue', 'purple')
#
#    imgs = dict()
#    for color in colors:
#        imgs[color] = pyglet.resource.image('block-{}.png'.format(color))
#
#    def __init__(self, color, passable=False, *args, **kwargs):
#        super().__init__(imgs[color], batch=batch, group=fg_group, *args, **kwargs)
#        self.color = color
#        self.passable = passable
#
#class BoundSprite(Sprite):
#
#    def __init__(self, img, bounds, *args, **kwargs):
#        super().__init__(img, *args, **kwargs)
#        self._bounds = bounds
#
#    @property
#    def u(self):
#        return self.y + self._bounds.u
#
#    @u.setter
#    def u(self, value):
#        self.y = value - self._bounds.u
#
#    @property
#    def d(self):
#        return self.y - self._bounds.d
#
#    @d.setter
#    def d(self, value):
#        self.y = value + self._bounds.d
#
#    @property
#    def l(self):
#        return self.x - self._bounds.l
#
#    @l.setter
#    def l(self, value):
#        self.x = value + self._bounds.l
#
#    @property
#    def r(self):
#        return self.x + self._bounds.r
#
#    @r.setter
#    def r(self, value):
#        self.x = value - self._bounds.r
#
#class Room:
#
#    ROWS = HEIGHT // Block.size
#    COLS = WIDTH // Block.size
#
#    def __init__(self):
#        self.blocks = dict()
#        self.generate_blocks(num_colors=3, num_cols=2)
#
#    def generate_blocks(self, num_colors, num_cols):
#        def init_blocks(c, r):
#            if 
#            if r > 0 and r < Room.ROWS - 1 and c > Room.COLS - num_cols - 2 and c < Room.COLS - 1:
#                img = Block.imgs[random.choice(Block.colors[num_colors:])]
#                return Block(img, x=c*Block.size, y=r*Block.size)
#            else:
#                return None
#        self.blocks = [[init_blocks(c, r) for c in range(Room.COLS)] for r in range(Room.ROWS)]
#
#class Player:
#
#    speed = 300
#
#    def __init__(self):
#        self.forward = True # True for facing right, false to face left
#        self._init_anims()
#        self.sprite = BoundSprite(self.anims['stand_r'], Bounds(60, 30, 60, 30),
#            batch=batch, group=fg_group)
#        self.keys = key.KeyStateHandler()
#        self.to_flying()
#
#    def _init_anims(self):
#        self.anims = dict()
#        self.anims['fly'] = Animation((
#            AnimationFrame(load_img('makayla-01.png'), 1/4),
#            AnimationFrame(load_img('makayla-02.png'), 1/4)))
#
#    def to_flying(self):
#        self.update = self._update_flying
#        self.sprite.image = self.anims['fly']
#
#    def _update_flying(self, dt):
#        self._apply_movement_keys(dt)
#
#    def _apply_movement_keys(self, dt):
#        if self.keys[key.UP]:
#            self.sprite.y += Player.speed * dt
#        if self.keys[key.DOWN]:
#            self.sprite.y -= Player.speed * dt
#        if self.keys[key.LEFT]:
#            self.sprite.x -= Player.speed * dt
#        if self.keys[key.RIGHT]:
#            self.sprite.x += Player.speed * dt

# Create the window
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Colorwand Castle')

#player = Player()
#player.sprite.x = WIDTH // 4
#player.sprite.y = HEIGHT // 2

#room = Room()

# Allow the player to receive keyboard input
#window.push_handlers(player.keys)

fps_display = pyglet.clock.ClockDisplay()

# Create the backgrounds
#background_img = pyglet.resource.image('background.png')
#background = Sprite(background_img, batch=batch, group=bg_group)
#walls_img = pyglet.resource.image('walls.png')
#walls = Sprite(walls_img, batch=batch, group=fg_group)

@window.event
def on_draw():
    #batch.draw() # Draw all sprites
    fps_display.draw() # Should be able to be toggled

def update(dt):
    #player.update(dt)
    pass

# Update the game once every frame
pyglet.clock.schedule_interval(update, 1 / FPS)

# Start the game!
pyglet.app.run()

