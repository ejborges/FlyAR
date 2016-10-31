---
layout: post
---
{% include JB/setup %}

A primary component of AR is to display virtual objects in the same field of view as the real world.  When moving in the real world, these virtual objects should move with the real world as if
the virtual objects are physically there. This may sound simple in practice; however distance traveled in the real world could be in feet, meters, degrees, minutes, seconds, etc, while in the
virtual world, distances are defined in arbitrary units. Many AR applications use some sort of placeholder object (such as an AR card) to know where the object should be moved to; however our
goal is to not need any of these placeholder objects, so we need a way to convert real-world coordinates to virtual units.

To accomplish this, I set up a small program that allows me to move a 3D wireframe cube along the X, Y, and Z axes with varying "deltas".  This delta value is the distance the virtual object
traveled each time I hit either the W, S, A, D, Q, E, up arrow, or down arrow keys. Why so many keys? Each key serves a different function:

| Key | Function |
|-----|----------|
| W   | Move cube up |
| S   | Move cube down |
| A   | Move cube left |
| D   | Move cube right |
| Q   | Move cube forward |
| E   | Move cube backward |
| Up arrow | Scale cube (incr.) |
| Down arrow | Scale cube (decr.) |

To start, I used a basic Rubik's cube as the real-world object, as it is essentially a perfect cube (the only way it *wouldn't* be is if the cube was incorrectly manufactured). I then aligned the
virtual cube to line up with the real cube.  Next, the real world cube was moved a known distance either back, forward, left, or right.  I then slowly moved the virtual cube only in the direction
I moved the real cube until the two lined up again. The value of the virtual cube was then recorded in Excel, along with the displacement of the real cube. This was repeated for several trials to
get data in all three axes. A collection of the data is below.

| Real Object X (in.) | Real Object Y (in.) | Real Object Z (in.) ||| Virtual Object X | Virtual Object Y | Virtual Object Z |
|---------------------|---------------------|---------------------|-|------------------|------------------|------------------|
| 0                   | 1                   | 0                   ||| 0.366            | 0.037            | 6.348            |
| 0                   | 1                   | 6                   ||| 0.366            | 0.037            | 8.298            |
| 0                   | 1                   | 12                  ||| 0.366            | 0.037            | 10.323           |
| -2                  | 1                   | 0                   ||| -0.373           | 0.037            | 6.348            |
| -4                  | 1                   | 0                   ||| -1.12            | 0.037            | 6.348            |
| -6                  | 1                   | 0                   ||| -1.86            | 0.037            | 6.348            |
| 2                   | 1                   | 0                   ||| 1.113            | 0.037            | 6.348            |
| 4                   | 1                   | 0                   ||| 1.856            | 0.037            | 6.348            |
| 0                   | 3.75                | 0                   ||| 0.366            | 1.042            | 6.348            |
| 0                   | -1.75               | 0                   ||| 0.366            | -0.968           | 6.348            |
