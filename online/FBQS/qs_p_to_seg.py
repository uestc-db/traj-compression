

import matplotlib
# matplotlib.use('TkAgg')
from utils import *
from curve_test import extract_data
from curve_approximation import curve_approx_dist, DouglasPeucker

from pylab import *
from csvfile_read import *

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




TOLERANCE = 0.001
DEBUG = False


def Print(s):
    if DEBUG:
        print s


class BoundingBox:

    def __init__(self, start):
        self.start = start
        self.max_x = None
        self.min_x = None
        self.max_y = None
        self.min_y = None

    def get_box(self):
        return [self.min_x, self.max_x, self.min_y, self.max_y]

    def get_corners(self, quadrant_idx=None):

        points = np.array(
            [[self.min_x, self.min_y], [self.max_x, self.min_y], [self.max_x, self.max_y], [self.min_x, self.max_y]])

        return points

    def add_point(self, p):

        # print 'adding to box', p
        if not self.min_x or p[0] < self.min_x:
            self.min_x = p[0]

        if not self.max_x or p[0] > self.max_x:
            self.max_x = p[0]

        if not self.min_y or p[1] < self.min_y:
            self.min_y = p[1]

        if not self.max_y or p[1] > self.max_y:
            self.max_y = p[1]

        # print 'added min/max', self.min_x, self.max_x, self.min_y, self.max_y

    def get_intersection(self, lines):

        lb_points = self.intersect_line(lines.lb_angle, lines.lb_angle_b)
        ub_points = self.intersect_line(lines.ub_angle, lines.ub_angle_b)

        if len(lb_points) == 0 or len(ub_points) == 0:
            print lines.lb_angle, lines.ub_angle, self.get_box()

        return (lb_points, ub_points)

    def p_in_box(self, p):
        if p[0] <= self.max_x + TOLERANCE and p[0] >= self.min_x - TOLERANCE and p[1] <= self.max_y + TOLERANCE and p[1] >= self.min_y - TOLERANCE:
            return True

        return False

    def intersect_line(self, angle, b, strict=False):

        inter_points = []

        for i in [self.min_x, self.max_x]:
            p = self.intersect_vertical(angle, b, i, self.min_y, self.max_y)

            if p is not None:
                if strict and (self.p_in_box(p) == False):
                    continue
                insert_unique_point(inter_points, p, TOLERANCE)

        for i in [self.min_y, self.max_y]:
            p = self.intersect_horizontal(angle, b, i, self.min_x, self.max_x)

            if p is not None:
                if strict and (self.p_in_box(p) == False):
                    continue

                insert_unique_point(inter_points, p, TOLERANCE)

        return inter_points

    def intersect_vertical(self, angle, b, x, miny, maxy):
        # print angle, x, b
        y = math.tan(angle) * x + b

        if y <= maxy + TOLERANCE and y >= miny - TOLERANCE:
            return np.array([x, y])

        return None

    def intersect_horizontal(self, angle, b, y, minx, maxx):

        if angle == 0:
            x = maxx
        else:
            x = (y - b) / math.tan(angle)

        if x <= maxx + TOLERANCE and x >= minx - TOLERANCE:
            return np.array([x, y])

        return None

    def plot(self, idx):
        if not idx:
            fig = plt.figure()
        else:
            fig = plt.figure(idx)

        # plot bounding box
        # print 'box', [self.min_x, self.max_x, self.min_y, self.max_y]
        plt.plot([self.min_x, self.max_x, self.max_x, self.min_x, self.min_x],
                 [self.min_y, self.min_y, self.max_y, self.max_y, self.min_y], 'r', linestyle='dashdot')


class BoundingLines:

    def __init__(self, start):
        self.start = start
        self.lb_angle = None
        self.ub_angle = None
        self.lb_point = None
        self.ub_point = None

        self.lb_angle_b = None
        self.ub_angle_b = None

        self.new_point = None
        self.new_angle = None

    def add_point(self, p, angle=None):
        self.new_point = p

        if not angle:
            self.new_angle = get_angle(self.start, p)
        else:
            self.new_angle = angle

        self.update()

    def update(self):

        new_angle = self.new_angle
        p = self.new_point

        if not self.ub_angle or new_angle > self.ub_angle:
            self.ub_angle = new_angle
            self.ub_point = p
            self.ub_angle_b = self.get_b(new_angle)

        if not self.lb_angle or new_angle < self.lb_angle:
            self.lb_angle = new_angle
            self.lb_point = p
            self.lb_angle_b = self.get_b(new_angle)

    def get_b(self, angle):
        b = self.start[1] - math.tan(angle) * self.start[0]
        return b

    def plot(self, idx):
        if not idx:
            fig = plt.figure()
        else:
            fig = plt.figure(idx)

        plt.plot(
            [self.start[0], self.lb_point[0]],
            [self.start[1], self.lb_point[1]], 'r', linestyle='dotted')

        plt.plot(
            [self.start[0], self.ub_point[0]],
            [self.start[1], self.ub_point[1]], 'r', linestyle='dotted')


