# *-* coding: utf-8 *-*

import numpy as np
import random as rd
from shapely.geometry import Polygon,Point

import matplotlib.pyplot as plt
showfig = False
nfig = 1

def get_random_point_in_polygon(poly):
    minx, miny, maxx, maxy = poly.bounds
    while True:
        p = Point(rd.uniform(minx, maxx), rd.uniform(miny, maxy))
        if poly.contains(p):
            return p

with open("polar_bear_init.txt", 'r') as file:
    lines = file.read().splitlines()

    subsets_of_points = []
    subsets_of_points.append(np.zeros((40,2)))
    nsubsets = 0
    subsets_hull_length = []
    counter  = 0
    for ll in lines[:-1]:
        if ll=='':
            subsets_hull_length.append(counter)
            subsets_of_points.append(np.zeros((40,2)))
            nsubsets += 1
            counter   = 0
        else:
            subsets_of_points[nsubsets][counter] = np.array(ll.split(','), dtype='float64')
            counter += 1

    subsets_hull_length.append(counter)
    nsubsets += 1

plt.figure(nfig)
nfig += 1
for (i,nhull) in enumerate(subsets_hull_length):
    poly = Polygon(subsets_of_points[i][:nhull,:])
    x,y = poly.exterior.xy
    plt.plot(x,y)

    for j in range(nhull,40):
        new_point = get_random_point_in_polygon(poly)
        x,y = new_point.coords[0]
        subsets_of_points[i][j,0] = x
        subsets_of_points[i][j,1] = y

    x = subsets_of_points[i][:,0]
    y = subsets_of_points[i][:,1]
    plt.scatter(x,y,s=5)

data = np.zeros((nsubsets*40,2))
for i in range(nsubsets):
    data[i*40:(i+1)*40] = subsets_of_points[i]

data[:,0] /= data[:,0].max()
data[:,1] /= data[:,1].max()
data[:,0] -= np.mean(data[:,0])
data[:,1] -= np.mean(data[:,1])
data *= 2.

plt.figure(nfig)
plt.scatter(data[:,0], data[:,1], s=5)
nfig += 1

file = open("./polar_bear.txt", "w")
for point in data:
    x,y = point[0],point[1]
    file.write("%.5f,%.5f\n" %(x,-y))
file.close()

if showfig:
    plt.show()
