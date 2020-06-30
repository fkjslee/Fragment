import os
import sys
import numpy as np
import cv2
import glob
import re


def get_inner_folders(path):
    inner_folder_list = [os.path.join(os.path.abspath(path), x) for x in os.listdir(path)]
    inner_folder_list = [x for x in inner_folder_list if os.path.isdir(x)]
    return inner_folder_list


def get_file_name(prefix, ext=None):  # no ext means dir name
    if ext:
        ext = "." + ext
    else:
        ext = ""
    files = glob.glob("{}_*{}".format(prefix, ext))
    if not files:
        return "{}_1{}".format(prefix, ext)
    indexes = [re.search("{}_(.*){}".format(prefix, ext), file).group(1) for file in files]
    nxt_idx = max(map(int, indexes)) + 1
    return "{}_{}{}".format(prefix, nxt_idx, ext)


def change_black_to_white_color(img, bg_color=(0, 0, 0)):
    if list(img[-1, -1, :]) == [255, 255, 255]:
        return img

    bg_mask1 = np.logical_and(np.logical_and(img[:, :, 0] == bg_color[0], img[:, :, 1] == bg_color[1]),
                              img[:, :, 2] == bg_color[2])

    img[bg_mask1] = 255
    return img


def kp_construct(corner_l):
    kp = []
    for each_corner in corner_l:
        kp.append(cv2.KeyPoint(x=each_corner[1], y=each_corner[0], _size=15))
    return kp


def drawMatching(strongest_string, pairData, q, r, bg_color=None):
    match = list()
    for i in range(len(strongest_string[0])):
        i_match = cv2.DMatch(i, i, 100)
        match.append(i_match)

    kp1 = kp_construct([i.center + pairData.img_descriptor_1.offset for i in strongest_string[0]])
    kp2 = kp_construct([i.center + pairData.img_descriptor_2.offset for i in strongest_string[1]])
    # out = cv2.drawMatches( pairData.img_descriptor_2.image, kp2, pairData.img_descriptor_1.image, kp1, match, None)
    out = draw_matches(pairData.img_descriptor_1.image, kp1, pairData.img_descriptor_2.image, kp2, match,
                       bg_color=bg_color)
    cv2.imwrite(os.path.join("matching_%d_%d.png" % (q, r)), out)


def draw_matches(img1, kp1, img2, kp2, matches, bg_color=None, color=None):
    """Draws lines between matching keypoints of two images.
    Keypoints not in a matching pair are not drawn.
    Places the images side by side in a new image and draws circles
    around each keypoint, with line segments connecting matching pairs.
    You can tweak the r, thickness, and figsize values as needed.
    Args:
        img1: An openCV image ndarray in a grayscale or color format.
        kp1: A list of cv2.KeyPoint objects for img1.
        img2: An openCV image ndarray of the same format and with the same
        element type as img1.
        kp2: A list of cv2.KeyPoint objects for img2.
        matches: A list of DMatch objects whose trainIdx attribute refers to
        img1 keypoints and whose queryIdx attribute refers to img2 keypoints.
        color: The color of the circles and connecting lines drawn on the images.
        A 3-tuple for color images, a scalar for grayscale images.  If None, these
        values are randomly generated.
    """
    # We're drawing them side by side.  Get dimensions accordingly.
    # Handle both color and grayscale images.
    if len(img1.shape) == 3:
        new_shape = (max(img1.shape[0], img2.shape[0]), img1.shape[1] + img2.shape[1], img1.shape[2])
    elif len(img1.shape) == 2:
        new_shape = (max(img1.shape[0], img2.shape[0]), img1.shape[1] + img2.shape[1])
    new_img = np.ones(new_shape, type(img1.flat[0]))
    if bg_color:
        new_img = new_img * np.array(bg_color)
    # Place images onto the new image.
    new_img[0:img1.shape[0], 0:img1.shape[1]] = img1
    new_img[0:img2.shape[0], img1.shape[1]:img1.shape[1] + img2.shape[1]] = img2

    # Draw lines between matches.  Make sure to offset kp coords in second image appropriately.
    r = 15
    thickness = 2
    if color:
        c = color
    for m in matches:
        # Generate random color for RGB/BGR and grayscale images as needed.
        if not color:
            c = np.random.randint(0, 256, 3) if len(img1.shape) == 3 else np.random.randint(0, 256)
        # So the keypoint locs are stored as a tuple of floats.  cv2.line(), like most other things,
        # wants locs as a tuple of ints.
        end1 = tuple(np.round(kp1[m.trainIdx].pt).astype(int))
        end2 = tuple(np.round(kp2[m.queryIdx].pt).astype(int) + np.array([img1.shape[1], 0]))
        c = c.astype(float)
        cv2.line(new_img, end1, end2, c, thickness=thickness)
        cv2.circle(new_img, end1, r, c, thickness)
        cv2.circle(new_img, end2, r, c, thickness)
    return new_img


def get_bg_mask(image, bg_color):
    bg_mask = np.all(image == bg_color, axis=2)
    return bg_mask


def get_fg_mask(image, bg_color):
    return np.logical_not(get_bg_mask(image, bg_color))


def merge_two_images(src1, src2, bg_color):
    assert src1.shape == src2.shape, "shape not match! not mergable"
    fg1 = get_fg_mask(src1, bg_color)
    fg2 = get_fg_mask(src2, bg_color)
    new_img = src1 * fg1[..., np.newaxis].astype(np.int32) + src2 * fg2[..., np.newaxis].astype(np.int32)
    new_img[np.logical_not(np.logical_or(fg1, fg2))] = bg_color
    return new_img

def read_bg_color(colorfile):
    with open(colorfile, 'r') as f:
        rgb = f.read()
        rgb = rgb.split(' ')
        rgb.reverse()
        bg_color = [int(s) for s in rgb]
    print("read in bg_color: {}".format(bg_color))
    return bg_color


if __name__ == "__main__":
    file = sys.argv[1]
    change_black_to_white_color(file)