class QuadrantSystem:

    def __init__(self, start, master, quadrant_idx):
        self.start = start
        self.centroid = None
        self.box = BoundingBox(start)
        self.lines = BoundingLines(start)
        self.intersections = []
        self.angle_range = None

        self.new_angle = None
        self.new_point = None
        self.point_count = 0

        self.lb_dists = None
        self.ub_dists = None
        self.corner_dists = None
        self.bounds = None
        self.master = master
        self.quadrant_idx = quadrant_idx

        self.farthest_point = None
        self.last_proj = None
        self.max_len = None
        self.cur_dist = 0
        self.cur_dist_pos = None
        self.cur_max_dist = None
        self.drift = 0
        self.proj = None

        self.key_point_pairs = []

    def process_point(self, p):
        if self.farthest_point is None:
            self.farthest_point = p
            self.max_len = point_to_point_distance(
                self.start, p)
            self.last_proj = p
        else:
            if point_to_point_distance(self.start, p) > self.max_len:
                self.farthest_point = p
                self.max_len = point_to_point_distance(
                    self.start, p)

            if self.master.last_computed != None:

                # print self.farthest_point
                farthest_on_last = get_proj(
                    self.start, self.master.last_computed_point, self.farthest_point)

                self.last_proj = farthest_on_last

                self.cur_dist = point_to_line_dist(
                    self.start, self.master.last_computed_point, p)

                self.cur_dist_pos = self.new_angle < self.master.last_angle

                theta = get_angle(self.start, p)

                xx = self.max_len * math.cos(theta) + self.start[0]
                yy = self.max_len * math.sin(theta) + self.start[1]
                pp = np.array([xx, yy])
                self.proj = pp

                if self.cur_max_dist == None:
                    self.cur_max_dist = self.master.last_computed

                d = point_to_line_dist(
                    self.start, self.master.last_computed_point, pp)

                if d > self.cur_max_dist:
                    self.cur_max_dist = d

    def add_point(self, p):

        master = self.master

        self.new_angle = get_angle(self.start, p)

        if not self.angle_range:

            self.angle_range = [
                int(self.new_angle / (math.pi / 2)) * math.pi / 2, (int(self.new_angle / (math.pi / 2)) + 1) * math.pi / 2]
            Print('angle: %s, range:%s' % (self.new_angle,  self.angle_range))

        master = self.master
        # print '1', master.points
        self.box.add_point(p)
        # print '2', master.points
        self.lines.add_point(p)
        # print '3', master.points

        if self.centroid == None:
            self.centroid = [p[0], p[1]]
        else:
            self.centroid[0] = (
                self.centroid[0] * (self.point_count) + p[0]) / (self.point_count + 1)
            self.centroid[1] = (
                self.centroid[1] * (self.point_count) + p[1]) / (self.point_count + 1)

        self.point_count += 1

         # get projection points on two bounding lines

        # print '4', master.points
    def angle_in_quadrant(self, angle):
        # if angle > math.pi:
        #    angle = angle - math.pi

        in_quadrant = False
        in_lines = False

        la = self.angle_range[0]
        ua = self.angle_range[1]
        if (angle >= la and angle < ua):# or (angle > math.pi and angle - math.pi >= la and angle - math.pi < ua) or (angle < math.pi and angle + math.pi >= la and angle + math.pi < ua):
            in_quadrant = True

        la = self.lines.lb_angle
        ua = self.lines.ub_angle
        if (angle >= la and angle <= ua):# or (angle > math.pi and angle - math.pi >= la and angle - math.pi <= ua) or (angle < math.pi and angle + math.pi >= la and angle + math.pi <= ua):
            in_lines = True

        #print in_quadrant, in_lines

        #print 'check quadrant', angle, self.angle_range, in_quadrant, in_lines
        return in_quadrant, in_lines

    def get_bounds(self, p):
        box = self.box.get_box()
        l_points = self.box.get_intersection(self.lines)

        # print 'l_points', l_points
        bounds = None

        if self.point_count == 1:
            d = point_to_line_segment_dist(
                self.start[0], self.start[1], p[0], p[1], self.box.min_x, self.box.min_y)
            bounds = [d, d]
            #print 'single box', bounds
            self.bounds = bounds
            return bounds

        angle = get_angle(self.start, p)
        corners = self.box.get_corners()
        i_points = self.box.intersect_line(angle, self.lines.get_b(angle))

        if self.quadrant_idx == 0:
            self.key_point_pairs = (  )


        lb_dists = []
        for i in l_points[0]:
            lb_dists.append(point_to_line_dist(self.start, p, i))
        lbp_dist = point_to_line_dist(self.start, p, self.lines.lb_point)

        ub_dists = []
        for i in l_points[1]:
            ub_dists.append(point_to_line_dist(self.start, p, i))
        ubp_dist = point_to_line_dist(self.start, p, self.lines.ub_point)

        corners = self.box.get_corners()
        corner_dists = []
        far_corner = []
        for i in corners:
            corner_dists.append(
                point_to_line_dist(self.start, p, i))
            far_corner.append( np.dot(i,p)/np.linalg.norm(p))
        far_corner = max(far_corner)


        # cp_dist = point_to_line_dist(self.start, p, self.centroid)

        angle = get_angle(self.start, p)
        in_quadrant, in_lines = self.angle_in_quadrant(angle)
        in_box = True

        # def intersect_line(self, angle, b):
        i_points = self.box.intersect_line(angle, self.lines.get_b(angle))
        if len(i_points) == 0:
            in_box = False

        self.lb_dists = lb_dists
        self.ub_dists = ub_dists
        self.corner_dists = corner_dists

        if in_quadrant:
            #print 'in quadrant'
            idx = self.quadrant_idx
            if idx % 2 == 0:
                c_dists = [corner_dists[0], corner_dists[2]]
            else:
                c_dists = [corner_dists[1], corner_dists[3]]
                # c_dists = [0]


            pd = np.linalg.norm(p)

            c_dist = 0
            if pd > far_corner:
                c_dist = max(c_dists)
            else:
                c_dist = min(c_dists)

            if in_lines:
                bounds = [
                    max(min(lb_dists), min(ub_dists), lbp_dist, ubp_dist, c_dist), max(lb_dists + ub_dists + c_dists)]
                # bounds = [0,1000]
            elif in_box:
                # bounds = [
                # max(min(lb_dists), min(ub_dists), sorted(corner_dists)[1],
                # lbp_dist, ubp_dist, cp_dist), max(lb_dists + ub_dists)]

                bounds = [
                    max(min(lb_dists), min(ub_dists), lbp_dist, ubp_dist, c_dist), max(lb_dists + ub_dists + c_dists)]
                # bounds = [0,1000]
            else:
                bounds = [
                    max(min(lb_dists), min(ub_dists), lbp_dist, ubp_dist, c_dist), max(lb_dists + ub_dists + c_dists)]
                # bounds = [0,1000]

            # if self.cur_max_dist != None:
            #     # print self.cur_max_dist, self.master.last_computed
            #     bounds[1] = min(
            #         2 * self.cur_max_dist, bounds[1])

            # print 'in quadrant', self.lines.lb_angle, self.lines.ub_angle, lb_dists, ub_dists, corner_dists
            # bounds
        else:
            #print 'not in quadrant'
            #bounds = [
            #    max(min(lb_dists), min(ub_dists), sorted(corner_dists)[2], lbp_dist, ubp_dist), max(corner_dists)+self.master.small[self.quadrant_idx]]
            #bounds = [
            #    max(min(lb_dists), min(ub_dists), sorted(corner_dists)[2], lbp_dist, ubp_dist), max(max(corner_dists),self.master.small[self.quadrant_idx])]
            bounds = [
                max(min(lb_dists), min(ub_dists), sorted(corner_dists)[2], lbp_dist, ubp_dist), max(corner_dists)]
            # bounds = [0,1000]
            # print "SHIT case"
            # print 'outta quadrant', lb_dists, ub_dists, corner_dists

        # if self.farthest_point != None and self.master.last_computed != None:

        #     curr_proj = get_proj(self.start, p, self.farthest_point)

        # self.drift = max(self.drift1, self.drift2)
        #     self.drift = point_to_point_distance(self.last_proj, curr_proj)

            # print self.drift1, self.drift2,  self.master.last_computed,
            # bounds

            # if (self.drift + self.master.last_computed) < bounds[1]:
            #    print self.drift + self.master.last_computed, bounds[1]
            # self.master.get_farthest()

            # print self.drift + self.master.last_computed, self.drift +
            # self.cur_dist

            # print self.drift + self.master.last_computed, self.drift +
            # print self.cur_dist_pos, self.master.last_pos
            # if self.cur_dist_pos and self.cur_dist_pos == self.master.last_pos:
            #    bounds[1] = min(2 * self.cur_max_dist, bounds[1])
            # else:
                # bounds[1] = min(
                    # point_to_line_dist(self.start,
                    # self.master.last_computed_point, p) + self.cur_max_dist,
                    # bounds[1])

            # bounds[1] = min(
            #     max(self.drift + self.master.last_computed, self.master.last_computed +
            #         self.cur_dist, self.master.last_computed +
            #         self.cur_max_dist,
            #         point_to_line_dist(
            #             self.start, self.master.last_computed_point, p) + self.master.last_computed,
            # point_to_line_dist(self.start, self.master.last_computed_point,
            # p) + self.cur_max_dist), bounds[1])
        self.bounds = bounds
        #self.bounds[1] += self.error_max
        return bounds

    def plot(self, idx=None):
        if not idx:
            fig = plt.figure()
        else:
            fig = plt.figure(idx)

        if self.proj != None:
            plt.plot(
                self.proj[0], self.proj[1], '^r', markersize=10.0)
        self.lines.plot(idx)
        self.box.plot(idx)


