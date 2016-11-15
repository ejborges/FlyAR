#!/usr/bin/python
from __future__ import absolute_import, division, print_function, unicode_literals
from sense_hat import SenseHat
import pi3d
from picamera import PiCamera
from ConfigParser import read_config

# TODO Replace this with a wrapper interface so we can quickly switch from SenseHAT to Emu's sensors
sense = SenseHat()
sense.clear()
DISPLAY = pi3d.Display.create(x=20, y=20, background=(0.0,0.0,0.0,0.0), layer=3)

shapesToDraw = read_config()
ORIGINAL_CAMERA_ALTITUDE = 0
CAMERA = pi3d.Camera(at=(0, 0, 10), eye=(0, 0, 0))
piCamera = PiCamera()
piCamera.start_preview()
# Shaders
#shader = pi3d.Shader("uv_light")
#shinesh = pi3d.Shader("uv_reflect")
#flatsh = pi3d.Shader("uv_flat")
#matsh = pi3d.Shader("mat_reflect")
#################################
# Textures
#patimg = pi3d.Texture("textures/PATRN.PNG")
#shapebump = pi3d.Texture("textures/floor_nm.jpg")
#shapshine = pi3d.Texture("textures/stars.jpg")
#light = pi3d.Light(lightpos=(-1.0, 0.0, 10.0), lightcol=(3.0, 3.0, 2.0), lightamb=(0.02, 0.01, 0.03), is_point=True)

# Create pi3d shapes based on the config information
pi3dShapes = []
for shape in shapesToDraw:
    ringRots = 4 if shape.shapeType == 1 else 12
    newShape = pi3d.Torus(radius=shape.radii[0], ringrots=ringRots, sides=ringRots, x=shape.position[0], y=shape.position[1], z=shape.position[2], rx=90, rz=45, thickness=0.1)
    newShape.set_material((shape.color[0], shape.color[1], shape.color[2]))
    pi3dShapes.append(newShape)

# Text - Sample
#arialFont = pi3d.Font("fonts/FreeMonoBoldOblique.ttf", (221,0,170,255))
#mystring = pi3d.String(font=arialFont, string="Something", z=4)
##mystring.set_shader(flatsh)

# Fetch key presses
mykeys = pi3d.Keyboard()

orientation = sense.get_orientation_degrees()
ORIGINAL_YAW = orientation['yaw']
ORIGINAL_PITCH = orientation['pitch']
ORIGINAL_ROLL = orientation['roll']
ORIGINAL_PRESSURE = sense.get_pressure()

while DISPLAY.loop_running():
    CAMERA.reset()
    for shape in pi3dShapes:
        shape.draw()
    #cylinder.draw(shinesh, [patimg, shapebump, shapshine], 4.0, 0.1)
    #mystring.draw()

    orientation = sense.get_orientation_degrees()
    CAMERA.rotateY(ORIGINAL_YAW - orientation['yaw'])
    CAMERA.rotateX(ORIGINAL_PITCH - orientation['pitch'])
    CAMERA.rotateZ(ORIGINAL_ROLL - orientation['roll'])

    # Updating the position of the object based on the pressure
    
    k = mykeys.read()
    
    if k == 27:
        mykeys.close()
        piCamera.stop_preview()
        DISPLAY.destroy()
        break
