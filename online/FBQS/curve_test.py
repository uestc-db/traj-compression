

#import curve_approximation_bk
from curve_approximation import *

from csvfile_read import *
import sys
import math
import pylab as pl
from pyproj import Proj
proj_init = Proj(init='epsg:32756')
import time
from pylab import *
import pylab

rcParams['pdf.use14corefonts'] = True
rcParams['font.family'] = 'serif'
rcParams['font.serif'] = 'Helvetica'
rcParams['ps.useafm'] = True
rcParams['axes.unicode_minus'] = False

rcParams['xtick.labelsize'] = '6'
rcParams['ytick.labelsize'] = '6'
rcParams['legend.fontsize'] = 'small'
rcParams['legend.frameon'] = False
rcParams['axes.labelsize'] = 'small'
rcParams['xtick.major.pad'] = '15'


def duty_cycle(n, k):
    step = int(math.ceil(float(n) / (k + 2)))
    # print n, step
    idx = range(0, n, step)

    if len(idx) < k + 2:
        idx.insert(len(idx) - 1, n - 2)

    if len(idx) < k + 2:
        if idx[-1] != n - 1:
            idx = idx + [n - 1]

    if len(idx) == k + 2:
        if idx[-1] != n - 1:
            idx[-1] = n - 1
    return idx


def dead_reckoning(x, y, k, angle_thresh=90, dist_thresh=100):
    N = len(x)
    angles = get_angles(x, y, 0)
    print 'angles', angles

    pre_angle = angles[0]
    acc_angle = 0
    solution = [0]

    ref_point_idx = 0
    for i in range(0, N - 1):

        diff = angles[i] - pre_angle

        if diff < 0.1:
            diff = 0
        acc_angle += diff
        pre_angle = angles[i]

        print 'angle', acc_angle

        # great angle change + dist
        if math.fabs(acc_angle) >= angle_thresh:

            dist = np.linalg.norm(
                np.array([x[i + 1] - x[ref_point_idx], y[i + 1] - y[ref_point_idx]]))

            if dist >= dist_thresh:
                solution.append(i + 1)
                ref_point_idx = i + 1
                print 'sample', i + 1
                if len(solution) == k + 1:
                    break
            acc_angle = 0

    solution.append(N - 1)

    return solution


def event_trigger(n, k):
    kk = k
    kk = min(n - 2, kk)
    idx = [0] + range(1, 1 + kk) + [n - 1]
    return idx


def sub_sample(data, rate):
    n = data.shape[0]
    idx = range(0, n, rate)
    sdata = data[idx, :]
    return sdata


def test_process(fname, n, ms):
    data = readcsvfile(fname + '.txt')

    if n < 0:
        step = -n
        data = sub_sample(data, step)  # [1:data.shape[0], :]
        n = data.shape[0]
    else:
        n = min(n, data.shape[0])
        data = data[0:n, :]

    data = np.array(data)
    # print data

    x = data[:, 0]
    y = data[:, 1]

    dmap = build_dist_map(x, y)
    for m in ms:

        t0 = time.clock()
        solution, dmax, max_id, status = curve_approximation(x, y, m, dmap)
        t1 = time.clock()
        solution1, dmax1, max_id1, status = curve_approximation_bk.curve_approximation(
            x, y, m)
        t2 = time.clock()

        print solution, dmax, max_id
        print solution1, dmax1, max_id1
        print t1 - t0, t2 - t1


def extract_data(fname, n, proj=True):
    data = readcsvfile(fname + '.txt')
    print 'loading data file', fname + '.txt'
    if n < 0:
        step = -n
        data = sub_sample(data, step)  # [1:data.shape[0], :]
        n = data.shape[0]
    else:
        n = min(n, data.shape[0])
        data = data[0:n, :]

    if proj:
        data = [list(proj_init(x[0], x[1])) for x in data]
    #    print 'after', data
    data = np.array(data)

    return data, n


def test_once(fname, data, m, dmap=None):
    data = np.array([list(proj_init(x[0], x[1])) for x in data])

    x = data[:, 0]
    y = data[:, 1]
    n = len(x)

    solution, dmax, max_id, status = curve_approximation(x, y, m, dmap)

    sln1 = duty_cycle(n, m)
    sln2 = rfd_duty_cycle(x, y, m)

    print sln2, len(sln2)

    dmax_check, max_id_check = curve_approx_dist(
        x, y, solution)

    dmax_check1, max_id_check1 = curve_approx_dist(
        x, y, sln1)

    dmax_check2, max_id_check2 = curve_approx_dist(
        x, y, sln2)

    writecsvfile(np.array([solution]), '%s-%d-%d.txt' % (fname, m, n))

    res = [dmax_check, dmax_check1, dmax_check2]

    # print solution
    print res

    fig, ax = pl.subplots()

    tmp = [0.5, 1.5, 2.5]
    ax.bar(tmp, res, width=1)

    plt.xticks(tmp, ['Optimum', 'DutyCycle', 'RefinedDutyCycle'])
    # pl.show()
    plt.savefig('%s-test-errs-%d-%d.pdf' % (fname, m, n))


