import os
import cv2
import numpy as np


def find_bg_color(file_list):
    raw_list = [cv2.imread(file_name)//16 for file_name in file_list]

    color_space = np.zeros((16, 16, 16))

    def bfs(color_space, i, j, k):
        visited = np.zeros(color_space.shape)
        visited[i, j, k] = 1
        x, y, z = i, j, k
        queue = []
        queue.append((i, j, k))

        while True:
            i, j, k = queue.pop(0)

            if color_space[i, j, k] == 1:
                distance = np.linalg.norm(np.asarray([x, y, z]) - np.asarray([i, j, k]))
                break

            if i - 1 >= 0 and visited[i - 1, j, k] == 0:
                queue.append((i - 1, j, k))
                visited[i - 1, j, k] = 1

            if i + 1 <= 15 and visited[i + 1, j, k] == 0:
                queue.append((i + 1, j, k))
                visited[i + 1, j, k] = 1

            if j - 1 >= 0 and visited[i, j - 1, k] == 0:
                queue.append((i, j - 1, k))
                visited[i, j - 1, k] = 1

            if j + 1 <= 15 and visited[i, j + 1, k] == 0:
                queue.append((i, j + 1, k))
                visited[i, j + 1, k] = 1

            if k - 1 >= 0 and visited[i, j, k - 1] == 0:
                queue.append((i, j, k - 1))
                visited[i, j, k - 1] = 1

            if k + 1 <= 15 and visited[i, j, k + 1] == 0:
                queue.append((i, j, k + 1))
                visited[i, j, k + 1] = 1

        return distance

    for raw in raw_list:
        rows, cols, chns = raw.shape
        for i in range(rows):
            # print np.count_nonzero(color_space)
            for j in range(cols):
                color_vec = raw[i, j]
                if color_space[tuple(color_vec)] == 0:
                    color_space[tuple(color_vec)] = 1  # 1 means this color is shown

    color_distance = np.zeros((16, 16, 16))

    for i in range(16):
        for j in range(16):
            for k in range(16):
                if color_space[i, j, k] == 1:
                    color_distance[i, j, k] = 0
                else:
                    d = bfs(color_space, i, j, k)
                    color_distance[i, j, k] = d

    assert np.count_nonzero(color_distance) + np.count_nonzero(color_space) == 16 ** 3, "distance wrong!"

    max_index = np.unravel_index(color_distance.argmax(), color_distance.shape)

    no_show_nearest = np.asarray(max_index)

    # expand it to 256 scale
    no_show_nearest = no_show_nearest * 16 + 8

    color_file = 'bg_color.txt'
    with open(color_file, 'w') as f:
        f.write('%s %s %s' % (no_show_nearest[2], no_show_nearest[1], no_show_nearest[0]))

    return color_file
