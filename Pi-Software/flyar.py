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
DISPLAY = pi3d.Display.create(x=0, y=0, background=(100.0,100.0,100.0,1.0), layer=3)

shapesToDraw = read_config()
CAMERA = pi3d.Camera(at=(0, 0, 10), eye=(0, 0, 0))

if USE_CAMERA:
    piCamera = PiCamera()
    piCamera.start_preview()

# Create pi3d shapes based on the config information
pi3dShapes = []
objectNumbers = []
objectNumber = 1
arialFont = pi3d.Font("fonts/FreeMonoBoldOblique.ttf", (100,100,100,255))
shader = pi3d.Shader('uv_flat')

# Build each distinct shape here. This makes it faster
ring = pi3d.Torus(radius=0, ringrots=12, sides=12, x=0, y=0, z=3, rx=90, rz=45, thickness=0.05)
ring.draw()
ring.position(0, 5, 3)
ring.draw()

for shape in shapesToDraw:
    if shape.shapeType == 1 or shape.shapeType == 2:
        # Build a torus
        ringRots = 12 if shape.shapeType == 1 else 4
        newShape = pi3d.Torus(radius=shape.radius, ringrots=ringRots, sides=ringRots, x=shape.position[0], y=shape.position[1], z=shape.position[2], rx=90, rz=45, thickness=0.05)
        newShape.set_material((shape.color[0]/255, shape.color[1]/255, shape.color[2]/255))
        logMessage = str(time()) + ":[FLY-AR] Created Torus with radius: {}, Sides: {}, Position: {}, Color: {}".format(shape.radius, ringRots, shape.position, shape.color)
        logging.info(logMessage)
    elif shape.shapeType == 3:
        # Build a cow
        newShape = pi3d.Model(file_string='objs/cow.obj', name='cow' + str(objectNumber), x=shape.position[0], y=shape.position[1], z=shape.position[2], sx=shape.radius, sy=shape.radius, sz=shape.radius)
        newShape.set_material((shape.color[0]/255, shape.color[1]/255, shape.color[2]/255))
        logMessage = str(time()) + ":[FLY-AR] Created cow with scale: {}, position: {}, color: {}".format(shape.radius, shape.position, shape.color)
        logging.info(logMessage)
    elif shape.shapeType == 4:
        # Build a rocket
        newShape = pi3d.Model(file_string='objs/phoenix/Aim-54_Phoenix.obj', name='rocket' + str(objectNumber), x=shape.position[0], y=shape.position[1], z=shape.position[2], sx=shape.radius*.01, sy=shape.radius*.01, sz=shape.radius*.01)
        newShape.set_material((shape.color[0]/255, shape.color[1]/255, shape.color[2]/255))
        logMessage = str(time()) + ":[FLY-AR] Created rocket with scale: {}, position: {}, color: {}".format(shape.radius, shape.position, shape.color)
        logging.info(logMessage)
    else:
        # It is either the UT shield or rocket logo, both of which are sprites
        filePath = 'sprites/toledo_shield.gif' if shape.shapeType == 5 else 'sprites/ToledoRockets.png'
        newShape = pi3d.ImageSprite(texture=filePath, x=shape.position[0], y=shape.position[1], z=shape.position[2], sx=shape.radius, sy=shape.radius, sz=shape.radius, shader=shader)
        logMessage = str(time()) + ":[FLY-AR] Created image with path: {}, position: {}, scale: {}".format(filePath, shape.position, shape.radius)
        logging.info(logMessage)


    pi3dShapes.append(newShape)

    # Add the corresponding number to each shape
    shapeString = pi3d.String(font=arialFont, string=str(objectNumber), x=shape.position[0], y=shape.position[1], z=shape.position[2])
    shapeString.set_shader(shader)
    objectNumbers.append(shapeString)
    objectNumber += 1



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
