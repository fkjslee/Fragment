import random
import math
from PIL import Image
import numpy as np
import pdb
import cv2
import time
import shutil
import os
from scipy.interpolate import interp1d
import pickle as pkl
import Transform

min_fragment_pixels = 60 ** 2

def update_bound(bound, point):
    bound[0] = point[0] if point[0] < bound[0] else bound[0]
    bound[1] = point[0] if point[0] > bound[1] else bound[1]
    bound[2] = point[1] if point[1] < bound[2] else bound[2]
    bound[3] = point[1] if point[1] > bound[3] else bound[3]

def slicer_skew(file_name, pieces, rand_angle, ctrl_length, starting=1, color_file=None):
    frag_count = starting
    # bg_color = [0, 0, 0, 0]
    if color_file == None:
        bg_color = [255, 255, 255]
    else:
        with open(color_file, 'r') as f:
            rgb = f.read()
            rgb = rgb.split(' ')
            rgb.reverse()
            bg_color = [int(s) for s in rgb]

    raw = cv2.imread(file_name)
    rows, cols, chns = raw.shape
    # raw = cv2.cvtColor(raw, cv2.COLOR_RGB2RGBA)

    def check_bound(mat_tmp, i, j, num):
        val = mat_tmp[i][j]
        if (abs(mat_tmp[i][max(0, j-1)] - val) == num or abs(mat_tmp[i][min(cols-1, j+1)] - val) == num
         or abs(mat_tmp[max(0, i-1)][j] - val) == num or abs(mat_tmp[min(rows-1, i+1)][j] - val) == num):
            return True
        else:
            return False

    def range_check(pt):
        _1 = -1
        _2 = -1
        if pt[0] > rows-1:
            _1 = rows-1
        elif pt[0] < 0:
            _1 = 0
        else:
            _1 = pt[0]
        if pt[1] > cols-1:
            _2 = cols-1
        elif pt[1] < 0:
            _2 = 0
        else:
            _2 = pt[1]
        return (_1, _2)

    def gen_neighbors(pt):
        i = pt[0]
        j = pt[1]
        tmp = [(i, j-1), (i, j+1), (i-1, j), (i+1, j), (i-1, j-1), (i-1, j+1), (i+1, j-1), (i+1, j+1), (i, j)]
        res = []
        for k in tmp:
            _k = range_check(k)
            if _k not in res:
                res.append(_k)
        return res

    lblmap = np.zeros((rows, cols)).astype(int)
    mask = np.zeros((rows, cols)).astype(int)
    all_debug = np.ones((rows, cols)).astype(int) * 255

    distr = []
    distr_bound = []  # up, down, left, right
    distr.append(cols * rows)
    distr_bound.append([0, rows - 1, 0, cols - 1])
    center = (cols / 2, rows / 2)
    curlbl = 0

    frag_tmp_path = 'debug'
    if os.path.exists('%s'%frag_tmp_path):
        shutil.rmtree('%s'%frag_tmp_path)
    os.mkdir('%s'%frag_tmp_path)
    all_ctrl_pts = []
    all_fixed_pts = []
    all_edge_pts = []
    all_edge = np.zeros((rows, cols)).astype(int)

    def dfs(mask, x, y):
        visited = np.zeros(mask.shape)
        mask_visited = visited.copy()
        rows, cols = mask.shape
        stack = []
        stack.append((y, x))
        visited[y, x] = 1
        running = True

        while running:
            if y - 1 >= 0 and not visited[y - 1, x] and not global_visited[y - 1, x] and not mask[y - 1, x]:
                y = y - 1
                stack.append((y, x))
                visited[y, x] = 1

            elif x - 1 >= 0 and not visited[y, x - 1] and not global_visited[y, x - 1] and not mask[y, x - 1]:
                x = x - 1
                stack.append((y, x))
                visited[y, x] = 1

            elif y + 1 < rows and not visited[y + 1, x] and not global_visited[y + 1, x] and not mask[y + 1, x]:
                y = y + 1
                stack.append((y, x))
                visited[y, x] = 1

            elif x + 1 < cols and not visited[y, x + 1] and not global_visited[y, x + 1] and not mask[y, x + 1]:
                x = x + 1
                stack.append((y, x))
                visited[y, x] = 1

            elif stack:
                y, x = stack.pop(-1)

                if y - 1 >= 0:
                    mask_visited[y - 1, x] = mask[y - 1, x]
                if x - 1 >= 0:
                    mask_visited[y, x - 1] = mask[y, x - 1]
                if y + 1 < rows:
                    mask_visited[y + 1, x] = mask[y + 1, x]
                if x + 1 < cols:
                    mask_visited[y, x + 1] = mask[y, x + 1]
            else:
                running = False

        visited = np.logical_or(visited, mask_visited).astype(np.float32)
        return visited

    while len(distr) < pieces:
        edge = np.zeros((rows, cols)).astype(int)

        new_number = len(distr)
        distr.append(0)
        distr_bound.append([])

        # TODO horizontal lines have strange bugs
        while True:
            angle = random.uniform(-35.0, 35.0)
            if abs(angle) > 7.0:
                break
        # angle = 20
        curlbl_height = distr_bound[curlbl][1] + 1 - distr_bound[curlbl][0]
        curlbl_width = distr_bound[curlbl][3] + 1 - distr_bound[curlbl][2]
        if curlbl_height < curlbl_width:
            angle = (90 - angle) if angle > 0 else (-90 - angle)

        k = math.tan(math.radians(angle))
        b = center[1] - k * center[0]

        distr_bound[curlbl] = [rows - 1, 0, cols - 1, 0]
        distr_bound[new_number] = [rows - 1, 0, cols - 1, 0]

        st = time.time()
        for i in range(rows):
            for j in range(cols):
                if lblmap[i][j] == curlbl:
                    if i > k * j + b:
                        lblmap[i][j] = new_number
                        distr[curlbl] -= 1
                        distr[new_number] += 1
                        update_bound(distr_bound[new_number], (i, j))
                    else:
                        update_bound(distr_bound[curlbl], (i, j))
        ed = time.time()
        print('upd_pts time: ', ed-st)

        ctrl_length = 15
        edge_pts = []
        ctrl_pts = []

        # pdb.set_trace()
        st = time.time()
        for i in range(rows):
            for j in range(cols):
                if lblmap[i][j] == new_number:
                    if check_bound(lblmap, i, j, abs(new_number - curlbl)):
                        edge_pts.append((i, j))

        if k < 0:
            edge_pts = sorted(edge_pts, key=lambda x: x[1], reverse=True)

        ctrl_pts = edge_pts[::ctrl_length]
        fix_pt = [1 for i in range(len(ctrl_pts))]
        if edge_pts[-1] not in ctrl_pts:
            ctrl_pts.append(edge_pts[-1])
            fix_pt.append(1)

        if ctrl_pts[0] != edge_pts[0] or ctrl_pts[-1] != edge_pts[-1]:
            pdb.set_trace()

        fix_pt[0] = 0
        fix_pt[1] = 0.1 if fix_pt[1] is not 0 else 0
        fix_pt[-1] = 0
        fix_pt[-2] = 0.1 if fix_pt[-2] is not 0 else 0

        all_edge_pts.append(edge_pts)
        all_ctrl_pts.append(ctrl_pts)
        all_fixed_pts.append(fix_pt)

        ed = time.time()
        print('edge_pts time: ', ed-st)

        for idx, pt in enumerate(edge_pts):
            edge[pt[0]][pt[1]] = 255
            all_edge[pt[0]][pt[1]] = 255
            # mask[pt[0]][pt[1]] = 255

        # for pt in ctrl_pts:
        #     all_debug[pt[0]][pt[1]] = 192
        # cv2.imwrite('%s/edge_%d.png' % (frag_tmp_path, len(distr)-2), edge, (int(cv2.IMWRITE_PNG_COMPRESSION), 0))

        max_pixel_cnt = 0
        arg_max = -1
        for i in range(len(distr)):
            if distr[i] > max_pixel_cnt:
                max_pixel_cnt = distr[i]
                arg_max = i

        curlbl = arg_max
        center = ((distr_bound[curlbl][2] + distr_bound[curlbl][3]) / 2,
                  (distr_bound[curlbl][0] + distr_bound[curlbl][1]) / 2)

    cv2.imwrite('%s/all_edge.png' % (frag_tmp_path), all_edge, (int(cv2.IMWRITE_PNG_COMPRESSION), 0))

    for idx, edge in enumerate(all_edge_pts):
        for idx2, _edge in enumerate(all_edge_pts):
            if idx != idx2:
                _p1 = _edge[0]
                _p2 = _edge[-1]
                _p1_neigh = gen_neighbors(_p1)
                _p2_neigh = gen_neighbors(_p2)
                res1 = set(_p1_neigh) & set(edge)
                res2 = set(_p2_neigh) & set(edge)
                if res1:
                    tar_pt = ()
                    min_dist = 99
                    for p in res1:
                        if min_dist > abs(p[0] - _p1[0]) + abs(p[1] - _p1[1]):
                            min_dist = abs(p[0] - _p1[0]) + abs(p[1] - _p1[1])
                            tar_pt = p
                    # edge_pt_idx = edge.index(tar_pt)
                    # ctrl_idx = math.ceil(edge_pt_idx / ctrl_length)
                    tmp = all_ctrl_pts[idx]
                    # tmp.insert(ctrl_idx, tar_pt)
                    if tar_pt not in tmp:
                        _st_bool = (tmp[0][0] > tar_pt[0], tmp[0][1] > tar_pt[1])
                        ctrl_idx = -2
                        for _t_idx, _t_p in enumerate(tmp):
                            if _st_bool != (_t_p[0] > tar_pt[0], _t_p[1] > tar_pt[1]):
                                ctrl_idx = _t_idx
                                break
                        if ctrl_idx == -2:
                            pdb.set_trace()
                        tmp.insert(ctrl_idx, tar_pt)
                        all_ctrl_pts[idx] = tmp
                        tmp = all_fixed_pts[idx]
                        tmp.insert(ctrl_idx, 0)
                        tmp[max(0, ctrl_idx-1)] = 0.1 if tmp[max(0, ctrl_idx-1)] is not 0 else 0
                        tmp[min(len(tmp)-1, ctrl_idx+1)] = 0.1 if tmp[min(len(tmp)-1, ctrl_idx+1)] is not 0 else 0
                        all_fixed_pts[idx] = tmp
                    else:
                        ctrl_idx = tmp.index(tar_pt)
                        tmp = all_fixed_pts[idx]
                        tmp[ctrl_idx] = 0
                        tmp[max(0, ctrl_idx-1)] = 0.1 if tmp[max(0, ctrl_idx-1)] is not 0 else 0
                        tmp[min(len(tmp)-1, ctrl_idx+1)] = 0.1 if tmp[min(len(tmp)-1, ctrl_idx+1)] is not 0 else 0
                        all_fixed_pts[idx] = tmp
                    # tmp = all_ctrl_pts[idx2]
                    # tmp[0] = tar_pt
                    # all_ctrl_pts[idx2] = tmp
                    if all_ctrl_pts[idx][0] != all_edge_pts[idx][0] or all_ctrl_pts[idx][-1] != all_edge_pts[idx][-1]:
                        pdb.set_trace()
                if res2:
                    tar_pt = ()
                    min_dist = 99
                    for p in res2:
                        if min_dist > abs(p[0] - _p2[0]) + abs(p[1] - _p2[1]):
                            min_dist = abs(p[0] - _p2[0]) + abs(p[1] - _p2[1])
                            tar_pt = p
                    # edge_pt_idx = edge.index(tar_pt)
                    # ctrl_idx = math.ceil(edge_pt_idx / ctrl_length)
                    tmp = all_ctrl_pts[idx]
                    if tar_pt not in tmp:
                        _st_bool = (tmp[0][0] > tar_pt[0], tmp[0][1] > tar_pt[1])
                        ctrl_idx = -2
                        for _t_idx, _t_p in enumerate(tmp):
                            if _st_bool != (_t_p[0] > tar_pt[0], _t_p[1] > tar_pt[1]):
                                ctrl_idx = _t_idx
                                break
                        if ctrl_idx == -2:
                            pdb.set_trace()
                        tmp.insert(ctrl_idx, tar_pt)
                        all_ctrl_pts[idx] = tmp
                        tmp = all_fixed_pts[idx]
                        tmp.insert(ctrl_idx, 0)
                        tmp[max(0, ctrl_idx-1)] = 0.1 if tmp[max(0, ctrl_idx-1)] is not 0 else 0
                        tmp[min(len(tmp)-1, ctrl_idx+1)] = 0.1 if tmp[min(len(tmp)-1, ctrl_idx+1)] is not 0 else 0
                        all_fixed_pts[idx] = tmp
                    else:
                        ctrl_idx = tmp.index(tar_pt)
                        tmp = all_fixed_pts[idx]
                        tmp[ctrl_idx] = 0
                        tmp[max(0, ctrl_idx-1)] = 0.1 if tmp[max(0, ctrl_idx-1)] is not 0 else 0
                        tmp[min(len(tmp)-1, ctrl_idx+1)] = 0.1 if tmp[min(len(tmp)-1, ctrl_idx+1)] is not 0 else 0
                        all_fixed_pts[idx] = tmp
                    # tmp = all_ctrl_pts[idx2]
                    # tmp[-1] = tar_pt
                    # all_ctrl_pts[idx2] = tmp
                    if all_ctrl_pts[idx][0] != all_edge_pts[idx][0] or all_ctrl_pts[idx][-1] != all_edge_pts[idx][-1]:
                        pdb.set_trace()


    for _idx, ctrl_pts in enumerate(all_ctrl_pts):
        # debug = np.zeros((rows, cols)).astype(int)
        fixed_pt = all_fixed_pts[_idx]

        poly_pts = []
        st = time.time()
        rand_scale = 14

        if fixed_pt[0] != 0 or fixed_pt[-1] != 0:
            pdb.set_trace()
        for idx, pts in enumerate(ctrl_pts):
            _1 = math.ceil(ctrl_pts[idx][0] + fixed_pt[idx] * random.uniform(-rand_scale, rand_scale))
            _2 = math.floor(ctrl_pts[idx][1] + fixed_pt[idx] * random.uniform(-rand_scale, rand_scale))
            poly_pts.append(range_check((_1, _2)))

        ed = time.time()
        print('polyline time: ', ed-st)

        st = time.time()
        borderline = []
        for idx in range(len(poly_pts) - 1):
            p0 = poly_pts[idx]
            p1 = poly_pts[idx + 1]
            borderline.append(p0)
            if p0[1] != p1[1]:
                _k = (p0[0] - p1[0]) / (p0[1] - p1[1])
                _min = min(p0[1], p1[1])
                _max = max(p0[1], p1[1])
                tmp = []
                for _j in np.arange(_min, _max, 0.001):
                    _0 = int(round(_k * (_j - p0[1]) + p0[0]))
                    _1 = int(round(_j))
                    _ = range_check((_0, _1))
                    if _ not in borderline:
                        borderline.append(_)
            else:
                _min = min(p0[0], p1[0])
                _max = max(p0[0], p1[0])
                for _i in range(_min, _max):
                    borderline.append((_i, p0[1]))

        if poly_pts[-1] != all_edge_pts[_idx][-1]:
            pdb.set_trace()
        if poly_pts[0] != all_edge_pts[_idx][0]:
            pdb.set_trace()

        borderline.append(poly_pts[-1])
        ed = time.time()
        print('borderline_%d time: ' % _idx, ed-st)

        for idx, pt in enumerate(borderline):
            # debug[pt[0]][pt[1]] = 255
            mask[pt[0]][pt[1]] = 255
            all_debug[pt[0]][pt[1]] = 128

        # for idx, pt in enumerate(poly_pts):
        #     all_debug[pt[0]][pt[1]] = 0

        # cv2.imwrite('%s/edge_poly_%d.png' % (frag_tmp_path, _idx), debug, (int(cv2.IMWRITE_PNG_COMPRESSION), 0))

    for _edge_pts in all_edge_pts:
        all_debug[_edge_pts[0][0]][_edge_pts[0][1]] = 0
        all_debug[_edge_pts[-1][0]][_edge_pts[-1][1]] = 0

    cv2.imwrite('%s/process_%d_0.png'%(frag_tmp_path, new_number), mask, (int(cv2.IMWRITE_PNG_COMPRESSION), 0))
    cv2.imwrite('%s/all_debug.png'%(frag_tmp_path), all_debug, (int(cv2.IMWRITE_PNG_COMPRESSION), 0))

    visited = np.zeros((rows, cols))
    global_visited = visited.copy()
    global_visited = np.logical_or(global_visited, mask)

    y, x = 0, 0
    frag_list = list()
    ground_truth = dict()

    visited_dict = {}
    first_flag = True
    while True:
        if global_visited[y, x] == 0:
            visited = dfs(mask, x, y)
            global_visited = np.logical_or(global_visited, visited)

            f, s = np.where(visited == True)
            ### eliminate orphan points
            if np.size(f) < min_fragment_pixels:
                continue
            f, s = np.mean(f), np.mean(s)

            onepiece = raw * np.dstack((visited, visited, visited)).astype(np.uint8)
            onepiece[visited == 0] = bg_color

            max_side = max(rows, cols)

            pad = np.tile(bg_color, (int(2.5 * max_side), int(2.5 * max_side), 1)).astype(np.uint8)

            pad[max_side:(rows + max_side), max_side:(cols + max_side)] = onepiece
            first, second = f + max_side, s + max_side

            if first_flag:
                r_mat = np.asarray([[1, 0, int(1.25*max_side) - first], [0, 1, int(max_side*1.25) - second]])
                first_flag = False
            else:
                r_mat = cv2.getRotationMatrix2D((first, second), np.random.randint(0, high=360), 1)

            onepiece = Transform.apply_transform(r_mat, pad,
                                                 bg_color=bg_color, numpy_shape=pad.shape)
            #########################

            visited_dict[frag_count] = visited

            # crop the background out, and change the r_mat correspondingly.
            ##  onepiece = crop_the_background_out(r_mat, onepiece, bg_color) ##

            r_mat = np.vstack((r_mat, [0, 0, 1]))
            ground_truth[frag_count] = r_mat

            # cv2.imwrite('fragment_inplace_%04d.png' % frag_count, pad)
            cv2.imwrite('fragment_%04d.png' % frag_count, onepiece)
            # cv2.imwrite('compressed_fragment_%04d.png' % frag_count, onepiece, (cv2.IMWRITE_PNG_COMPRESSION, 2))
            frag_list.append('fragment_%04d.png' % (frag_count))
            frag_count += 1

        if y + 1 < rows:
            y = y + 1

        elif x + 1 < cols:
            x = x + 1
            y = 0

        else:
            break

    # save everything
    with open('fragmentList.txt', 'a+') as f:
        for name in frag_list:
            f.write(name + '\n')

    with open('groundTruth.txt', 'a+') as f:
        for i, mat in ground_truth.items():
            name = Transform.transform2string(np.linalg.pinv(mat))  # make "mat" back in place
            f.write('%d \n' % i)
            f.write(name + '\n')

    adjacent_map = {}
    for i, v1 in visited_dict.items():
        adjacent_map[i] = list()
        for j, v2 in visited_dict.items():
            if i != j and np.argwhere(np.logical_and(v1, v2) == True).size > 3:
                adjacent_map[i].append(j)

    with open('adjacent_map.txt', 'a+') as f:
        for k_map in adjacent_map:
            f.write("query " + str(k_map) + " : ")
            f.write(str(adjacent_map[k_map]) + "\n")

    with open('adjacent_map.pkl', 'ab+') as f:
        pkl.dump(adjacent_map, f)

    with open('gt_matching_global.tr', 'w') as gt_global:
        print('Node ' + '\nNode '.join(map(lambda x: str(x - 1), adjacent_map.keys())), file=gt_global)
        for query in adjacent_map:
            for ans in adjacent_map[query]:
                mat = np.dot(ground_truth[query], np.linalg.pinv(ground_truth[ans]))
                q = query - 1
                a = ans - 1
                mat = Transform.transform2string(mat)
                print('%d %d ' % (q, a), file=gt_global, end='')
                print('100 ', file=gt_global, end='')
                print(mat, file=gt_global)

    cv2.imwrite(os.path.basename(file_name) + '.mask.png', mask, (int(cv2.IMWRITE_PNG_COMPRESSION), 0))
    return frag_count - 1