class TrajProcessor:

    def __init__(self, start, eps, test=False, debug=False, prev_err =0.0):
        self.quadrants = [None, None, None, None]
        self.start = start
        self.end = None
        self.points = []
        self.lower_bound = 0
        self.upper_bound = 0
        self.test = test
        self.pro_count = 0
        self.epsilon = eps
        self.last_computed = None
        self.last_computed_point = None
        self.max_d = 0
        self.last_max_point = None
        self.last_angle = None
        self.last_pos = None
        self.curr_max = None
        self.small=[0,0,0,0]
        self.prev_err = prev_err

        #print 'self.prev_err',self.prev_err

    def re_init(self, start, eps, test=False, debug=False, prev_err =0.0):
        self.quadrants = [None, None, None, None]
        self.start = start
        self.end = None
        self.points = []
        self.lower_bound = 0
        self.upper_bound = 0
        self.test = test
        self.pro_count = 0
        self.epsilon = eps
        self.last_computed = None
        self.last_computed_point = None
        self.max_d = 0
        self.last_max_point = None
        self.last_angle = None
        self.last_pos = None
        self.curr_max = None
        self.small=[0,0,0,0]
        self.prev_err = prev_err

    def get_g_box(self):

        res = [[], [], [], []]
        for q in self.quadrants:
            if q:
                res[0].append(q.box.min_x)
                res[1].append(q.box.max_x)
                res[2].append(q.box.min_y)
                res[3].append(q.box.max_y)

        res[0] = min(res[0] + [self.start[0], self.end[0]])
        res[1] = max(res[1] + [self.start[0], self.end[0]])
        res[2] = min(res[2] + [self.start[1], self.end[1]])
        res[3] = max(res[3] + [self.start[1], self.end[1]])

        return res

    def plot(self, p, idx=1, scale=10, show_bounds=True, decision_text=''):

        if len(self.points) <= 4:
            return

        self.get_farthest()

        fig = None
        if not idx:
            fig = plt.figure(figsize=(12, 10), dpi=80)
        else:
            fig = plt.figure(idx, figsize=(12, 10), dpi=80)

        # forceAspect(ax)
        for q in self.quadrants:
            if q:
                q.plot(idx)

        box = self.get_g_box()

        # print "start/p/box:", self.start, self.end, box
        # print self.points
        # get_angle(self.start, self.end)

        # plt.xticks(
        #     np.arange(box[0], box[1], scale, dtype=np.int))
        # plt.yticks(
        #     np.arange(box[2], box[3], scale, dtype=np.int))

        ax = fig.gca()
        ax.set_aspect('equal', 'box')

        plt.plot([self.start[0], self.start[0]],
                 [box[2], box[3]], '-k')
        plt.plot([box[0], box[1]],
                 [self.start[1], self.start[1]], '-k')

        points = np.array(self.points)
        plt.plot(self.start[0], self.start[1], 'ob', markersize=8.0)
        plt.plot(p[0], p[1], 'or', markersize=8.0)

        # last farthest point

        if self.last_max_point != None:
            plt.plot(
                self.last_max_point[0], self.last_max_point[1], '*g', markersize=20.0)

            plt.plot(
                self.last_computed_point[0], self.last_computed_point[1], 'og', markersize=20.0)

        self.get_farthest(True)
        plt.plot(
            self.last_max_point[0], self.last_max_point[1], '*r', markersize=20.0)

        plt.plot(
            self.last_computed_point[0], self.last_computed_point[1], 'or', markersize=20.0)

        plt.plot(points[:, 0], points[:, 1], '*g', markersize=8.0)

        plt.plot([self.start[0], p[0]],
                 [self.start[1], p[1]], '-g')

        if show_bounds:
            ax.annotate('start', (self.start[0], self.start[1]))
            ax.annotate('end', (p[0], p[1]))

            text_coords = [[0.9, 0.9], [0.02, 0.9], [0.02, 0.05], [0.9, 0.05]]
            for i in xrange(0, 4):
                if self.quadrants[i] != None:
                    if self.quadrants[i].bounds != None:
                        plt.figtext(text_coords[i][0], text_coords[i][1], 'lb:%.2f\nub:%.2f' % (
                            self.quadrants[i].bounds[0], self.quadrants[i].bounds[1]), color='r')
            plt.figtext(
                0.4, 0.02, 'Maxd:%f, Decision:%s' % (self.max_d, decision_text), color='r', weight='bold')
        # mng = plt.get_current_fig_manager()
        # mng.resize(*mng.window.maxsize())
        plt.show()

    def get_quadrant_idx(self, p):
        a = p[0] - self.start[0]
        b = p[1] - self.start[1]

        idx = -1
        if a > 0 and b >= 0:
            idx = 0
        elif a <= 0 and b > 0:
            idx = 1
        elif a < 0 and b <= 0:
            idx = 2
        elif a >= 0 and b < 0:
            idx = 3

        return idx

    def process_point(self, p):

        idx = self.get_quadrant_idx(p)
        self.end = p
        # print('quandrant idx:%s' % idx)

        lbs = []
        ubs = []
        drifts = []

        if len(self.points) > 0:
            if self. last_computed_point is not None:
                d = point_to_line_dist(self.start, self.last_computed_point, p)

                # print 'curr dist', d
                if d > self.curr_max:
                    self.curr_max = d

            if len(self.points) <= 4:
                maxd = self.get_farthest()
                lbs = [maxd]
                ubs = [maxd]
            else:
                # print self.quadrants
                for q in self.quadrants:

                    if q:
                        q.process_point(p)
                        bs = q.get_bounds(p)
                        lbs.append(bs[0])
                        ubs.append(bs[1])

            self.lower_bound = max(lbs)
            self.upper_bound = max(ubs)

        # print 'upper', self.upper_bound
        return idx

    def post_process(self, p, idx):

        d = point_to_point_distance(self.start,self.end)
        if  d <= self.epsilon - self.prev_err:
            if self.small[idx]<d:
                self.small[idx]=d
            return

        if not self.quadrants[idx]:

            # print 'create for ', idx
            self.quadrants[idx] = QuadrantSystem(self.start, self, idx)

        # print 'addding', self.points
        # print 'adding %s to %d' % (p, idx)
        self.quadrants[idx].add_point(p)
        # print 'addding', self.points, p
        self.points.append(p)
        #self.last_computed = False
        # print 'added', self.points

    def get_farthest(self, count=False):
        max_d = 0

        if len(self.points) > 0:
            max_i = 0

            for i in xrange(0, len(self.points)):
                d = point_to_line_dist(self.start, self.end, self.points[i])

                if d > max_d:
                    max_i = i
                    max_d = d

            if count:
                self.pro_count += 1
                self.last_computed = max_d
                self.last_computed_point = self.end
                self.last_angle = get_angle(self.start, self.end)
                self.last_pos = get_angle(
                    self.start, self.points[max_i]) < self.last_angle
                #print max_i, len(self.points)

                for q in self.quadrants:
                    if q:
                        q.drift = 0.0
                
                self.last_max_point = self.points[max_i]
                self.curr_max = max_d

        self.max_d = max_d
        #print max_d
        return max_d

    def check_result(self):
        max_d = self.get_farthest()
        #print 'max_d', max_d
        if max_d + self.prev_err> self.epsilon:
            print 'fault', max_d, self.end
            print self.lower_bound, self.upper_bound
            self.plot(self.end)
            sys.exit(0)

    def decision(self, conservative=False, do_check=False, visualise=False, test=False):
        max_i = 1
        max_d = 0

        #print 1
        if test:
            max_d = self.get_farthest()
            #print max_d, self.prev_err,self.lower_bound,self.upper_bound
            if max_d < self.lower_bound - 0.0000001 or max_d>self.upper_bound+0.0000001:
                #print 'faulty', len(self.points), max_d, self.lower_bound, self.upper_bound
                self.plot(self.end)
                #sys.exit(0)

        # if np.count_nonzero(self.quadrants) == 2:
        #     return -1, 0

        if self.lower_bound > self.epsilon:
            if visualise:
                self.plot(self.end, decision_text='lb>e, Take point')
            return -1, self.lower_bound

        if self.upper_bound + self.prev_err <= self.epsilon:

            if visualise:
                self.plot(self.end, decision_text='ub<=e, Proceed')
            if do_check:
                self.check_result()
            return 0, 0

        #print 'cant decide', self.prev_err
        if not conservative:
            # print 'damn lb:%s ub:%s' % (self.lower_bound, self.upper_bound)
            #self.plot(self.end, decision_text='lb<e<ub, Recalculate')

            # if point_to_point_distance(self.end, self.points[-1]) > 1000:
                # print 'stop the shit'
            #    return -1, 0

            if visualise:
                self.plot(self.end, decision_text='lb<e<ub, Recalculate')

            if test:
                self.pro_count += 1
            else:

                if len(self.points) <= 4:
                    max_d = self.upper_bound
                else:
                    #self.plot(self.end, decision_text='lb<e<ub, Recalculate')
                    max_d = self.get_farthest(count=True) 

            #print max_d
            if max_d  + self.prev_err> self.epsilon:
                return -1, max_d
            else:
                if do_check:
                    self.check_result()
                return 0, max_d

        return -1, max_d


