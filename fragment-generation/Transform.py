from skimage import transform as sktrans
import cv2
import numpy as np


def estimate_transform(src_points, dst_points, with_errors=False):
    tform = sktrans.estimate_transform('euclidean', src_points, dst_points)
    if with_errors:
        return tform.params, tform.residuals(src_points, dst_points)
    else:
        return tform.params


def get_bbox(src1, src2, mat1, mat2):
    p1 = np.zeros((4, 2))
    p1[1] = [src1.shape[0], 0]
    p1[2] = [0, src1.shape[1]]
    p1[3] = [src1.shape[0], src1.shape[1]]
    p2 = np.zeros((4, 2))
    p2[1] = [src2.shape[0], 0]
    p2[2] = [0, src2.shape[1]]
    p2[3] = [src2.shape[0], src2.shape[1]]

    p1_prime = sktrans.matrix_transform(p1, mat1)
    p2_prime = sktrans.matrix_transform(p2, mat2)
    min_bounding = np.min(np.vstack((p1_prime, p2_prime)), axis=0).astype(np.int32)
    max_bounding = np.max(np.vstack((p1_prime, p2_prime)), axis=0).astype(np.int32)

    print("bouding:\n ", min_bounding, max_bounding)
    return min_bounding, max_bounding

def transform_then_offset(mat, offset):
    off_mat = np.eye(3)
    off_mat[0,2] = offset[0]
    off_mat[1,2] = offset[1]
    new_mat = np.dot(off_mat, mat)
    return new_mat

def extract_fg_coords(src_img, bg_color):
    fg = np.logical_not(np.all(src_img == bg_color, axis=2))
    return np.unique(np.vstack(np.nonzero(fg)).T, axis=0)



def transform_coords(mat, np_coords):
    '''
    :param mat: 3x3 mat
    :param np_coords: numpy coordinates, of shape n x 2
    :return: return transformed coords, type int32
    '''
    return sktrans.matrix_transform(np_coords, mat).astype(np.int32)

def invert_matrix(m):
    m[0, 1] *= -1
    m[1, 0] *= -1
    t = m[0:2, 2]
    m[0, 2] = t[1, 2]
    m[1, 2] = t[0, 2]
    return t


def apply_transform(t, src_image, bg_color, numpy_shape=None):
    theta = np.arctan2(t[1, 0], t[0, 0])
    m = cv2.getRotationMatrix2D((0, 0), theta * 180 / np.pi, 1)
    m[0, 2] = m[0, 2] + t[1, 2]
    m[1, 2] = m[1, 2] + t[0, 2]
    if numpy_shape is None:
        transformed_img = cv2.warpAffine(src_image, m, (src_image.shape[1], src_image.shape[0]),
                                         flags=cv2.INTER_NEAREST,
                                         borderValue=bg_color)
    else:
        transformed_img = cv2.warpAffine(src_image, m, (numpy_shape[1], numpy_shape[0]),
                                         flags=cv2.INTER_NEAREST,
                                         borderValue=bg_color)
    return transformed_img


def transform2string(np_matrix):
    return np.array2string(np_matrix, suppress_small=True, formatter={'float_kind': lambda x: '%.6f' % x}).replace("[",
                                                                                                                   ""). \
        replace("]", "").replace(". ", "").replace("\n", " ").replace(".000000", "")


def string2transform(string):
    splitted = string.split()
    if len(splitted) != 9:
        raise Exception(
            'Something wrong with the string2transform, size: ' + str(len(splitted)) + ' is not 9! ' + string)

    return np.asarray(list(map(float, splitted))).reshape([3, 3])
