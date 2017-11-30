import numpy as np
import sys
import math
#import matplotlib.pyplot as plt
from pyproj import Proj
import time
#proj_init = Proj(init='epsg:32756')
proj_init = Proj(init='epsg:20356')


import matplotlib
from pylab import *

rcParams['pdf.use14corefonts'] = True
rcParams['font.family'] = 'serif'
rcParams['font.serif'] = 'Helvetica'
rcParams['ps.useafm'] = True
rcParams['axes.unicode_minus'] = False

rcParams['xtick.labelsize'] = '15'
rcParams['ytick.labelsize'] = '15'
rcParams['legend.fontsize'] = '15'
rcParams['legend.frameon'] = False
rcParams['axes.labelsize'] = '15'
rcParams['xtick.major.pad'] = '15'


class Timer:

    def __init__(self):
        self.start = None
        self.end = None

        self.times = []
        self.periods = []

    def tic(self, msg=None):

        curr = time.clock()
        self.times.append(curr)

        t = 0
        if not self.start:
            self.start = curr

        else:
            t = curr - self.start
            self.periods.append(t)
            self.start = curr

            if msg:
                print '%s:%f' % (msg, t)
        return t

    def print_times(self, msg=None):
        print msg
        print 'times:', self.periods

    def clear(self):
        self.start = None
        self.end = None

        self.times = []
        self.periods = []


def point_to_line_segment_dist(xA, yA, xB, yB, xC, yC, infi=False):
    # start, end, p
    v_AB = np.array([xB - xA, yB - yA])
    v_AC = np.array([xC - xA, yC - yA])
    v_BC = np.array([xC - xB, yC - yB])

    AB = np.linalg.norm(v_AB)
    AC = np.linalg.norm(v_AC)
    BC = np.linalg.norm(v_BC)

    point_to_line_segment_dist.counter += 1

    if not infi:
        if AB ** 2 + AC ** 2 <= BC ** 2:
            return AC
        elif AB ** 2 + BC ** 2 <= AC ** 2:
            return BC

    if np.abs(AB - 0) < 0.000000001:
        return AC
    else:
        return np.linalg.norm(np.cross(v_AB, v_AC)) / AB


point_to_line_segment_dist.counter = 0


def point_to_line_dist(s, e, x):
    d = point_to_line_segment_dist(s[0], s[1], e[0], e[1], x[0], x[1], infi=False)
    return d


def point_to_point_distance(p0, p1):
    return np.linalg.norm(p0 - p1)


def get_angle(p0, p1, to_positive=True):

    val_x0 = p0[0]
    val_y0 = p0[1]
    val_x1 = p1[0]
    val_y1 = p1[1]

    yy = val_y1 - val_y0
    xx = val_x1 - val_x0

    angles = math.atan2(yy, xx)

    if to_positive:
        if angles < 0:
            angles = 2 * math.pi + angles

    # print 'angle:', p0, p1, yy, xx, angles

    return angles  # , dists

def get_angle_vec(p0, p1):

    val_x0 = p0[:,0]
    val_y0 = p0[:,1]
    val_x1 = p1[:,0]
    val_y1 = p1[:,1]

    yy = val_y1 - val_y0
    xx = val_x1 - val_x0

    angles = np.arctan2(yy, xx)

    return angles  # , dists

def rotate_point(X, theta, p):

    if X is None:
        return None

    t0 = (X[0]-p[0]) *np.cos(theta) - (X[1]-p[1])*np.sin(theta) + p[0]
    t1 = (X[0]-p[0]) *np.sin(theta) + (X[1]-p[1])*np.cos(theta) + p[1]

    return np.array([t0,t1])

def rotate_points(X, theta):

    if X is None:
        return None

    t0 = X[:,0] *np.cos(theta) - X[:,1]*np.sin(theta)
    t1 = X[:,0] *np.sin(theta) + X[:,1]*np.cos(theta)

    X[:,0] = t0
    X[:,1] = t1

    return X

def get_proj(s, e, x):
    r = point_to_point_distance(s, x)
    theta0 = get_angle(s, e)
    theta1 = get_angle(s, x)

    theta = theta0 - theta1
    r = r * math.cos(theta)

    proj_x = r * math.cos(theta) + s[0]
    proj_y = r * math.sin(theta) + s[1]

    return np.array([proj_x, proj_y])


def total_dist(X):

    d = X[1:, :] - X[0:-1, :]
    d = d ** 2
    s = np.sqrt(np.sum(d, axis=1))
    d = np.sum(s)

    return d

def sync_error(X,Y, age_decay_func=None, plot=False):
    reconstructed=np.zeros([0,2])

    for i in np.arange(1, X.shape[0]):
        s,e = X[i-1:i+1,2]
        #print s,e
        xs = np.linspace(X[i-1,0], X[i,0],e-s+1)
        ys = np.linspace(X[i-1,1], X[i,1],e-s+1)


        if i == 1:
            new = np.vstack([xs,ys]).T
        else:
            new = np.vstack([xs,ys]).T[1:,:]

        reconstructed = np.vstack([reconstructed,new])
        #print reconstructed.shape[0]

    errs = np.linalg.norm(reconstructed-Y, axis=1)

    if age_decay_func != None:
        errs = errs * age_decay_func(np.arange(Y.shape[0]))[::-1]

    if plot:
        #plt.plot(range(Y.shape[0]),errs)

        plt.plot(range(Y.shape[0]),age_decay**np.arange(Y.shape[0])[::-1])
        plt.show()
    err = np.mean(errs)
    return err, errs



def insert_unique_point(arr, p, tolerance):
    flag = False
    for i in arr:
        if abs(i[0] - p[0]) < tolerance and abs(i[1] - p[1]) < tolerance:
            flag = True

    if not flag:
        arr.append(p)
    return arr

def cart2pol(x, y):
    rho = np.sqrt(x**2 + y**2)
    phi = np.arctan2(y, x)
    return(rho, phi)

def pol2cart(rho, phi):
    x = rho * np.cos(phi)
    y = rho * np.sin(phi)
    return(x, y)

if __name__ == '__main__':
    print point_to_line_segment_dist(0, 0, 1, 0, 5, 1)
    print point_to_line_segment_dist(0, 0, 1, 0, 5, 1, False)