def best_angle(X, obj, i):
    pps = [[],[],[],[]]
    pcounts = [0,0,0,0]
    #offsets = [math.pi*0.25, math.pi*0.75,math.pi*1.25,math.pi*1.75]

    offsets = [math.pi*0.5,math.pi*1.0,math.pi*1.5, math.pi*2]

    ssize = 10
    for j in range(1,ssize):
        if i+j < X.shape[0]:
            idx =  obj.get_quadrant_idx(X[i+j])
            pps[idx].append(X[i+j])
            pcounts[idx] += 1

    max_j = 0
    for j in range(1,4):
        if pcounts[max_j] < pcounts[j]:
            max_j = j

    if pcounts[max_j]>0:
        tangle = get_angle( obj.start, np.mean(  np.array(pps[max_j]), axis=0 ) )
        angle =   offsets[max_j] - tangle
        #print 'angle --> angle', tangle, angle
        #print obj.start, np.mean(  np.array(pps[max_j]), axis=0),  offsets[max_j], tangle, angle
    else:
        angle = 0

    return angle




def traj_to_seg(X, eps):
    n = X.shape[0]
    obj = None
    arr = [0]
    i = 0
    pcount = 0
    angle = 0
    while i < n:
        if not obj:
            obj = TrajProcessor(X[i], eps)
            angle = best_angle(X, obj, i)
            i += 1
            continue
        
        pp = copy(X[i])
        #print pp
        pp = pp.reshape([1,2])
        #print pp
        if angle != 0:
            pp = rotate_points(pp, angle)
        
        pp = pp.flatten()
        idx = obj.process_point(pp)

        decision, max_d = obj.decision(conservative=True, visualise=True)

        print 'decision', decision

        if decision == -1:

            arr.append(i - 1)
            obj = TrajProcessor(X[i - 1], eps)
            angle = best_angle(X, obj, i-1)
            pcount += obj.pro_count
        else:
            obj.post_process(obj.end, idx)
            i += 1

    if arr[-1] != len(X) - 1:
        arr.append(len(X) - 1)

    print n, pcount
    return arr, float(pcount) / (n - 1)


