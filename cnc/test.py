from matplotlib.patches import Circle
import math
import numpy as np
x = np.arange(20)

y = np.where(True,math.sqrt( 9 - math.pow(x-1) )+1,0 )


x = np.linspace(-1, 1, 500)
y1 = np.sqrt(1-x**2)
y2 = -np.sqrt(1-x**2)

y= math.sqrt( 9 - math.pow(x-1) )+1
y = x**2
