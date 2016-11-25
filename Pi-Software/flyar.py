#!/usr/bin/python
from __future__ import absolute_import, division, print_function, unicode_literals

# Handle logging FIRST
import logging
logging.basicConfig(filename='flyar.log', filemode='w', level=logging.INFO)

from sense_hat import SenseHat
import pi3d
from picamera import PiCamera
from ConfigParser import read_config

# TODO Replace this with a wrapper interface so we can quickly switch from SenseHAT to Emu's sensors
sense = SenseHat()
sense.clear()
DISPLAY = pi3d.Display.create(x=0, y=0, background=(0.0,0.0,0.0,0.0), layer=3)

shapesToDraw = read_config()
CAMERA = pi3d.Camera(at=(0, 0, 10), eye=(0, 0, 0))
#piCamera = PiCamera()
#piCamera.start_preview()

# Create pi3d shapes based on the config information
pi3dShapes = []
objectNumbers = []
objectNumber = 1
arialFont = pi3d.Font("fonts/FreeMonoBoldOblique.ttf", (221,0,170,255))
shader = pi3d.Shader('uv_flat')
for shape in shapesToDraw:
    ringRots = 12 if shape.shapeType == 1 else 4
    newShape = pi3d.Torus(radius=shape.radii[0], ringrots=ringRots, sides=ringRots, x=shape.position[0], y=shape.position[1], z=shape.position[2], rx=90, rz=45, thickness=0.1)
    newShape.set_material((shape.color[0]/255, shape.color[1]/255, shape.color[2]/255))
    logMessage = "Created Torus with radius: {}, Sides: {}, Position: {}, Color: {}".format(shape.radii[0], ringRots, shape.position, shape.color)
    logging.info(logMessage)
    pi3dShapes.append(newShape)

    # Add the corresponding number to each shape
    shapeString = pi3d.String(font=arialFont, string=str(objectNumber), x=shape.position[0], y=shape.position[1], z=shape.position[2])
    shapeString.set_shader(shader)
    objectNumbers.append(shapeString)
    objectNumber += 1



# Fetch key presses
mykeys = pi3d.Keyboard()

orientation = sense.get_orientation_degrees()
ORIGINAL_YAW = orientation['yaw']
ORIGINAL_PITCH = orientation['pitch']
ORIGINAL_ROLL = orientation['roll']
ORIGINAL_PRESSURE = sense.get_pressure()

previousYaw = ORIGINAL_YAW
previousPitch = ORIGINAL_PITCH
previousRoll = ORIGINAL_ROLL

cameraX = 0
cameraY = 0
cameraZ = 0
while DISPLAY.loop_running():
    CAMERA.reset()
    for shape in pi3dShapes:
        shape.draw()
    for text in objectNumbers:
        text.draw()
    #cylinder.draw(shinesh, [patimg, shapebump, shapshine], 4.0, 0.1)
    #mystring.draw()

    orientation = sense.get_orientation_degrees()
    currentYaw = orientation['yaw']
    currentPitch = orientation['pitch']
    currentRoll = orientation['roll']

    if abs(previousYaw - currentYaw) > .265389732:
        cameraY = ORIGINAL_YAW - currentYaw
        previousYaw = currentYaw

    if abs(previousPitch - currentPitch) > .31902688:
        cameraX = ORIGINAL_PITCH - currentPitch
        previousPitch = currentPitch

    if abs(previousRoll - currentRoll) > .14933539:
        cameraZ = ORIGINAL_ROLL - currentRoll
        previousRoll = currentRoll

    CAMERA.rotateY(cameraY)
    CAMERA.rotateX(cameraX)
    CAMERA.rotateZ(cameraZ)

    k = mykeys.read()
    
    if k == 27:
        mykeys.close()
#        piCamera.stop_preview()
        DISPLAY.destroy()
        break