def merge_all_data(ids):
    X = None
    n = 0
    for id in ids:
        XX, nn = extract_data(
            'data/%s' % id, -1, False)
        #print 'data/%s' % id
        #XX = np.array([list(proj_init(x[0], x[1])) for x in XX])

        if X is None:
            X = XX
        else:
            X = np.concatenate((X, XX), axis=0)
        n += nn

    return X, n


def run_one(fnum, eps, check=False, approx=False, visualise=False, test=False, X=None, n=None, prev_err=0.0):

    #if X is None and n is None:
    #    X, n = extract_data(
    #        '../data/%s' % fnum, -1, False)
        #X = np.array([list(proj_init(x[0], x[1])) for x in X])

    #print 'Starting job for %s, eps %f, approx %s, no.points %d' % (fnum, eps, approx, n)

    obj = None
    arr = [0]
    bounds = []
    eps = eps
    max_dd = 0
    pcount = 0
    i = 0
    angle = 0

    while i < n:

        #print 'id:%d'%i
        if not obj:
            obj = TrajProcessor(X[i], eps, prev_err=prev_err)
            angle = best_angle(X, obj, i)
            i += 1
            continue

        #if i % 5000 == 0:
        #    print 'Processing %d' % i

        # if point_to_point_distance(X[i - 1], X[i]) < 0.000001:
        #    i += 1
        #    continue

        # print 'before process', obj.points
        pp = copy(X[i])
        #print pp
        if angle != 0:
          pp = rotate_point(pp, angle, obj.start)
        #print 'rotated', X[i],angle, pp
        idx = obj.process_point(pp)

        #print 'before decision'
        decision, max_d = obj.decision(
            conservative=approx, do_check=check, visualise=visualise, test=test)

        bounds.append([obj.lower_bound, obj.upper_bound, obj.max_d])

        #print 'decision', decision, max_d, obj.lower_bound, obj.upper_bound
        # obj.plot(X[i])
        #print 'decision %s, obj.lower_bound %s, obj.upper_bound %s, pcount\
        #%s' % (decision, obj.lower_bound, obj.upper_bound, pcount)
        if decision == -1:
            # sys.exit(0)

            arr.append(i - 1)

            # print np.array(obj.points)[0:-1, :], arr
            # dmax, idmax = curve_approx_dist(np.array(obj.points)[0:-1, 0], np.array(obj.points)[0:-1, 1], arr)

            # if dmax > eps:
            #    print 'fault', dmax, arr
            #    sys.exit(0)

            pcount += obj.pro_count

            # print 'creating new obj'

            obj.re_init(X[i - 1], eps, prev_err=prev_err)
            angle = best_angle(X, obj, i-1)
            if max_d > max_dd:
                max_dd = max_d
        else:
            # print 'before postprocess', obj.points
            obj.post_process(obj.end, idx)
            # print 'after postprocess', obj.points
            i += 1

    if arr[-1] != len(X) - 1:
        arr.append(len(X) - 1)

    dmax, idmax, ranges = curve_approx_dist(X[:, 0], X[:, 1], arr)

    # print len(arr), None, 1.0 - float(pcount) / (n - 2), arr, None

    # assert(dmax <= eps)
    # res0 = [len(arr),
    #         1.0 - float(pcount) / (n - 1), eps, dmax]
    res0=[]
    # print res0
    # ResultList, dmax = DouglasPeucker(X, np.arange(len(X)), eps)

    # dmax, idmax = curve_approx_dist(X[:, 0], X[:, 1], ResultList)

    # print len(ResultList), dmax, ResultList

    # res1 = [len(ResultList), dmax]
    # print res1
    # print 'pcount', pcount
    #print 'done'
    return res0, bounds, X, np.array(arr)
    #return res0, res1, 1.0 - float(pcount) / (n - 1), bounds, X, np.array(idx)


