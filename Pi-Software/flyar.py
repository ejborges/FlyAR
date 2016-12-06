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

# Flags to use during debug mode
USE_CAMERA = True
DEBUG = True

DISPLAY = pi3d.Display.create(x=0, y=0, background=(0.0,255.0,0.0,1.0), layer=3)

shapesToDraw = read_config()
TEXT_CAMERA = pi3d.Camera(is_3d=False)
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

# Build each distinct 3D model here. This makes it faster as it will use the same batch of vertices, reducing load time
cowModel = pi3d.Model(file_string='objs/cow.obj')
rocketModel = pi3d.Model(file_string='objs/phoenix/Aim-54_Phoenix.obj')

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
        newShape = cowModel.clone()
        newShape.position(shape.position[0], shape.position[1], shape.position[2])
        newShape.scale(shape.radius, shape.radius, shape.radius)
        newShape.set_material((shape.color[0]/255, shape.color[1]/255, shape.color[2]/255))
        logMessage = str(time()) + ":[FLY-AR] Created cow with scale: {}, position: {}, color: {}".format(shape.radius, shape.position, shape.color)
        logging.info(logMessage)
    elif shape.shapeType == 4:
        # Build a rocket
        newShape = rocketModel.clone()
        newShape.position(shape.position[0], shape.position[1], shape.position[2])
        newShape.scale(shape.radius*.01, shape.radius*.01, shape.radius*.01)
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
screenshotNumber = 0
while DISPLAY.loop_running():
    sensorData.update()
    CAMERA.reset()
    for shape in pi3dShapes:
        shape.draw(camera=CAMERA)
    for text in objectNumbers:
        text.draw(camera=CAMERA)
        
    
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

    # Draw yaw, pitch and roll if in debug mode
    if DEBUG:
        displayString = "Yaw: {0:.2f}\nPitch: {1:.2f}\nRoll: {2:.2f}".format(cameraY, cameraX, cameraZ)
        positionInfo = pi3d.FixedString("fonts/FreeMonoBoldOblique.ttf", displayString, camera=TEXT_CAMERA, color=(255,255,255,255), font_size=18, margin=0.0, justify='R', background_color=(0,0,0,255), shader=shader, f_type='SMOOTH')
        positionInfo.sprite.positionX(300)
        positionInfo.sprite.positionY(-300)
        positionInfo.draw()

    gc.collect()

    k = mykeys.read()
    
    if k == 27:
        mykeys.close()
        if USE_CAMERA:
            piCamera.stop_preview()
        DISPLAY.destroy()
        break
    elif k == 10:
        fileName = 'screenshot' + str(screenshotNumber) + '.jpg'
        pi3d.screenshot(fileName)
        logging.info('Created screenshot with name: {}'.format(fileName))
        if USE_CAMERA:
            fileName = 'camera' + str(screenshotNumber) + '.jpg'
            piCamera.capture(fileName)
            logging.info('Took picture with name: {}'.format(fileName))
        screenshotNumber += 1
