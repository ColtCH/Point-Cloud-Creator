import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

#Populated for 3D scatter graph.
x_coords = []
y_coords = []
z_coords = []

fig = plt.figure()
ax = Axes3D(fig)

filename = "debug.txt"
file = open(filename, "r")
for line in file:
    #words of line should now contain x y z
    words_of_line = line.split()
    x = float(words_of_line[0])
    y = float(words_of_line[1])
    z = float(words_of_line[2])
    x_coords.append(x)
    y_coords.append(y)
    z_coords.append(z)

ax.scatter(x_coords, y_coords, z_coords);
plt.show()