def make_plot_pp():

    ids = (('car', [171, 174]), ('bat', [163, 175, 186, 1937]))

    epsss = [
        [1, 2, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100], range(1, 11)]
    idx = 0

    for tp in fids:

        fid = tp[0]
        ids = tp[1]
        X, n = merge_all_data(ids)

        res0 = []
        res1 = []
        res2 = []
        pp = []
        epss = epsss[idx]

        for eps in epss:
            r0, r1, p, bs, X, idx = run_one(
                fid, eps, check=False, approx=False, visualise=False, test=True, X=X, n=n)
            res0.append(r0)
            res1.append(r1)
            pp.append(p)
            # plot_bounds(fid, eps, bs, sample=len(bs) / 100)

            r2, r1, p, bounds, X, idx = run_one(
                fid, eps, check=False, approx=True)
            res2.append(r2)

        res0 = np.array(res0)
        res1 = np.array(res1)
        res2 = np.array(res2)
        pp = np.array(pp)

        print 'plot'
        plot_res(res0, res1, pp, idx, fid, [str(eps) for eps in epss])
        plot_res(res0, res2, None, idx, str(fid)
                 + '-approx', [str(eps) for eps in epss])

        idx += 1


def plot_res(res0, res1, pp, idx, fname, epss):
    # res = [(arr, dmax, 1.0 - float(pcount) / (n - 1)), (res, dmax)]

    # fig = plt.figure(idx)
    print res0, res1, pp
    fig, ax1 = plt.subplots()
    print -2

    N = len(epss)

    l0, = ax1.plot(
        np.arange(N), res0[:, 0], '-.*r', markersize=8, linewidth=1)
    l1, = ax1.plot(
        np.arange(N), res0[:, 1], '-.^r', markersize=8, linewidth=1)
    l2, = ax1.plot(
        np.arange(N), res1[:, 0], '-.*g', markersize=8, linewidth=1)
    l3, = ax1.plot(
        np.arange(N), res1[:, 1], '-.^g', markersize=8, linewidth=1)
    print -1

    if pp != None:
        ax2 = ax1.twinx()
        l4, = ax2.plot(
            np.arange(N), pp, '-.ob', markersize=8, linewidth=1)
        print 0

        plt.legend([l0, l1, l2, l3, l4],
                   ['No.Pnt', 'MaxErr', 'No.Pnt DP', 'MaxErr DP', 'pp'], loc=7)
        ax2.set_ylabel('Pruning Power')
        ax2.set_ylim([0, 1.2])
        for tl in ax2.get_yticklabels():
            tl.set_color('b')
    else:
        plt.legend([l0, l1, l2, l3],
                   ['No.Pnt', 'MaxErr', 'No.Pnt Approx', 'MaxErr Approx'], loc=7)

    ax1.set_xlabel('Epsilon (m)')
    ax1.set_ylabel('MaxErr/No.Pnt')

    plt.subplots_adjust(left=-1)
    plt.tight_layout()
    print 1

    plt.xticks(np.arange(N), epss)

    plt.savefig('../figs/tc-%s.pdf' % (fname), bbox_inches='tight')
    plt.close()
    print 2


