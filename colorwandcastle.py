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

batch = pyglet.graphics.Batch()
background_group = pyglet.graphics.OrderedGroup(0)
walkground_group = pyglet.graphics.OrderedGroup(1)
character_group = pyglet.graphics.OrderedGroup(2)

class Player(pyglet.window.key.KeyStateHandler):
    def __init__(self):
        makayla_img = pyglet.resource.image('makayla.png')
        center_anchor(makayla_img)
        self.makayla = pyglet.sprite.Sprite(img=makayla_img, x=WIDTH / 2, y=HEIGHT / 2)

    def update(self, dt):
        if self[key.LEFT]:
            print('Left!')

    def draw(self):
        self.makayla.draw()

player = Player()
window = pyglet.window.Window(width=WIDTH, height=HEIGHT, caption='Doctor Who', visible=False)
window.push_handlers(player)

fps_display = pyglet.clock.ClockDisplay()

@window.event
def on_draw():
    window.clear()
    player.draw()
    fps_display.draw()

def update(dt):
    player.update(dt)

pyglet.clock.schedule_interval(update, 1 / FPS)
window.set_visible(True)
pyglet.app.run()

