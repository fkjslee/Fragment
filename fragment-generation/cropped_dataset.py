import cv2
import numpy as np
import os
import time
import sys
import shutil
import util

class CropDataset():
    def __init__(self, dir, color_file, save_cropped=True):
        self.data_path = dir
        with open(os.path.join(self.data_path, 'fragmentList.txt'), 'r') as f:
            img_list = f.readlines()
        self.bg_color = util.read_bg_color(os.path.join(self.data_path, 'bg_color.txt'))
        img_list = [img.strip('\n') for img in img_list]

        std_dir = os.path.abspath(os.path.join(self.data_path, "std"))

        self.img_list = [os.path.abspath(os.path.join(self.data_path, file)) for file in img_list]
        if not os.path.exists(std_dir):
            os.mkdir(std_dir)

        shutil.copy2(os.path.join(self.data_path, "bg_color.txt"), os.path.join(std_dir, "bg_color.txt"))
        shutil.copy2(os.path.join(self.data_path, "fragmentList.txt"), os.path.join(std_dir, "fragmentList.txt"))

        with open(os.path.join(self.data_path, 'groundTruth.txt'), 'r') as f:
            id = 0
            std_file = open(os.path.join(std_dir, 'groundTruth.txt'), "w")
            for line in f:
                if line.strip().isdigit():
                    id = int(line)
                    std_file.write(line)
                else:
                    mat = self.string2transform(line)
                    img = self.img_list[id - 1]
                    image, offset = self.crop_the_background_out(img)
                    cv2.imwrite(os.path.join(std_dir, os.path.split(img)[1]), image, (cv2.IMWRITE_PNG_COMPRESSION, 0))
                    trans_mat = np.eye(3)
                    trans_mat[0, 2] = offset[0]
                    trans_mat[1, 2] = offset[1]

                    mat = np.dot(mat, trans_mat)

                    std_file.write(self.transform2string(mat) + '\n')

    def transform2string(self, np_matrix):
        return np.array2string(np_matrix, suppress_small=True, formatter={'float_kind': lambda x: '%.6f' % x}).replace(
            "[",
            ""). \
            replace("]", "").replace(". ", "").replace("\n", " ").replace(".000000", "")

    def string2transform(self, string):
        splitted = string.split()
        if len(splitted) != 9:
            raise Exception(
                'Something wrong with the string2transform, size: ' + str(len(splitted)) + ' is not 9! ' + string)
        return np.asarray(list(map(float, splitted))).reshape([3, 3])

    @staticmethod
    def guessBGcolor(filename):
        image = cv2.imread(filename)
        color_list = list(map(tuple, [image[0, 0], image[0, -1], image[-1, -1], image[-1, 0]]))
        counts = [color_list.count(color) for color in color_list]
        return color_list[counts.index(max(counts))]


    def crop_the_background_out(self, file_name):
        image = cv2.imread(file_name)
        bg_color = self.bg_color
        # to extract the bounding box of img
        try:
            rows, cols, _ = image.shape
        except:
            raise ValueError("file not valid:" + str(self.file_name))
        bg_mask = util.get_bg_mask(image, self.bg_color)
        fg_mask = np.logical_not(bg_mask)

        min_y, max_y = np.min(np.where(fg_mask)[0]), np.max(np.where(fg_mask)[0])
        min_x, max_x = np.min(np.where(fg_mask)[1]), np.max(np.where(fg_mask)[1])

        offset = [max(1, min_y - 20), max(1, min_x - 20)]

        fg_mask = fg_mask[offset[0]:max_y + 20, offset[1]:max_x + 20]
        image = image[offset[0]:max_y + 20, offset[1]:max_x + 20].astype(np.uint8)
        return (image, offset)



if __name__ == "__main__":
    CropDataset(sys.argv[1], "bg_color.txt")
