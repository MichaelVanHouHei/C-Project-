import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.animation import FFMpegWriter
roadWidth = 2000
max_frame = 3600
fig = plt.figure(figsize=(6, 6))
plt.ion()
ax = plt.gca()
ax.grid()
ln1, = ax.plot([], [], 'ro')
ln2, = ax.plot([], [], 'ro', color='r')
Frames = []
# denote that each line is Frame \t id \t x \t y \t
file1 = open('xyzoutputAn.xyz', 'r')
Lines = file1.readlines()
cars = []
lastID = 0
for line in Lines:
    r = line.split('\t')
    frameID = int(r[1])
    x = float(r[2])
    y = float(r[3])
    cars.append((x, y))
    if lastID != frameID:
        Frames.append(cars)
        cars = []
        lastID = frameID
        max_frame = frameID
file1.close()


def init():
    global max_frame
    ax.set_xlim(-2, roadWidth)
    ax.set_ylim(-10, 20)
    plt.axline((0, -2), (roadWidth, -2))
    plt.axline((0, 2), (roadWidth, 2))
    plt.axline((0, 6), (roadWidth, 6))
    plt.axline((0, 10), (roadWidth, 10))
    plt.axline((0, 14), (roadWidth, 14))
    return ln1,


def update(i):
    print("generating frame:" + str(i))
    x_in = []
    y_in = []
    cars = Frames[i]
    for j in range(len(cars)-1):
        car = Frames[i][j]
        x_in.append(car[0])
        y_in.append(car[1])
    ln2.set_data(x_in, y_in)
    return ln2,


# 500 is how many frames you want to genearte
ani = animation.FuncAnimation(
    fig, update, range(max_frame), init_func=init, interval=30)
ani.save('roll1000.gif', writer='Pillow', fps=30)
#f = r"road.mp4"
#writer = FFMpegWriter(fps=15, metadata=dict(artist='Me'), bitrate=1800)
#ani.save("movie.mp4", writer=writer)
plt.show()
