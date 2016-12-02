#!/usr/bin/python
from __future__ import absolute_import, division, print_function, unicode_literals

# Handle logging FIRST
import logging
logging.basicConfig(filename='/var/tmpflyar/flyar.log', level=logging.INFO)

from sense_hat import SenseHat
import pi3d
from picamera import PiCamera
from configparser import read_config
from dataretriever import FlyARData
import math
import gc
from time import time

USE_CAMERA = False
DISPLAY = pi3d.Display.create(x=0, y=0, background=(100.0,100.0,100.0,0.0), layer=3)

shapesToDraw = read_config()
CAMERA = pi3d.Camera(at=(0, 0, 10), eye=(0, 0, 0))

if USE_CAMERA:
    piCamera = PiCamera()
    piCamera.start_preview()

# Create pi3d shapes based on the config information
pi3dShapes = []
objectNumbers = []
objectNumber = 1
arialFont = pi3d.Font("fonts/FreeMonoBoldOblique.ttf", (221,0,170,255))
shader = pi3d.Shader('uv_flat')
for shape in shapesToDraw:
    ringRots = 12 if shape.shapeType == 1 else 4
    newShape = pi3d.Torus(radius=shape.radius, ringrots=ringRots, sides=ringRots, x=shape.position[0], y=shape.position[1], z=shape.position[2], rx=90, rz=45, thickness=0.1)
    newShape.set_material((shape.color[0]/255, shape.color[1]/255, shape.color[2]/255))
    logMessage = str(time()) + ":[FLY-AR] Created Torus with radius: {}, Sides: {}, Position: {}, Color: {}".format(shape.radius, ringRots, shape.position, shape.color)
    logging.info(logMessage)
    pi3dShapes.append(newShape)

    # Add the corresponding number to each shape
    shapeString = pi3d.String(font=arialFont, string=str(objectNumber), x=shape.position[0], y=shape.position[1], z=shape.position[2])
    shapeString.set_shader(shader)
    objectNumbers.append(shapeString)
    objectNumber += 1



#Let's have a cow and Marvin
cow = pi3d.Model(file_string='cow.obj', name='cow', z=-5.0, sx=.2, sy=.2, sz=.2)

# Fetch key presses
mykeys = pi3d.Keyboard()

ORIGINAL_YAW = None
ORIGINAL_PITCH = None
ORIGINAL_ROLL = None

previousYaw = None
previousPitch = None
previousRoll = None

cameraX = 0
cameraY = 0
cameraZ = 0

sensorData = FlyARData()
first = True
while DISPLAY.loop_running():
    sensorData.update()
    CAMERA.reset()
    for shape in pi3dShapes:
        shape.draw()
    for text in objectNumbers:
        text.draw()

    cow.draw()

    # If this is the first time, get the original values
    if first:
        ORIGINAL_YAW = sensorData.yaw
        ORIGINAL_PITCH = sensorData.pitch
        ORIGINAL_ROLL = sensorData.roll
        first = False

    cameraY = ORIGINAL_YAW - sensorData.yaw
    cameraX = ORIGINAL_PITCH - sensorData.pitch
    cameraZ = ORIGINAL_ROLL - sensorData.roll

    CAMERA.rotateY(cameraY)
    CAMERA.rotateX(cameraX)
    CAMERA.rotateZ(cameraZ)

    gc.collect()

    k = mykeys.read()
    
    if k == 27:
        mykeys.close()
        if USE_CAMERA:
            piCamera.stop_preview()
        DISPLAY.destroy()
        break
