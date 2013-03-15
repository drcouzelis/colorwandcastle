#!/usr/bin/env python3

import pyglet
from pyglet.window import key

WIDTH = 1280
HEIGHT = 800
FPS = 120.0
BLOCK_SIZE = 80

# Define resource directories
pyglet.resource.path = ['res']
pyglet.resource.reindex()

# Put all sprites into a single batch to speed up drawing
batch = pyglet.graphics.Batch()

# Groups allow sprite to be drawn in the correct layer
background_group = pyglet.graphics.OrderedGroup(0)
walkground_group = pyglet.graphics.OrderedGroup(1)
character_group = pyglet.graphics.OrderedGroup(2)

class Bounds:
    def __init__(self, u=0, l=0, d=0, r=0):
        self.u, self.l, self.d, self.r = u, l, d, r

playfield_bounds = Bounds(HEIGHT - BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, WIDTH - BLOCK_SIZE)

class Player(pyglet.window.key.KeyStateHandler):

    speed = 300
    R = 'right'
    L = 'left'

    def __init__(self):
        self.sprite = None
        self.dir = Player.R
        self.fly_anm = pyglet.image.Animation((
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_fly_1.png'), 1/4),
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_fly_2.png'), 1/4)
            ))
        self.stand_anm = pyglet.image.Animation((
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_stand_1.png'), 1/4),
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_stand_2.png'), 1/4),
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_stand_3.png'), 1/4),
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_stand_2.png'), 1/4)
            ))
        self.walk_anm = pyglet.image.Animation((
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_walk_1.png'), 1/4),
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_walk_2.png'), 1/4),
            pyglet.image.AnimationFrame(pyglet.resource.image('makayla_walk_3.png'), 1/4),
            ))
        self.bounds = Bounds(171, 52, 51, 112)
        self.to_flying()

    @property
    def x(self):
        return self.sprite.x if self.sprite else 0

    @property
    def y(self):
        return self.sprite.y if self.sprite else 0

    @x.setter
    def x(self, value):
        self.sprite.x = value

    @y.setter
    def y(self, value):
        self.sprite.y = value

    def to_flying(self):
        if self.sprite:
            self.sprite.delete()
        flip = True if self.dir == Player.L else False
        anm = self.fly_anm.get_transform(flip_x=flip)
        if flip:
            anm.anchor_x = 0
        self.sprite = pyglet.sprite.Sprite(anm, x=self.x, y=self.y, batch=batch, group=character_group)
        self.update = self.update_flying

    def to_standing(self):
        if self.sprite:
            self.sprite.delete()
        flip = True if self.dir == Player.L else False
        anm = self.stand_anm.get_transform(flip_x=flip)
        if flip:
            anm.anchor_x = 0
        self.sprite = pyglet.sprite.Sprite(anm, x=self.x, y=self.y, batch=batch, group=character_group)
        self.update = self.update_standing

    def to_walking(self):
        if self.sprite:
            self.sprite.delete()
        flip = True if self.dir == Player.L else False
        anm = self.walk_anm.get_transform(flip_x=flip)
        if flip:
            anm.anchor_x = 0
        self.sprite = pyglet.sprite.Sprite(anm, x=self.x, y=self.y, batch=batch, group=character_group)
        self.update = self.update_walking

    def update_flying(self, dt):
        if self[key.UP]:
            self.y += Player.speed * dt
        if self[key.DOWN]:
            self.y -= Player.speed * dt
        if self[key.LEFT]:
            self.x -= Player.speed * dt
            if self.dir != Player.L:
                self.dir = Player.L
                self.to_flying()
        if self[key.RIGHT]:
            self.x += Player.speed * dt
            if self.dir != Player.R:
                self.dir = Player.R
                self.to_flying()
        self.bound(playfield_bounds)
        if self.on_floor():
            self.to_standing()

    def update_standing(self, dt):
        if self[key.UP]:
            self.y += Player.speed * dt
            self.to_flying()
        if self[key.LEFT]:
            self.x -= Player.speed * dt
            self.dir = Player.L
            self.to_walking()
        if self[key.RIGHT]:
            self.x += Player.speed * dt
            self.dir = Player.R
            self.to_walking()
        self.bound(playfield_bounds)

    def update_walking(self, dt):
        if self[key.UP]:
            self.to_flying()
            self.y += Player.speed * dt
        if self[key.LEFT]:
            self.x -= Player.speed * dt
            if self.dir != Player.L:
                self.dir = Player.L
                self.to_walking()
        if self[key.RIGHT]:
            self.x += Player.speed * dt
            if self.dir != Player.R:
                self.dir = Player.R
                self.to_walking()
        self.bound(playfield_bounds)
        if not self[key.LEFT] and not self[key.RIGHT]:
            self.to_standing()

    def on_floor(self):
        return self.y + self.bounds.d == playfield_bounds.d

    def bound(self, bounds):
        if self.y + self.bounds.d < bounds.d:
            self.y = bounds.d - self.bounds.d
        if self.x + self.bounds.l < bounds.l:
            self.x = bounds.l - self.bounds.l
        if self.y + self.bounds.u > bounds.u:
            self.y = bounds.u - self.bounds.u
        if self.x + self.bounds.r > bounds.r:
            self.x = bounds.r - self.bounds.r

# Create the window
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Colorwand Castle')

player = Player()
player.sprite.x = WIDTH // 2
player.sprite.y = HEIGHT // 2

# Allow the player to receive keyboard input
window.push_handlers(player)

fps_display = pyglet.clock.ClockDisplay()

# Create the backgrounds
background_img = pyglet.resource.image('background.png')
background = pyglet.sprite.Sprite(background_img, batch=batch, group=background_group)
walls_img = pyglet.resource.image('walls.png')
walls = pyglet.sprite.Sprite(walls_img, batch=batch, group=walkground_group)

@window.event
def on_draw():
    window.clear() # Renove when unnecessary
    batch.draw() # Draw all sprites
    fps_display.draw() # Should be able to be toggled

def update(dt):
    player.update(dt)

# Update the game once every frame
pyglet.clock.schedule_interval(update, 1 / FPS)

# Start the game!
pyglet.app.run()

