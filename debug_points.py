import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import fileinput

_x = []
_y = []
_z = []

fig = plt.figure()

ax = Axes3D(fig)

ax.set_xlim3d(-2500, 2500)
ax.set_ylim3d(-2500, 2500)
ax.set_zlim3d(-2500, 2500)

for line in fileinput.input():
    #words of line should now contain x y z
    words_of_line = line.split()
    x = float(words_of_line[0])
    y = float(words_of_line[1])
    z = float(words_of_line[2])
    _x.append(x)
    _y.append(y)
    _z.append(z)

ax.scatter(_x, _y, _z, s = .1)
plt.show()
