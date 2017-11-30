#
#
# This program calculates the optimal approximating curve to the original curve.
# The given approximating curve has a global minimized maxmimum distance to the original curve.
# Suppose the original curve has N points.
# The approximating curve has m + 2 points (or sometimes less than m + 2) and m is assigned by the user (m <= N - 2).
# The time complexity is O(N^3) and the space complexity is O(N^2).
#
# The function 'solution, dmax, status = curve_approximation(x, y, m)' has the following inputs and outputs:
#
#      x: an array of size N, the x coordinates of the orignal curve
#      y: an array of size N, the y coordinates of the orignal curve
#      m: the maximum number of sampling points (not including the first and the last points of the original curve)
#
#      solution: a list of point index characterizing the approximating curve (including the first and the last points of the original curve)
#                For example, the returned 'solution' may be [0, 3, 4, 9] for m = 2 and N = 10, which means the approximating curve is formed by the 0th, 3rd, 4th, 9th points of the orignal curve.
#      dmax: the minimized maximum distance
#      max_id: the index of the furthest point of the original curve
#      status:  some interim variables of the algorithm for debugging use
#
#
#


import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
import copy
import datetime
from utils import point_to_line_segment_dist, get_angle_vec, get_angle
import math
# def point_to_line_segment_dist(xA, yA, xB, yB, xC, yC, infi=True):
# start, end, p
#     v_AB = np.array([xB - xA, yB - yA])
#     v_AC = np.array([xC - xA, yC - yA])
#     v_BC = np.array([xC - xB, yC - yB])

#     AB = np.linalg.norm(v_AB)
#     AC = np.linalg.norm(v_AC)
#     BC = np.linalg.norm(v_BC)

#     if not infi:
#         if AB ** 2 + AC ** 2 <= BC ** 2:
#             return AC
#         elif AB ** 2 + BC ** 2 <= AC ** 2:
#             return BC

#     return np.linalg.norm(np.cross(v_AB, v_AC)) / AB


def curve_approx_dist(x, y, s_index, proj=False):

    x = list(x)
    y = list(y)
    Ns = len(s_index)
    dmax = 0.0
    max_id = 0
    ranges = None
    for i in range(Ns - 1):
        for j in range(s_index[i] + 1, s_index[i + 1]):
            d = point_to_line_segment_dist(
                x[s_index[i]], y[s_index[i]], x[s_index[i + 1]], y[s_index[i + 1]], x[j], y[j])
            # print s_index[i], s_index[i+1], j, d
            if d > dmax:
                dmax = d
                max_id = j
                ranges = (s_index[i],s_index[i+1]) 
    return dmax, max_id, ranges


def min_num_approx(E, N, epsilon):
    G = nx.DiGraph()
    for i in range(N - 1):
        for j in range(i + 1, N):
            if E[i, j] <= epsilon:
                G.add_edge(i, j)
    return nx.shortest_path(G, 0, N - 1)


def build_dist_map(x, y):
    x = list(x)
    y = list(y)
    N = len(x)
    E = np.zeros((N, N))
    E_id = np.zeros((N, N))
    e_list = []

    for i in range(N - 2):
        for j in range(i + 2, N):

            dmax = 0.0
            max_id = 0
            for k in range(i + 1, j):
                # print 'i:%d, j:%d, k:%d' % (i, j, k)
                d = point_to_line_segment_dist(
                    x[i], y[i], x[j], y[j], x[k], y[k])
                # print d
                if d > dmax:
                    dmax = d
                    max_id = k
            E[i, j] = dmax
            E_id[i, j] = max_id
            e_list.append(dmax)
            # print 'i:%d, j:%d, dmax:%.2f' % (i, j, dmax)
    e_list.sort()
    map_vars = {'E': E, 'E_id': E_id, 'e_list': e_list}
    return map_vars


def curve_approximation(x, y, m, map_vars={}):

    N = len(x)

    if m >= N - 2:
        return range(N), 0.0, None, [np.zeros((N, N)), np.zeros((N, N)), []]

    if not map_vars:
        map_vars = build_dist_map(x, y)

    E = map_vars['E']
    E_id = map_vars['E_id']
    e_list = map_vars['e_list']

    # print 'binary:'
    low = 0
    high = len(e_list) - 1
    solution = []

    while low <= high:

        mid = int((low + high) / 2)
        path = min_num_approx(E, N, e_list[mid])
        # print low, mid, high, path, len(path), e_list[mid]
        if len(path) == m + 2:
            solution = path
            break

        elif len(path) < m + 2:
            high = mid - 1
            solution = path

        else:
            low = mid + 1

    # print 'left most minimum:'
    min_k = len(solution)
    if min_k == m + 2:
        high = mid
    low = 0

    while low < high:
        mid = int((low + high) / 2)
        # print low, mid, high
        path = min_num_approx(E, N, e_list[mid])
        if len(path) == min_k:
            high = mid
            solution = path

        else:
            low = mid + 1

    dmax = 0.0
    fp_id = 0
    for i in range(1, len(solution)):
        p = solution[i - 1]
        q = solution[i]
        if E[p, q] > dmax:

            dmax = E[p, q]
            fp_id = E_id[p, q]

    # print 'iterate:'
    # for i in range(len(e_list)):
    #    path = min_num_approx(E, N, e_list[i])
    #    print i, path, e_list[i]

    # print 'The minimum error (maximum distance) for %d samplings is %.8lf' %
    # (min_k -2, dmax_optimum)
    return solution, dmax, fp_id, [E, E_id, e_list]