def plot_bounds(fname, eps, bounds, sample):
    # res = [(arr, dmax, 1.0 - float(pcount) / (n - 1)), (res, dmax)]

    # fig = plt.figure(idx)

    fig, ax1 = plt.subplots()

    bounds = np.array(bounds)
    thresh = 3 * eps
    print bounds

    idx = (bounds[:, 1] < thresh).nonzero()
    print len(idx[0])

    bounds = bounds[idx[0], :]
    N = len(bounds)
    bounds = bounds[range(0, N, sample), :]

    N = bounds.shape[0]

    l0, = ax1.plot(
        np.arange(N), bounds[:, 0], ':xg', markersize=6, linewidth=1,mfc="None")
    l1, = ax1.plot(
        np.arange(N), bounds[:, 1], ':+b', markersize=6, linewidth=1,mfc="None")
    l2, = ax1.plot(
        np.arange(N), bounds[:, 2], ':or', markersize=8, linewidth=1,mfc="None")

    l3, = ax1.plot([0, N], [eps, eps], '-k')

    plt.legend([l0, l1, l2, l3],
               ['LowerBound', 'UpperBound', 'Actual Deviation', 'Tolerance'], loc=0)

    ax1.set_ylabel('Meters')

    plt.subplots_adjust(left=-1)
    plt.tight_layout()

    plt.savefig('../figs/tc-%s-%d-bounds.pdf' %
                (fname, eps), bbox_inches='tight')

    plt.close()


