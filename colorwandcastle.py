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

def center_anchor(img):
    '''Change the image anchor to be the center of the image.
       Returns the image.'''
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2
    return img

# Put all sprites into a single batch to speed up drawing
batch = pyglet.graphics.Batch()

# Groups allow sprite to be drawn in the correct layer
background_group = pyglet.graphics.OrderedGroup(0)
walkground_group = pyglet.graphics.OrderedGroup(1)
character_group = pyglet.graphics.OrderedGroup(2)

class Player(pyglet.window.key.KeyStateHandler):
    def __init__(self):
        img = center_anchor(pyglet.resource.image('makayla.png'))
        self.sprite = pyglet.sprite.Sprite(img, batch=batch, group=character_group)
        self.sprite.x = WIDTH // 2
        self.sprite.y = HEIGHT // 2

    def update(self, dt):
        if self[key.UP]:
            self.sprite.y += WALK_SPEED * dt
        if self[key.DOWN]:
            self.sprite.y -= WALK_SPEED * dt
        if self[key.LEFT]:
            self.sprite.x -= WALK_SPEED * dt
        if self[key.RIGHT]:
            self.sprite.x += WALK_SPEED * dt

player = Player()

# Create the window
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Colorwand Castle')

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

