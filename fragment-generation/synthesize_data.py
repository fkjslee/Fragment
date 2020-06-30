import os
import shutil
import cv2
import numpy as np
from scipy.interpolate import interp1d
import pickle as pkl
import Transform

min_fragment_pixels = 60 ** 2


def slicer(file_name, h, v, rand_scale, h_rand_angle, v_rand_angle, ctrl_length,
           starting=1, color_file=None):
    frag_count = starting

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

    # adjusting the variations according to image size
    h_rand_scale, v_rand_scale = rows * rand_scale * (4.0 / h), cols * rand_scale * (4.0 / v)
    h_rand_angle, v_rand_angle = h_rand_angle * (3.0 / h), v_rand_angle * (3.0 / v)

    mask = np.zeros((rows, cols)).astype(np.uint8)
    h_lines = h - 1
    v_lines = v - 1

    def dfs(mask, x, y):

        '''
        with open('log.txt','ab') as f:
            f.write('%s %s\n' %(x, y))
        '''
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

    for i in range(h_lines):
        s = np.random.normal(0, h_rand_scale, cols // ctrl_length)
        # s = np.array([np.sin(i / (cols // ctrl_length) * 2 * 3.14) for i in range(cols // ctrl_length)])
        off_angel = np.random.normal(0, h_rand_angle)
        for k, val in enumerate(s):
            if k > 0:
                s[k] = s[k - 1] + val + k * off_angel
        mu = (i + 1.0) / (h_lines + 1) * rows
        h = np.ceil(s + mu)
        h = list(h)
        for i in range(len(h)):
            if h[i] < 0:
                h[i] = 0
            elif h[i] >= rows:
                h[i] = rows

        x = np.linspace(0, cols, num=cols // ctrl_length)
        f = interp1d(x, h, kind='slinear')
        line = f(range(cols))
        line = line.astype(np.int32)
        for i, ycor in enumerate(line):
            if 0 <= ycor < rows:
                mask[ycor, i] = 255
            if i > 0 and abs(ycor - line[i - 1]) > 1:
                mask[range(*sorted([line[i - 1], ycor])), i] = 255

    for i in range(v_lines):
        s = np.random.normal(0, v_rand_scale, rows // ctrl_length)
        off_angel = np.random.normal(0, v_rand_angle)
        for k, val in enumerate(s):
            if k > 0:
                s[k] = s[k - 1] + val + k * off_angel
        mu = (i + 1.0) / (v_lines + 1) * cols
        h = np.ceil(s + mu)
        h = list(h)
        for i in range(len(h)):
            if h[i] < 0:
                h[i] = 0
            elif h[i] >= cols:
                h[i] = cols
        y = np.linspace(0, rows, num=rows // ctrl_length)
        f = interp1d(y, h, kind='slinear')
        line = f(range(rows))
        line = line.astype(np.int32)
        for i, xcor in enumerate(line):
            if 0 <= xcor < cols:
                mask[i, xcor] = 255
            if i > 0 and abs(xcor - line[i - 1]) > 1:
                mask[i, range(*sorted([line[i - 1], xcor]))] = 255

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
