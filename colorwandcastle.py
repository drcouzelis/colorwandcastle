#!/usr/bin/env python3

import pyglet
from pyglet.window import key

WIDTH = 1280
HEIGHT = 800
FPS = 120.0
WALK_SPEED = 240

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
    def __init__(self):
        self.fly_img = pyglet.resource.image('makayla_fly_1.png')
        self.stand_img = pyglet.image.resource('makayla_stand_1.png')
        self.bounds = Bounds(60, 30, 60, 30)
        self.to_flying()

    def to_flying(self):
        self.sprite = pyglet.sprite.Sprite(fly_img, batch=batch, group=character_group)
        self.update = self.update_flying

    def update_flying(self, dt):
        self.sprite.y += WALK_SPEED * dt if self[key.UP]
        self.sprite.y -= WALK_SPEED * dt if self[key.DOWN]
        self.sprite.x -= WALK_SPEED * dt if self[key.LEFT]
        self.sprite.x += WALK_SPEED * dt if self[key.RIGHT]
        bound(self.sprite, playfield_bounds)
        if self.sprite.y == playfield.d:
            self.to_standing()

# Create the window
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Colorwand Castle')

player = Player()
player.sprite.x = WIDTH // 2
player.sprite.y = HEIGHT // 2

# Allow the player to receive keyboard input
window.push_handlers(player)

fps_display = pyglet.clock.ClockDisplay()

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

