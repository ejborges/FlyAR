import numpy as numpy
import numpy as np
from math import sin, cos, tan, asin, acos, atan2, fabs, sqrt

def angle2dcm(yaw, pitch, roll, input_units='rad', rotation_sequence='321'):
    """
    Returns a transformation matrix (aka direction cosine matrix or DCM) which 
    transforms from navigation to body frame.  Other names commonly used, 
    besides DCM, are `Cbody2nav` or `Rbody2nav`.  The rotation sequence 
    specifies the order of rotations when going from navigation-frame to 
    body-frame.  The default is '321' (i.e Yaw -> Pitch -> Roll).

    Parameters
    ----------
    yaw   : yaw angle, units of input_units.
    pitch : pitch angle, units of input_units.
    roll  : roll angle , units of input_units.
    input_units: units for input angles {'rad', 'deg'}, optional.
    rotationSequence: assumed rotation sequence {'321', others can be 
                                                implemented in the future}.

    Returns
    -------
    Rnav2body: 3x3 transformation matrix (numpy matrix data type).  This can be
               used to convert from navigation-frame (e.g NED) to body frame.
        
    Notes
    -----
    Since Rnav2body is a proper transformation matrix, the inverse
    transformation is simply the transpose.  Hence, to go from body->nav,
    simply use: Rbody2nav = Rnav2body.T

    Examples:
    ---------
    >>> import numpy as np
    >>> from nav import angle2dcm
    >>> g_ned = np.matrix([[0, 0, 9.8]]).T # gravity vector in NED frame
    >>> yaw, pitch, roll = np.deg2rad([90, 15, 0]) # vehicle orientation
    >>> g_body = Rnav2body * g_ned
    >>> g_body
    matrix([[-2.53642664],
            [ 0.        ],
            [ 9.4660731 ]])
            
    >>> g_ned_check = Rnav2body.T * g_body 
    >>> np.linalg.norm(g_ned_check - g_ned) < 1e-10 # should match g_ned
    True

    Reference
    ---------
    [1] Equation 2.4, Aided Navigation: GPS with High Rate Sensors, Jay A. Farrel 2008
    [2] eul2Cbn.m function (note, this function gives body->nav) at:
    http://www.gnssapplications.org/downloads/chapter7/Chapter7_GNSS_INS_Functions.tar.gz
    """
    # Apply necessary unit transformations.
    if input_units == 'rad':
        pass
    elif input_units == 'deg':
        yaw, pitch, roll = np.radians([yaw, pitch, roll])

    # Build transformation matrix Rnav2body.
    s_r, c_r = sin(roll) , cos(roll)
    s_p, c_p = sin(pitch), cos(pitch)
    s_y, c_y = sin(yaw)  , cos(yaw)

    if rotation_sequence == '321':
        # This is equivalent to Rnav2body = R(roll) * R(pitch) * R(yaw)
        # where R() is the single axis rotation matrix.  We implement
        # the expanded form for improved efficiency.
        Rnav2body = np.matrix([
                [c_y*c_p               ,  s_y*c_p              , -s_p    ],
                [-s_y*c_r + c_y*s_p*s_r,  c_y*c_r + s_y*s_p*s_r,  c_p*s_r],
                [ s_y*s_r + c_y*s_p*c_r, -c_y*s_r + s_y*s_p*c_r,  c_p*c_r]])

    else: 
        # No other rotation sequence is currently implemented
        print('WARNING (angle2dcm): requested rotation_sequence is unavailable.')
        print('                     NaN returned.')
        Rnav2body = np.nan
    
    return Rnav2body