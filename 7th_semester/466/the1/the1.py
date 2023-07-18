# Mustafa Sezgin
# 2380863

import os
import skimage.exposure
import skimage.io
import skimage.transform
import skimage.util
import numpy as np
import matplotlib.pyplot as plt

INPUT_PATH = "./THE1_Images/"
OUTPUT_PATH = "./Outputs/"

def read_image(img_path, rgb = True):
    return skimage.io.imread(img_path, not rgb)

def write_image(img, output_path):
    # skimage.io.imsave(output_path, (img * 255).astype(np.uint8))
    img = skimage.util.img_as_ubyte(img)
    skimage.io.imsave(output_path, img)
    return

def rotate_image(img,  degree = 0, interpolation_type = "linear"):
    if interpolation_type == "linear":
        return skimage.transform.rotate(img, degree, order=1)
    elif interpolation_type == "cubic":
        return skimage.transform.rotate(img, degree, order=3)
    else:
        return skimage.transform.rotate(img, degree, order=0)

def extract_save_histogram(img, path):
    # hist, bin_centers = skimage.exposure.histogram(img)
    plt.hist(img.ravel(), bins=20)
    plt.savefig(path)
    plt.clf()
    return

def histogram_equalization(img):
    return skimage.exposure.equalize_hist(img)

def adaptive_histogram_equalization(img):
    return skimage.exposure.equalize_adapthist(img)

if __name__ == '__main__':
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)

    # PART1
    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 45, "linear")
    write_image(output, OUTPUT_PATH + "a1_45_linear.png")

    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 45, "cubic")
    write_image(output, OUTPUT_PATH + "a1_45_cubic.png")

    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 90, "linear")
    write_image(output, OUTPUT_PATH + "a1_90_linear.png")

    img = read_image(INPUT_PATH + "a1.png")
    output = rotate_image(img, 90, "cubic")
    write_image(output, OUTPUT_PATH + "a1_90_cubic.png")

    img = read_image(INPUT_PATH + "a2.png")
    output = rotate_image(img, 45, "linear")
    write_image(output, OUTPUT_PATH + "a2_45_linear.png")

    img = read_image(INPUT_PATH + "a2.png")
    output = rotate_image(img, 45, "cubic")
    write_image(output, OUTPUT_PATH + "a2_45_cubic.png")

    # PART2
    img = read_image(INPUT_PATH + "b1.png", rgb = False)
    extract_save_histogram(img, OUTPUT_PATH + "original_histogram.png")
    equalized = histogram_equalization(img)
    extract_save_histogram(equalized, OUTPUT_PATH + "equalized_histogram.png")
    write_image(equalized, OUTPUT_PATH + "enhanced_image.png")

    # BONUS
    # Define the following function
    equalized = adaptive_histogram_equalization(img)
    extract_save_histogram(equalized, OUTPUT_PATH + "adaptive_equalized_histogram.png")
    write_image(equalized, OUTPUT_PATH + "adaptive_enhanced_image.png")
