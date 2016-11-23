from sense_hat import SenseHat
from angle2dcm import angle2dcm
import math
from os import system

DEBUG = False
G = -9.81

with open('pi_at_rest.csv', 'wb') as f:
    f.write(b'accX,accY,accZ' + b'\n')

previousXAccel = None
previousYAccel = None
previousZAccel = None

while True:
    sense = SenseHat()
    acc = sense.get_accelerometer_raw()
    orien = sense.get_orientation_radians()
    orienDeg = sense.get_orientation()

    if DEBUG:
        print("Z Accel:            {}".format(acc['z']))
        print("Cosine of yaw:      {}".format(math.cos(orien['yaw'])))
        print("Cosine of roll:     {}".format(math.cos(orien['roll'])))
        print("Sine of yaw:        {}".format(math.sin(orien['yaw'])))
        print("Sine of roll:       {}".format(math.sin(orien['roll'])))
        print("Gyro Yaw (rad):     {}".format(orien['yaw']))
        print("Gyro Yaw (deg):     {}".format(orienDeg['yaw']))
        print("--------------------------------------------------------------------")


#    cosineMatrix = angle2dcm(orien['yaw'], orien['pitch'], orien['roll'])

    cosineAlpha = math.cos(orien['pitch'])
    cosineBeta = math.cos(orien['yaw'])
    cosineGamma = math.cos(orien['roll'])
    sineAlpha = math.sin(orien['pitch'])
    sineBeta = math.sin(orien['yaw'])
    sineGamma = math.sin(orien['roll'])

    transformedGravityVector = []
    transformedGravityVector.append(-G * sineBeta)
    transformedGravityVector.append(G * cosineBeta * sineGamma)
    transformedGravityVector.append(G * cosineBeta * cosineGamma)

    transformedAccelerationX = (acc['x'] * 9.81) - transformedGravityVector[0]
    transformedAccelerationY = (acc['y'] * 9.81) - transformedGravityVector[1]
    transformedAccelerationZ = (acc['z'] * 9.81) - transformedGravityVector[2]

    if previousXAccel == None:
        previousXAccel = transformedAccelerationX
        previousYAccel = transformedAccelerationY
        previousZAccel = transformedAccelerationZ
        continue

    xMovement = ''
    yMovement = ''
    zMovement = ''
    if abs(previousXAccel - transformedAccelerationX) > 4.04152002:
        xMovement = 'X'
    if abs(previousYAccel - transformedAccelerationY) > 0.01798845:
        yMovement = 'X'
    if abs(previousZAccel - transformedAccelerationZ) > 1.90756904:
        zMovement = 'X'

    print("X Movement: {}".format(xMovement))
    print("Y Movement: {}".format(yMovement))
    print("Z Movement: {}".format(zMovement))

    previousXAccel = transformedAccelerationX
    previousYAccel = transformedAccelerationY
    previousZAccel = transformedAccelerationZ

    system('clear')


    #if not DEBUG:
        #print(transformedAccelerationX)
        #print(transformedAccelerationY)
        #print(transformedAccelerationZ)
        #print("---------------------------------------------------------------------")
