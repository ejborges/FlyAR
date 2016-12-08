#!/usr/bin/python
from __future__ import absolute_import, division, print_function, unicode_literals

import pi3d

# Flags to use during debug mode
DISPLAY = pi3d.Display.create(x=0, y=0, background=(255.0,255.0,255.0,1.0), layer=3)

CAMERA = pi3d.Camera(at=(0, 0, 10), eye=(0, 0, 0))

# Create pi3d shapes based on the config information
arialFont = pi3d.Font("fonts/FreeMonoBoldOblique.ttf", (100,100,100,255))
shader = pi3d.Shader('uv_flat')

mykeys = pi3d.Keyboard()

# Build each distinct 3D model here. This makes it faster as it will use the same batch of vertices, reducing load time
cowModel = pi3d.Model(file_string='objs/cow.obj', x=0, y=0, z=3, sx=.1, sy=.1, sz=.1)
cowModel.set_material((.5,.5,.5))
rocketModel = pi3d.Model(file_string='objs/phoenix/Aim-54_Phoenix.obj', x=0, y=0, z=3, sx=.005, sy=.005, sz=.005)
rocketModel.set_material((.5,.5,.5))

ring = pi3d.ImageSprite(texture='sprites/ToledoRockets.png', x=0, y=0, z=3, sx=.4, sy=.4, sz=.4, shader=shader)

screenshotNumber = 0
while DISPLAY.loop_running():
    CAMERA.reset()

    ring.draw()
        
    k = mykeys.read()
    
    if k == 27:
        mykeys.close()
        DISPLAY.destroy()
        break
    elif k == 10:
        fileName = 'screenshot' + str(screenshotNumber) + '.jpg'
        pi3d.screenshot(fileName)
        screenshotNumber += 1
