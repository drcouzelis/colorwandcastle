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

def to_tile(pos):
    return pos // Block.size

def to_pos(tile):
    return tile * Block.size

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

class Star:

    speed = 200

    imgs = dict()
    for color in Block.colors:
        imgs[color] = load_img('star-{}.png'.format(color))
    imgs_flip = dict()
    for color in Block.colors:
        imgs_flip[color] = imgs[color].get_transform(flip_y=True)

    def __init__(self, color, x, y):
        if color not in Block.colors:
            raise
        anim = Animation((
            AnimationFrame(Star.imgs[color], 1/4),
            AnimationFrame(Star.imgs_flip[color], 1/4)))
        sprite = Sprite(anim, batch=batch, group=act_group, x=x + Block.size, y=y)
        bounds = Bounds(5, 5, 5, 5)
        self.actor = Actor(sprite, bounds)
        self.controller = None

    def update(self, dt):
        if self.controller:
            self.controller.update(dt)

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
        self.star = Star('red', x=self.x + Block.size, y=self.y)
        self.star.controller = FollowPlayerStarController(star=self.star, player=self)

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
        if self.star:
            self.star.update(dt)

class Room:

    cols = WIDTH // Block.size
    rows = HEIGHT // Block.size

    def __init__(self, columns, colors):
        self.bg = self.create_bg()
        self.walls = self.create_walls()
        self.blocks = self.create_blocks(columns, colors)
        self.enemies = list()

    def create_bg(self):
        bg = list()
        for c in range(Room.cols):
            for r in range(Room.rows):
                bg.append(Sprite(pyglet.resource.image('background.png'),
                    x=c * Block.size, y=r * Block.size, batch=batch, group=bg_group))
        return bg

    def create_walls(self):
        walls = dict()
        for c in range(Room.cols):
            for r in range(Room.rows):
                if c == 0 or c == Room.cols - 1 or r == 0 or r == Room.rows - 1:
                    sprite = Sprite(pyglet.resource.image('bricks.png'),
                        x=c * Block.size, y=r * Block.size, batch=batch, group=fg_group)
                    walls[(r, c)] = sprite
        return walls

    def create_blocks(self, columns, colors):
        blocks = dict()
            # Access with a (r, c) tuple
        for c in range(Room.cols - columns - 1, Room.cols - 1):
            for r in range(1, Room.rows - 1):
                color = random.choice(Block.colors[:colors])
                block = Block(color, c * Block.size, r * Block.size)
                blocks[(r, c)] = block
        return blocks

class FollowPlayerStarController:

    offset_x = 25
    offset_y = 10
	
    def __init__(self, star, player):
        self.star = star
        self.player = player

    def update(self, dt):
        self.star.x = self.player.x + FollowPlayerStarController.offset_x
        self.star.y = ((self.player.y // Block.size) * Block.size) + FollowPlayerStarController.offset_y

class ShootingStarController:

    pass

class KeyboardPlayerController:

    def __init__(self, player, room):
        self.player = player
        self.room = room
        self.star_shot = False

    def update(self, dt):
        orig_x = self.player.x
        orig_y = self.player.y
        keys = self.player.keys
        if keys[key.UP]:
            self.player.y += Player.speed * dt
            if not self.good_move():
                self.player.y = orig_y
        if keys[key.DOWN]:
            self.player.y -= Player.speed * dt
            if not self.good_move():
                self.player.y = orig_y
        if keys[key.LEFT]:
            self.player.x -= Player.speed * dt
            if not self.good_move():
                self.player.x = orig_x
        if keys[key.RIGHT]:
            self.player.x += Player.speed * dt
            if not self.good_move():
                self.player.x = orig_x
        # Shoot a star!
        if keys[key.SPACE] and not self.star_shot:
            print('Pretending to shoot a star!')
            self.star_shot = True
        if self.star_shot and not keys[key.SPACE]:
            self.star_shot = False

    def good_move(self):
        walls = self.room.walls
        blocks = self.room.blocks
        u = to_tile(self.player.actor.u)
        l = to_tile(self.player.actor.l)
        d = to_tile(self.player.actor.d)
        r = to_tile(self.player.actor.r)
        # Don't fly through walls!
        if (u, l) in walls or (u, r) in walls or (d, l) in walls or (d, r) in walls:
            return False
        # Don't fly through blocks!
        if (u, l) in blocks or (u, r) in blocks or (d, l) in blocks or (d, r) in blocks:
            return False
        return True

#class PlayEnvironment:
#
#    def __init__(self, player=None, room=None):
#        self.room = room
#        self.player = player
#
#    def move_possible(self, to_move, x, y):
#        if not room:
#            return True
#        if isinstance(to_move, Player):
#            u = (y + to_move.actor.bounds.u) // Block.size
#            l = (x + to_move.actor.bounds.l) // Block.size
#            d = (y + to_move.actor.bounds.d) // Block.size
#            r = (x + to_move.actor.bounds.r) // Block.size
#            print('{} {} {} {}'.format(u, l, d, r))
#            #if (u, l) or (u, r) or (d, l) or (d, r) in room.blocks:
#            if (u, l) in room.blocks:
#                return False
#        return True
#
#    def update_collisions(self):
#        pass

scale = 3

# Create the window
window = pyglet.window.Window(width=WIDTH * scale, height=HEIGHT * scale, caption='Colorwand Castle')

# These arguments are x, y and z respectively
# This scales your window
glScalef(scale, scale, scale)

player = Player()
player.x = WIDTH // 4
player.y = HEIGHT // 2

# Allow the player to receive keyboard input
window.push_handlers(player.keys)

room = Room(columns=4, colors=6)

player.controller = KeyboardPlayerController(player, room)

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