def shortestDistanceToSegment(point, line_s, line_e):

    d = point_to_line_segment_dist(
        line_s[0], line_s[1], line_e[0], line_e[1], point[0], point[1])

    return d


def DouglasPeucker(PointList, ids, epsilon):
    # Find the point with the maximum distance
    # print 'PointList:', PointList
    dmax = 0
    index = 0
    end = len(ids)
    for i in range(1, end):
        d = shortestDistanceToSegment(
            PointList[ids[i], :], PointList[ids[0],:], PointList[ids[-1],:])
        if d > dmax:
            index = i
            dmax = d

    # If max distance is greater than epsilon, recursively simplify
    if dmax > epsilon:
        # Recursive call
        recResults1, dmax1 = DouglasPeucker(
            PointList, ids[0:(index + 1)], epsilon)
        recResults2, dmax2 = DouglasPeucker(
            PointList, ids[index:end], epsilon)

        # Build the result list
        # print 'recResults1', recResults1
        # print 'recResults2', recResults2
        ResultList = np.concatenate((recResults1[0:-1], recResults2[:]))
        dmax = max(dmax, dmax1, dmax2)
    else:
        ResultList = np.array([ids[0], ids[-1]])

    # Return the result
    return ResultList, dmax


def buffered_dp(X, eps, buff_size):

    N = X.shape[0]

    res = [0]

    i = 0
    e = 1
    while e < N:
        e = i + buff_size
        if e >= N:
            e = N

        ids = np.arange(min(buff_size, e - i))
        r, d = DouglasPeucker(X[i:e, :], ids, eps)

        r += i
        # print r
        res += list(r[1:])
        i = e - 1

    return res


def buffered_greedy(X, eps, buff_size):
    N = X.shape[0]
    res = [0]

    s = 0
    e = 2
    while e < N:

        d, id = curve_approx_dist(X[s:e, 0], X[s:e, 1], [0, e - s - 1])
        # print s, e, d

        if d <= eps and e - s < buff_size:
            e += 1
        else:
            res += [e - 2]
            s = e - 2

    return res


def dead_reckoning(X, eps):

    n = X.shape[0]
    d = X[1:, :] - X[0:-1, :]
    d = d ** 2
    d = np.sqrt(np.sum(d, axis=1))

    ids=[]
    spd = d

    angles = get_angle_vec( X[0:-1, :], X[1:, :] )
    #print X,n,angles[10:100]
    #print d[10:100]
    print 'max_spd', max(d)

    d_on_x  = 0.0

    max_d = 0.0
    ids = [0]
    start_idx = 0
    d_on_x = d[0]

    for i in xrange(2, n):
        max_d += abs(d[i-1]* math.sin(angles[i-1]-angles[start_idx]))

        #d_on_x += d[i-1]*math.cos(angles[i-1]-angles[start_idx-1])

        #theta = get_angle(X[start_idx],X[i])
        #max_d = math.sin(theta) * d_on_x

        #print d_on_x, theta, max_d

        if abs(max_d) > eps:
            max_d = 0
            ids.append(i-1)
            start_idx = i-1
            #d_on_x = d[start_idx]

        #print max_d

    if ids[-1] != n-1:
        ids.append(n-1)

    return ids


def uniform_sampling(X, eps ):
    print X[10:100,:]
    d = X[1:, :] - X[0:-1, :]
    d = d ** 2
    d = np.sqrt(np.sum(d, axis=1))

    spd = d

    print 'maxspd', max(spd)
    step = int(eps/max(spd))

    n = X.shape[0]
    if step == 0:
        return range(0,n)
    

    ids = range(0,n,step)
    if ids[-1] != n-1:
        ids.append(n-1)

    return ids


def DFS(begin, end, m, s_index):
    if m == 0:
        s_index.append(end - 1)
        # print np.unique(s_index)
        s_final = copy.deepcopy(s_index)
        d_max, max_id = curve_approx_dist(x, y, np.unique(s_index))
        # print d_max, max_id
        s_index.pop()
        return d_max, max_id, s_final
    else:
        d_max = float('inf')
        max_id = 0
        s_final = []

        for i in range(begin, end):
            s_index.append(i)
            d_temp, id_temp, s_temp = DFS(i, end, m - 1, s_index)

            if d_temp < d_max:
                d_max = d_temp
                max_id = id_temp
                s_final = s_temp

            s_index.pop()

        return d_max, max_id, s_final


