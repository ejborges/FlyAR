#!/usr/bin/python
from __future__ import absolute_import, division, print_function, unicode_literals

import pi3d
from picamera import PiCamera

DISPLAY = pi3d.Display.create(x=0, y=0, background=(0.0,0.0,0.0,0.0), layer=3)

CAMERA = pi3d.Camera(at=(0, 0, 1), eye=(0, 0, 0))
TEXT_CAMERA = pi3d.Camera(is_3d=False)
piCamera = PiCamera()
piCamera.start_preview()
# Shaders
shader = pi3d.Shader("uv_light")
shinesh = pi3d.Shader("uv_reflect")
flatsh = pi3d.Shader("uv_flat")
matsh = pi3d.Shader("mat_reflect")
#################################
# Textures
patimg = pi3d.Texture("textures/PATRN.PNG")
shapebump = pi3d.Texture("textures/floor_nm.jpg")
shapshine = pi3d.Texture("textures/stars.jpg")
light = pi3d.Light(lightpos=(-1.0, 0.0, 10.0), lightcol=(3.0, 3.0, 2.0), lightamb=(0.02, 0.01, 0.03), is_point=True)

# Create shape
cylinder = pi3d.EnvironmentCube(light=light, size=1, name="Cylinder", x=0, y=0, z=5)
cylinder.set_line_width(2)

# Text
arialFont = pi3d.Font("fonts/FreeMonoBoldOblique.ttf", (221,0,170,255))
delta = 1

# Fetch key presses
mykeys = pi3d.Keyboard()
scale = 1
lastKeyRead = -1
while DISPLAY.loop_running():
    displayString = "Delta: {}, X: {}, Y:{}, Z: {}".format(delta, cylinder.x(), cylinder.y(), cylinder.z())
    mystring = pi3d.FixedString("fonts/FreeMonoBoldOblique.ttf", displayString, camera=TEXT_CAMERA, color=(255,255,255,255), font_size=24, margin=0.0, justify='R', background_color=(0,0,0,255), shader=flatsh, f_type='SMOOTH')
    CAMERA.reset()
    cylinder.draw(shinesh, [patimg, shapebump, shapshine], 4.0, 0.1)

    mystring.draw()

    k = mykeys.read()
    if k != -1:
        lastKeyRead = k
    
    if k == 97:
        # A key pressed
        previousXPosition = cylinder.x()
        cylinder.positionX(previousXPosition - delta)
    elif k == 100:
        # D key pressed
        previousXPosition = cylinder.x()
        cylinder.positionX(previousXPosition + delta)
    elif k == 119:
        # W key pressed
        previousYPosition = cylinder.y()
        cylinder.positionY(previousYPosition + delta)
    elif k == 115:
        # S key pressed
        previousYPosition = cylinder.y()
        cylinder.positionY(previousYPosition - delta)
    elif k == 113:
        # Q key pressed
        previousZPosition = cylinder.z()
        cylinder.positionZ(previousZPosition - delta)
    elif k == 101:
        # E key pressed
        previousZPosition = cylinder.z()
        cylinder.positionZ(previousZPosition + delta)
    elif k == 49:
        # 1 key pressed
        delta = 1
    elif k == 50:
        # 2 key pressed
        delta = 0.5
    elif k == 51:
        # 3 key pressed
        delta = 0.25
    elif k == 52:
        # 4 key pressed
        delta = 0.1
    elif k == 53:
        # 5 key pressed
        delta = 0.05
    elif k == 54:
        # 6 key pressed
        delta = 0.025
    elif k == 55:
        # 7 key pressed
        delta = 0.001
    elif k == 56:
        # 8 key pressed
        delta = 0.0005
    elif k == 57:
        # 9 key pressed
        delta = 0.00025
    elif k == 259:
        # Up arrow pressed
        scale += delta
        cylinder.scale(scale, scale, scale)
    elif k == 258:
        # Down arrow pressed
        scale -= delta
        cylinder.scale(scale, scale, scale)
    elif k == 263:
        # Backspace pressed
        delta = 1
        scale = 1
        cylinder.positionX(0)
        cylinder.positionY(0)
        cylinder.positionZ(5)
        cylinder.scale(1,1,1)
    elif k == 27:
        # Escape pressed
        mykeys.close()
        piCamera.stop_preview()
        DISPLAY.destroy()
        break