def run_all(fids=None):
    fids = [174]
    epsss = [[5]]
    # epss = [10, 100]

    #fids = [171, 163]
    #epsss = [
    #    [1, 2, 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100], range(1, 11)]
    idx = 0

    for fid in fids:

        res0 = []
        res1 = []
        res2 = []
        pp = []
        epss = epsss[idx]

        for eps in epss:
            r0, r1, p, bs, X, idx = run_one(
                fid, eps, check=False, approx=False, visualise=False, test=True)
            res0.append(r0)
            res1.append(r1)
            pp.append(p)
            plot_bounds(fid, eps, bs, sample=len(bs) / 100)

            r2, r1, p, bounds, X, idx = run_one(
                fid, eps, check=False, approx=True)
            res2.append(r2)

        res0 = np.array(res0)
        res1 = np.array(res1)
        res2 = np.array(res2)
        pp = np.array(pp)

        print 'plot'
        #plot_res(res0, res1, pp, idx, fid, [str(eps) for eps in epss])
        #plot_res(res0, res2, None, idx, str(fid)
        #         + '-approx', [str(eps) for eps in epss])

        idx += 1

# bats : 186, 175, 163, 1937
# cars : 171, 174

if __name__ == '__main__':

    #run_all()
    # point_to_line_segment_dist.counter = 0

#    X, n = merge_all_data(ids=[163, 175, 186, 1937])
#    writecsvfile(X, '../data/batsp.txt')
    XX, n = merge_all_data(ids=[174])
    N = n
#    writecsvfile(X, '../data/carsp.txt')


    # N = 50
    # a = np.arange(1,N+1,2)
    # b = np.zeros(N/2)+0.5
    # c = np.arange(2,N+2,2)
    # d = np.zeros(N/2)+0.5

    # x = np.vstack([a,b]).T.flatten()
    # y = np.vstack([d,c]).T.flatten()

    # X = np.vstack([x,y]).T

    e = 2.0

    res, bounds, X, arr = run_one(None, 5*e, check=False, approx=False, visualise=False, test=False, X=XX, n=XX.shape[0])
    print res
    print arr.shape[0], float(arr.shape[0])/XX.shape[0]
    print curve_approx_dist(XX[:,0], XX[:,1], arr)
    

    res0, bounds, X, arr0 = run_one(None, e, check=False, approx=False, visualise=False, test=False, X=XX, n=XX.shape[0])
    print arr0[:10]
    print res0
    print arr0.shape[0], float(arr0.shape[0])/XX.shape[0]
    print curve_approx_dist(XX[:,0], XX[:,1], arr0)


    #arr1, dmax = DouglasPeucker(XX[arr0,:], np.arange(arr0.shape[0]), e*4)
    print 'n',arr0.shape[0], XX[arr0,:].shape
    res1, bounds, X, arr1 = run_one(None, 5*e, check=False, approx=False, visualise=False, test=False, X=XX[arr0,:], n=arr0.shape[0], prev_err=e)
    print res1
    print arr1.shape[0], float(arr1.shape[0])/XX.shape[0]
    print arr1[:10]
    print arr0[arr1][:10]

    #print 9906 in arr0
    print curve_approx_dist(XX[arr0,0], XX[arr0,1], arr1)
    print curve_approx_dist(XX[:,0], XX[:,1], arr0[arr1])



    from curve_approximation import *
    #ResultList, dmax, max_id, status = curve_approximation(x, y, e)
    #ResultList = curve_approximation(X[:,0], X[:,1], e)
    #ResultList = ResultList[0]
    # print len(ResultList[0])/float(N)


    ResultList, dmax = DouglasPeucker(XX, np.arange(len(XX)), e*5)
    #ResultList = buffered_greedy(X, e, 16)

    print len(ResultList), len(ResultList)/float(N)

    # #plt.plot( X[:,0], X[:,1], 'o' )
    # plt.plot( X[arr,0], X[arr,1], 'o-' )
    # #plt.plot( X[ResultList,0], X[ResultList,1], '-+' )

    # print curve_approx_dist(X[:,0], X[:,1], ResultList)
    # #plt.show()

    # plt.tight_layout()

    # plt.savefig('extremecase.pdf', bbox_inches='tight')



    # print X.shape
    #res0, bounds, X, arr = run_one(
    #     'bats', 2, check=False, approx=False, visualise=False, test=False, X=X, n=n)
    #print res0
    # X, n = merge_all_data(ids=[171, 174])

    # 71 .9x
    # 86 .86  75 .89 937 88  63
    #fid = 163
    #eps = 2
    #X, n = extract_data('../data/%s' % fid, -1, False)
    #n = 2000
    #X = X[0:n, :]

    #X = np.array([list(proj_init(x[0], x[1])) for x in X])
    #res0, bounds, X, arr = run_one(
    #    fid, eps, check=False, approx=False, visualise=True, test=False, X=X, n=n)

    ##ResultList, dmax = DouglasPeucker(X, np.arange(len(X)), eps)
    #print len(ResultList)
    # dmax, idmax = curve_approx_dist(X[:, 0], X[:, 1], ResultList)
    # print 'bqs', point_to_line_segment_dist.counter

    # from plot_path import plot_path
    # plot_path(X, idx, 'path.pdf', 'sc', title=None)