def vec_reshape(v, unpack=False):
    if unpack:
        v = list(v[0])
    else:
        n = len(v)
        v = v.reshape(n, 1)
    return v


def get_noise(m, n, noise_cap=0):
    rseed = int((datetime.datetime.now() -
                 datetime.datetime(1970, 1, 1)).total_seconds())
    print rseed
    np.random.seed(rseed)

    if m == 1 and n == 1:
        return random.random() * noise_cap
    else:
        return np.random.rand(m, n) * noise_cap


def get_angles(x, y, noise_cap=0):
    N = len(x)
    val_x0 = x[0:-1]
    val_x1 = x[1:]
    val_y0 = y[0:-1]
    val_y1 = y[1:]

    yy = val_y1 - val_y0
    xx = val_x1 - val_x0

    noise = get_noise(N, 1, noise_cap)

    # print yy, len(yy)
    # print xx, len(xx)
    # print np.arctan2
    # print 'noise', noise
    angles = np.arctan2(list(yy), list(xx))  # + noise
    #dists = np.sqrt(xx * xx + yy * yy)

    return angles / np.pi * 180  # , dists


def get_dist(x, y):
    pass


def brute_force(x, y, m):
    N = len(x)
    s_index = [0]

    d_max, max_id, s_final = DFS(0, N, m, s_index)

    return d_max, max_id, list(np.unique(s_final))


if __name__ == '__main__':
    # N = 50
    # H = 5.0
    # m = 10
    # x = vec_reshape(np.arange(N))
    # y = vec_reshape(np.random.rand(N) * H)

    N = 50
    m=14
    a = np.arange(1,N+1,2)
    b = np.zeros(N/2)+0.5
    c = np.arange(2,N+2,2)
    d = np.zeros(N/2)+0.5

    x = np.vstack([a,b]).T.flatten()
    y = np.vstack([d,c]).T.flatten()

    X = np.vstack([x,y]).T

    # print 'x:', x
    # print 'y:', y
    solution, dmax, max_id, status = curve_approximation(x, y, m)

    # print solution, dmax, max_id
    dmax_check, max_id_check = curve_approx_dist(x, y, solution)
    print solution, len(solution), dmax_check
    # The red solid lines and red points indicate the original curve.
    #plt.plot(x, y, 'o-r')
    # The blue dashed lines and blue points indicate the approximating curve
    # and the optimal sampling points.
    #plt.plot(x[solution], y[solution], 'o--b', markersize=10.0)
    # The green point indicates the furthest point of the orignal curve to the
    # approximating curve.
    #plt.plot(x[max_id], y[max_id], 'o', color='green', markersize=10.0)
    #plt.xlim(0, max(N, H))
    #plt.ylim(0, max(N, H))
    # plt.show()

    data = np.concatenate((x, y), 1)
    #print 'data', data
    solution2, max_d = DouglasPeucker(X, np.arange(N), 5)
    print len(solution2)

    # dmax_check, max_id_check = curve_approx_dist(x, y, solution2)
    # print dmax_check

    # from curve_test import duty_cycle
    # ss = duty_cycle(N, m)
    # print ss
    # dmax_check, max_id_check = curve_approx_dist(x, y, ss)
    # print dmax_check


    # for N in range(1,15):
    #    for m in range(1,5):
    #        x = np.arange(N)
    #        y = np.random.rand(N) * H
    #
    #        solution, dmax, max_id = curve_approximation(x, y, m)
    #        dmax2, max_id2, solution2 = brute_force(x, y, m)
    #
    #        if dmax != dmax2 or solution != solution2:
    #            print '\n unusual detected: N = %d, m = %d' % (N, m)
    #            print 'input X: ',x
    #            print 'input Y: ',y
    #            print 'brute force solution:', solution2
    #            print 'minimum error:% .8lf, furthest point %d' % (dmax2, max_id2)
    #            print 'polynomial solution:', solution
    #            print 'minimum error:% .8lf' % (dmax, max_id)
    #
    #            dmax_check1, max_id_check1 = curve_approx_dist(x, y, solution)
    #            dmax_check2, max_id_check2 = curve_approx_dist(x, y, solution2)
    #
    #            print 'check brute force dmax %.8lf' % (dmax_check2)
    #            print 'check polynomial dmax %.8lf' % (dmax_check1)


    #dmax2, max_id2, solution2 = brute_force(x, y, 1)
    # print 'N = %d, m = %d \n' % (N,m)
    # print '\n brute force solution:', solution2
    # print 'minimum error:% .8lf' % (dmax2)
    # print '\n polynomial solution:', solution
    # print 'minimum error:% .8lf' % (dmax)