def process_once(fname, data, m, dmap=None, epsilon=10):

    x = data[:, 0]
    y = data[:, 1]
    n = len(x)

    solution, dmax, max_id, status = curve_approximation(x, y, m, dmap)

    sln1 = duty_cycle(n, m)
    sln2 = event_trigger(n, m)
    sln2 = list(DouglasPeucker(data[:, 0:2.5], range(0, n), epsilon)[0])

    print sln2, len(sln2)

    dmax_check, max_id_check = curve_approx_dist(
        x, y, solution)

    dmax_check1, max_id_check1 = curve_approx_dist(
        x, y, sln1)

    dmax_check2, max_id_check2 = curve_approx_dist(
        x, y, sln2)

    writecsvfile(np.array([solution]), '%s-%d-%d.txt' % (fname, m, n))

    res = [dmax_check, dmax_check1, dmax_check2]

    # print solution
    # print res

    fig, ax = pl.subplots()

    tmp = [0.5, 1.5, 2.5]
    ax.bar(tmp, res, width=1)

    plt.xticks(tmp, ['Optimum', 'DutyCycle', 'EventTriggered'])
    # pl.show()
    plt.savefig('%s-errs-%d-%d.pdf' % (fname, m, n))


def process_multi(fname, data, ms, dmap, idx):

    x = data[:, 0]
    y = data[:, 1]
    n = len(x)

    ds = []
    for m in ms:
        plt.figure(idx)
        solution, dmax, max_id, status = curve_approximation(x, y, m, dmap)

        sln1 = duty_cycle(n, m)
        #sln2 = event_trigger(n, m)

        dmax_check, max_id_check = curve_approx_dist(
            x, y, solution)

        dmax_check1, max_id_check1 = curve_approx_dist(
            x, y, sln1)

        # dmax_check2, max_id_check2 = curve_approx_dist(
        #    x, y, sln2)

        writecsvfile(np.array([solution]), '%s-%d-%d.txt' % (fname, m, n))

        ds.append([dmax_check, dmax_check1])

    ds = np.array(ds)

    l0, = plt.plot(ms, ds[:, 0], '-.*', markersize=15, linewidth=2)
    l1, = plt.plot(ms, ds[:, 1], '-.+', markersize=15, linewidth=2)
    #l2, = plt.plot(ms, ds[:, 2], '-.x', markersize=10)

    plt.legend([l0, l1], ['Optimal', 'Duty Cycle'])

    plt.xlabel('Number of Samples Selected')
    plt.ylabel('Error (m)')
    plt.subplots_adjust(left=-1)
    plt.tight_layout()

    plt.savefig('%s-all-errs-%d-%d.pdf' % (fname, m, n), bbox_inches='tight')


def writemap(dmap, fname):
    writecsvfile(dmap['E'], '%s-E.map' % fname)
    writecsvfile(dmap['E_id'], '%s-E_id.map' % fname)
    writecsvfile(np.array([dmap['e_list']]), '%s-e_list.map' % fname)


def readmap(fname):
    E = readcsvfile('%s-E.map' % fname)
    E_id = readcsvfile('%s-E_id.map' % fname)
    e_list = readcsvfile('%s-e_list.map' % fname)
    dmap = {'E': E, 'E_id': E_id, 'e_list': list(e_list[0, :])}
    return dmap

if __name__ == '__main__':

    #test_process('../data/gpsout-163', -20, [5, 10, 20])

    # n171: 3040
    # n163: 1017

    # fname = '../data/gpsout-163-commute'
    # data = extract_data(fname, 200)
    # dmap = readmap('163commute')
    # process_once(fname, data, 10, dmap, 45)

    fname = '../data/gpsout-163'
    data = extract_data(fname, 1000)
    dmap = readmap('163')
    process_multi(fname, data, [5, 10, 20, 50, 100, 200], dmap, 1)

    #test_once(fname, data, 22, dmap)

    #x = np.array([0, 0.5, 1, 0.5, 0, -0.5, -1, -0.5, 0])
    #y = np.array([1, 0.5, 0, -0.5, -1, -0.5, 0, 0.5, 1])
    # print get_angles(x, y)

    #dmap = build_dist_map(data[:, 0], data[:, 1])
    #writemap(dmap, '163commute')
    #dmap = build_dist_map(data[:, 0], data[:, 1])
    # process_multi(fname, data, [5, 10, 20, 50, 100, 200], dmap)
    #writecsvfile(dmap, '163.map')
    #writemap(dmap, '163')
    fname = '../data/gpsout-171'
    data = extract_data(fname, -3)
 #   data = np.array([list(proj_init(x[0], x[1])) for x in data])
    #dmap = build_dist_map(data[:, 0], data[:, 1])
    #dmap = readcsvfile('171.map')

    dmap = readmap('171')
    process_multi(fname, data, [5, 10, 20, 50, 100, 200], dmap, 2)
    #process_once(fname, data, 22, dmap, 2000)
    #writemap(dmap, '171')

    # for i in [5, 10, 20, 50]:






    # import time
    # t0 = time.clock()
    # fname = '../data/gpsout-163'
    # process_once(fname, -10, 3)
    # t1 = time.clock()
    # print t1 - t0









    # fname = '../data/gpsout-173'
    # process_once(fname, -2, 5)
    # t2 = time.clock()
    # print t2 - t1
