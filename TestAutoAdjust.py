#!/usr/bin/python
from __future__ import absolute_import, division, print_function, unicode_literals

""" Some
multiline
comment
"""

import pi3d
from picamera import PiCamera

DISPLAY = pi3d.Display.create(x=20, y=20, background=(0.0,0.0,0.0,0.0), layer=3)
CAMERA = pi3d.Camera(at=(0, 0, 10), eye=(0, 0, 0))

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
cylinder = pi3d.Cylinder(radius=0.7, height=1.5, sides=24, name="Cylinder", x=0, y=0, z=10)

# Text
arialFont = pi3d.Font("fonts/FreeMonoBoldOblique.ttf", (221,0,170,255))
mystring = pi3d.String(font=arialFont, string="Something", z=4)
mystring.set_shader(flatsh)

# Fetch key presses
mykeys = pi3d.Keyboard()
angl = 0

while DISPLAY.loop_running():
	CAMERA.reset()
	cylinder.draw(shinesh, [patimg, shapebump, shapshine], 4.0, 0.1)
	
	mystring.draw()

	CAMERA.rotateY(angl)
	angl += .05

	k = mykeys.read()
	if k == 27:
		mykeys.close()
		DISPLAY.destroy()
		break
