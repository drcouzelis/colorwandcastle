#!/usr/bin/env python3

import pyglet
from pyglet.window import key

WIDTH = 1280
HEIGHT = 800
FPS = 120.0

pyglet.resource.path = ['res']
pyglet.resource.reindex()

def center_anchor(img):
    img.anchor_x = img.width // 2
    img.anchor_y = img.height // 2
    return img

batch = pyglet.graphics.Batch()
background_group = pyglet.graphics.OrderedGroup(0)
walkground_group = pyglet.graphics.OrderedGroup(1)
character_group = pyglet.graphics.OrderedGroup(2)

class Player(pyglet.window.key.KeyStateHandler):
    def __init__(self):
        makayla_img = 
        center_anchor(makayla_img)
        self.sprite = pyglet.sprite.Sprite(center_anchor(pyglet.resource.image('makayla.png')), batch=batch, group=character_group)

    def update(self, dt):
        if self[key.LEFT]:
            sprite.x -= 10 * dt
        if self[key.RIGHT]:
            sprite.x += 10 * dt

player = Player()
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Colorwand Castle')
window.push_handlers(player)

fps_display = pyglet.clock.ClockDisplay()

@window.event
def on_draw():
    window.clear()
    batch.draw()
    fps_display.draw()

def update(dt):
    player.update(dt)

pyglet.clock.schedule_interval(update, 1 / FPS)
window.set_visible(True)
pyglet.app.run()

