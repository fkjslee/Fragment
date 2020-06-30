import os
import cv2
import shutil
import sys
from synthesize_data import slicer
from find_bg_color_bfs import find_bg_color
from cropped_dataset import *
from synthesize_skew import slicer_skew

def batch_proc_skew(pieces):
    file_list = os.listdir(os.path.curdir)
    img_list = [os.path.abspath(f) for f in file_list if f.lower().endswith(('JPEG','jpg','png'))]

    for each_f in img_list:

        # make new folder for all
        new_folder = os.path.splitext(each_f)[0]
        if os.path.exists(new_folder):
            shutil.rmtree(new_folder)
            # time.sleep(0.1)
        os.mkdir(new_folder)
        os.chdir(new_folder)
        c_file = find_bg_color([each_f])

        ending = slicer_skew(each_f, pieces, rand_angle=0.01, ctrl_length=12, color_file=c_file)
        print(ending, "pieces get")
        # if ending != (h_cuts) * (v_cuts):
        #     os.chdir(os.pardir)
        #     batch_proc(h_cuts, v_cuts)
        #     break
        CropDataset(".", c_file)

def batch_proc(h_cuts, v_cuts, data_path='\\mixed'):
    # read in all image in that folder
    file_list = os.listdir(os.path.curdir)
    img_list = [os.path.abspath(f) for f in file_list if f.lower().endswith(('JPEG','jpg','png'))]

    for each_f in img_list:

        # make new folder for all
        new_folder = os.path.splitext(each_f)[0]
        if os.path.exists(new_folder):
            shutil.rmtree(new_folder)
            # time.sleep(0.1)
        os.mkdir(new_folder)
        os.chdir(new_folder)
        c_file = find_bg_color([each_f])

        ending = slicer(each_f, h_cuts, v_cuts, rand_scale=7/1000,
                        h_rand_angle=0.01, v_rand_angle=0.01, ctrl_length=12,
                        color_file=c_file)
        print(ending, "pieces get")
        # if ending != (h_cuts) * (v_cuts):
        #     os.chdir(os.pardir)
        #     batch_proc(h_cuts, v_cuts)
        #     break
        CropDataset(".", c_file)


if __name__ == "__main__":
    print(" The dir is ", sys.argv[1])
    os.chdir(sys.argv[1])

    if len(sys.argv) == 4 and sys.argv[3].isdigit():
        h_cuts = int(sys.argv[2])
        v_cuts = int(sys.argv[3])
        batch_proc(h_cuts, v_cuts)
        os.chdir(os.path.pardir)
    elif len(sys.argv) == 3:
        total_pieces = int(sys.argv[2])
        batch_proc_skew(total_pieces)
        os.chdir(os.path.pardir)
    else:
        raise NotImplementedError("To be implemented")
