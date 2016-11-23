# Shamelessly retrieved from answers.ros.org/question/196149/how-to-rotate-vector-by-quaternion-in-python

import transformations

def qv_mult(q1, v1):
    v1 = transformations.unit_vector(v1)
    q2 = list(v1)
    q2.append(0.0)
    return transformations.quaternion_multiply(
            transformations.quaternion_multiply(q1, q2),
            transformations.quaternion_conjugate(q1)
    )[:3]
