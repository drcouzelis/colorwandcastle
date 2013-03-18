#!/usr/bin/env python3

import pyglet
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
character_group = pyglet.graphics.OrderedGroup(2)

def load_img(filename):
    img = pyglet.resource.image(filename)
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2
    return img

class Bounds:
    def __init__(self, u=0, l=0, d=0, r=0):
        self.u, self.l, self.d, self.r = u, l, d, r

class Block(pyglet.sprite.Sprite):
    size = 80

playfield_bounds = Bounds(HEIGHT - Block.size, Block.size, Block.size, WIDTH - Block.size)

class Player(pyglet.window.key.KeyStateHandler):

    speed = 300

    def __init__(self):
        self.forward = True # True for facing right, false to face left
        self.anims = dict()
        self.anims['stand_r'] = pyglet.image.Animation((
            pyglet.image.AnimationFrame(load_img('makayla_stand_1.png'), 1/4),
            pyglet.image.AnimationFrame(load_img('makayla_stand_2.png'), 1/4),
            pyglet.image.AnimationFrame(load_img('makayla_stand_3.png'), 1/4),
            pyglet.image.AnimationFrame(load_img('makayla_stand_2.png'), 1/4)))
        self.anims['walk_r'] = pyglet.image.Animation((
            pyglet.image.AnimationFrame(load_img('makayla_walk_1.png'), 1/6),
            pyglet.image.AnimationFrame(load_img('makayla_walk_2.png'), 1/6),
            pyglet.image.AnimationFrame(load_img('makayla_walk_3.png'), 1/6),))
        self.anims['fly_r'] = pyglet.image.Animation((
            pyglet.image.AnimationFrame(load_img('makayla_fly_1.png'), 1/4),
            pyglet.image.AnimationFrame(load_img('makayla_fly_2.png'), 1/4)))
        self.anims['stand_l'] = self.anims['stand_r'].get_transform(flip_x=True)
        self.anims['walk_l'] = self.anims['walk_r'].get_transform(flip_x=True)
        self.anims['fly_l'] = self.anims['fly_r'].get_transform(flip_x=True)
        self.bounds = Bounds(60, 30, 60, 30)
        self.sprite = pyglet.sprite.Sprite(self.anims['fly_r'])
        self.to_flying()

    @property
    def x(self):
        return self.sprite.x

    @x.setter
    def x(self, value):
        self.sprite.x = value

    @property
    def y(self):
        return self.sprite.y

    @y.setter
    def y(self, value):
        self.sprite.y = value

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

    def to_flying(self):
        if self.sprite:
            self.sprite.delete()
        anim = self.anims['fly_r'] if self.forward else self.anims['fly_l']
        self.sprite = pyglet.sprite.Sprite(anim, x=self.x, y=self.y, batch=batch, group=character_group)
        self.update = self.update_flying

    def to_standing(self):
        self.sprite.delete()
        anim = self.anims['stand_r'] if self.forward else self.anims['stand_l']
        self.sprite = pyglet.sprite.Sprite(anim, x=self.x, y=self.y, batch=batch, group=character_group)
        self.update = self.update_standing

    def to_walking(self):
        self.sprite.delete()
        anim = self.anims['walk_r'] if self.forward else self.anims['walk_l']
        self.sprite = pyglet.sprite.Sprite(anim, x=self.x, y=self.y, batch=batch, group=character_group)
        self.update = self.update_walking

    def update_flying(self, dt):
        if self[key.UP]:
            self.y += Player.speed * dt
        if self[key.DOWN]:
            self.y -= Player.speed * dt
        if self[key.LEFT]:
            self.x -= Player.speed * dt
            if self.forward:
                self.forward = False
                self.to_flying()
        if self[key.RIGHT]:
            self.x += Player.speed * dt
            if not self.forward:
                self.forward = True
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
            self.forward = False
            self.to_walking()
        if self[key.RIGHT]:
            self.x += Player.speed * dt
            self.forward = True
            self.to_walking()
        self.bound(playfield_bounds)

    def update_walking(self, dt):
        if self[key.UP]:
            self.to_flying()
            self.y += Player.speed * dt
        if self[key.LEFT]:
            self.x -= Player.speed * dt
            if self.forward:
                self.forward = False
                self.to_walking()
        if self[key.RIGHT]:
            self.x += Player.speed * dt
            if not self.forward:
                self.forward = True
                self.to_walking()
        self.bound(playfield_bounds)
        if not self[key.LEFT] and not self[key.RIGHT]:
            self.to_standing()

    def on_floor(self):
        return self.d == playfield_bounds.d

    def bound(self, bounds):
        if self.d < bounds.d:
            self.d = bounds.d
        if self.l < bounds.l:
            self.l = bounds.l
        if self.u > bounds.u:
            self.u = bounds.u
        if self.r > bounds.r:
            self.r = bounds.r

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
    batch.draw() # Draw all sprites
    fps_display.draw() # Should be able to be toggled

def update(dt):
    player.update(dt)

# Update the game once every frame
pyglet.clock.schedule_interval(update, 1 / FPS)

# Start the game!
pyglet.app.run()

