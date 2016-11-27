# Handles calculating the proper acceleration of the Pi in each direction, removing the gravitational component
# TODO Make this better - still rough data coming back
import math

G = -9.81

def getFilteredAcceleration(xAccel, yAccel, zAccel, roll, pitch, yaw):
    '''
    Returns a 3 element array: [xFilteredAccel, yFilteredAccel, zFilteredAccel]
    '''

    cosineAlpha = math.cos(pitch)
    cosineBeta = math.cos(yaw)
    cosineGamma = math.cos(roll)
    sineAlpha = math.sin(pitch)
    sineBeta = math.sin(yaw)
    sineGamma = math.sin(roll)

    transformedGravityVector = []
    transformedGravityVector.append(-G * sineBeta)
    transformedGravityVector.append(G * cosineBeta * sineGamma)
    transformedGravityVector.append(G * cosineBeta * cosineGamma)

    transformedAccelerationX = (xAccel * 9.81) - transformedGravityVector[0]
    transformedAccelerationY = (yAccel * 9.81) - transformedGravityVector[1]
    transformedAccelerationZ = (zAccel * 9.81) - transformedGravityVector[2]

    return [transformedAccelerationX, transformedAccelerationY, transformedAccelerationZ]
