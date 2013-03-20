#!/usr/bin/env python3

import pyglet
from pyglet.image import Animation, AnimationFrame
from pyglet.sprite import Sprite
from pyglet.window import key

WIDTH = 1280
HEIGHT = 800
FPS = 120.0

# Define resource directories
pyglet.resource.path = ['res']
pyglet.resource.reindex()

# Put all sprites into a single batch to speed up drawing
batch = pyglet.graphics.Batch()

# Groups allow sprite to be drawn in the correct layer
background_group = pyglet.graphics.OrderedGroup(0)
walkground_group = pyglet.graphics.OrderedGroup(1)
hero_group = pyglet.graphics.OrderedGroup(2)

def load_img(filename):
    img = pyglet.resource.image(filename)
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2
    return img

class Bounds:
    def __init__(self, u=0, l=0, d=0, r=0):
        self.u, self.l, self.d, self.r = u, l, d, r

class Block(Sprite):
    size = 80

playfield_bounds = Bounds(HEIGHT - Block.size, Block.size, Block.size, WIDTH - Block.size)

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

class Room:

    def __init__(self):
        self.block_imgs = dict()
        self.block_imgs['red'] = pyglet.resource.image('block-red.png')
        self.block_imgs['orange'] = pyglet.resource.image('block-orange.png')
        self.block_imgs['yellow'] = pyglet.resource.image('block-yellow.png')
        self.block_imgs['green'] = pyglet.resource.image('block-green.png')
        self.block_imgs['blue'] = pyglet.resource.image('block-blue.png')
        self.block_imgs['purple'] = pyglet.resource.image('block-purple.png')
        #self.block_grid = list(10)
        #for row in range(0, 9):
        #    self.block_grid[row] = list(16)
        self.generate_blocks()

    def generate_blocks(self):
        pass

class Player:

    bounds = Bounds(60, 30, 60, 30)
    speed = 300

    def __init__(self):
        self.forward = True # True for facing right, false to face left
        self._init_anims()
        self.sprite = BoundSprite(self.anims['stand_r'], Bounds(60, 30, 60, 30),
            batch=batch, group=hero_group)
        self.keys = key.KeyStateHandler()
        self.to_flying()

    def _init_anims(self):
        self.anims = dict()
        self.anims['stand_r'] = Animation((
            AnimationFrame(load_img('makayla_stand_1.png'), 1/4),
            AnimationFrame(load_img('makayla_stand_2.png'), 1/4),
            AnimationFrame(load_img('makayla_stand_3.png'), 1/4),
            AnimationFrame(load_img('makayla_stand_2.png'), 1/4)))
        self.anims['walk_r'] = Animation((
            AnimationFrame(load_img('makayla_walk_1.png'), 1/6),
            AnimationFrame(load_img('makayla_walk_2.png'), 1/6),
            AnimationFrame(load_img('makayla_walk_3.png'), 1/6),))
        self.anims['fly_r'] = Animation((
            AnimationFrame(load_img('makayla_fly_1.png'), 1/4),
            AnimationFrame(load_img('makayla_fly_2.png'), 1/4)))
        self.anims['stand_l'] = self.anims['stand_r'].get_transform(flip_x=True)
        self.anims['walk_l'] = self.anims['walk_r'].get_transform(flip_x=True)
        self.anims['fly_l'] = self.anims['fly_r'].get_transform(flip_x=True)


    def to_flying(self):
        self.update = self.update_flying
        if self.forward:
            self.sprite.image = self.anims['fly_r']
        else:
            self.sprite.image = self.anims['fly_l']

    def to_standing(self):
        self.update = self.update_standing
        if self.forward:
            self.sprite.image = self.anims['stand_r']
        else:
            self.sprite.image = self.anims['stand_l']

    def to_walking(self):
        self.update = self.update_walking
        if self.forward:
            self.sprite.image = self.anims['walk_r']
        else:
            self.sprite.image = self.anims['walk_l']

    def update_flying(self, dt):
        if self.keys[key.UP]:
            self.sprite.y += Player.speed * dt
        if self.keys[key.DOWN]:
            self.sprite.y -= Player.speed * dt
        if self.keys[key.LEFT]:
            self.sprite.x -= Player.speed * dt
            if self.forward:
                self.forward = False
                self.to_flying()
        if self.keys[key.RIGHT]:
            self.sprite.x += Player.speed * dt
            if not self.forward:
                self.forward = True
                self.to_flying()
        self.bound(playfield_bounds)
        if self.on_floor():
            self.to_standing()

    def update_standing(self, dt):
        if self.keys[key.UP]:
            self.sprite.y += Player.speed * dt
            self.to_flying()
        if self.keys[key.LEFT]:
            self.sprite.x -= Player.speed * dt
            self.forward = False
            self.to_walking()
        if self.keys[key.RIGHT]:
            self.sprite.x += Player.speed * dt
            self.forward = True
            self.to_walking()
        self.bound(playfield_bounds)

    def update_walking(self, dt):
        if self.keys[key.UP]:
            self.to_flying()
            self.sprite.y += Player.speed * dt
        if self.keys[key.LEFT]:
            self.sprite.x -= Player.speed * dt
            if self.forward:
                self.forward = False
                self.to_walking()
        if self.keys[key.RIGHT]:
            self.sprite.x += Player.speed * dt
            if not self.forward:
                self.forward = True
                self.to_walking()
        self.bound(playfield_bounds)
        if not self.keys[key.LEFT] and not self.keys[key.RIGHT]:
            self.to_standing()

    def on_floor(self):
        return self.sprite.d == playfield_bounds.d

    def bound(self, bounds):
        if self.sprite.d < bounds.d:
            self.sprite.d = bounds.d
        if self.sprite.l < bounds.l:
            self.sprite.l = bounds.l
        if self.sprite.u > bounds.u:
            self.sprite.u = bounds.u
        if self.sprite.r > bounds.r:
            self.sprite.r = bounds.r

# Create the window
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Colorwand Castle')

player = Player()
player.sprite.x = WIDTH // 4
player.sprite.d = playfield_bounds.d

room = Room()

# Allow the player to receive keyboard input
window.push_handlers(player.keys)

fps_display = pyglet.clock.ClockDisplay()

# Create the backgrounds
background_img = pyglet.resource.image('background.png')
background = Sprite(background_img, batch=batch, group=background_group)
walls_img = pyglet.resource.image('walls.png')
walls = Sprite(walls_img, batch=batch, group=walkground_group)

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

