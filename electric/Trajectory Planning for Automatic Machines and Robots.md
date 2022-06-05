

In this book, the following notation is adopted.
- One-dimensional trajectories:
    - $q(t)$ : position profile
    - $t$ : independent variable, that can be either the “time” (as normally assumed in the book) or the angular position θ of the
master in a system based on electronic cams
    - $q^{(1)}(t), q˙(t)$ : time-derivative of the position (velocity profile)
    - $q^{(2)}(t), q''(t)$ : time-derivative of the velocity (acceleration profile)
    - $ q(3)(t),q'''(t)$ : time-derivative of the acceleration (jerk profile)
    - $q(4)(t)$ : time-derivative of the jerk (snap, jounce or ping profile)
s(t) : spline function
qk(t) :k-th position segment (k = 0, . . . , n−1) in multi-segment trajectories
˜q(t) : reparameterization of q(t) (scaling in time), ˜q(t) = q(t) with
t = σ(t)