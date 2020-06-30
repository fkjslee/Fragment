import numpy as np
import cv2
import pdb


def calc_trans_mat(dirname, pic1, pic2, x, y, r, score):
    src=cv2.imread('./' + dirname + '/fragment_%04d.png' % int(pic1))
    dst=cv2.imread('./' + dirname + '/fragment_%04d.png' % int(pic2))

    rot = cv2.getRotationMatrix2D((dst.shape[1]/2, dst.shape[0]/2), r, 1.0)
    rot_mat = np.identity(3)
    rot_mat[0][0] = rot[0][0]
    rot_mat[0][1] = rot[1][0]
    rot_mat[1][0] = rot[0][1]
    rot_mat[1][1] = rot[1][1]
    rot_mat[0][2] = rot[1][2]
    rot_mat[1][2] = rot[0][2]
    trans = np.array([[1, 0, x], [0, 1, y], [0,0,1]])
    trans += np.array([[0, 0, (src.shape[0] - dst.shape[0]) / 2], [0, 0, (src.shape[1] - dst.shape[1]) / 2], [0, 0, 0]])
    trans = np.matmul(trans, rot_mat)
    res = np.zeros((4, 3))
    res[0] = [pic1-1, pic2-1, score]
    res[1:] = trans
    return res

if __name__ == "__main__":
    dirname = 'mixed2'
    filename = dirname + '_calc_res.txt'
    data = np.loadtxt(filename)
    assert (data[:, 2:5] == 0).all()
    pairs = data[:, 0:2]
    trans = data[:, 5:8]
    score = data[:, 8:]
    final_res = np.zeros((4 * len(pairs), 3))
    for i in range(len(pairs)):
        final_res[i*4:i*4+4] = calc_trans_mat(dirname, pairs[i][0], pairs[i][1], trans[i][0], trans[i][1], trans[i][2], score[i][0])
    np.savetxt('./' + dirname + '/' + filename[:-4] + '_np_res.txt', final_res)



