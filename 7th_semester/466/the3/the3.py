# Mustafa Sezgin
# 2380863

import os
import numpy as np
import skimage.io
import skimage.color
import skimage.filters
import skimage.measure
import skimage.util
import sklearn.cluster
import matplotlib.pyplot as plt


INPUT_PATH = "./THE3_Images/"
OUTPUT_PATH = "./Outputs/"


def read_image(img_path, rgb = True):
    return skimage.io.imread(img_path, not rgb)

def write_image(img, output_path):
    # # skimage.io.imsave(output_path, (img * 255).astype(np.uint8))
    # img = img.astype(np.uint8)
    img = skimage.util.img_as_ubyte(img)
    skimage.io.imsave(output_path, img)
    return

def detect_faces_1(img, k, skin_color):
    m = img.shape[0]
    n = img.shape[1]

    # create kmeans object
    kmeans = sklearn.cluster.KMeans(n_clusters=k)
    kmeans.fit(img.reshape((m * n, 3)))

    # get the clusters
    clusters = kmeans.labels_.reshape(m, n)

    # get the index of cluster that has color closest to face skin color
    cluster_colors = np.copy(kmeans.cluster_centers_)
    closest_color_i = np.argmin(np.sum(np.abs(cluster_colors - skin_color), axis=1))

    # image with only selected cluster
    cluster = np.zeros((m, n))
    cluster[clusters == closest_color_i] = 1

    # partition the cluster into regions
    labels_cluster = skimage.measure.label(cluster)
    regions = skimage.measure.regionprops(labels_cluster)

    # bounding boxes of several regions
    plt.figure(figsize=(10, 10))
    plt.imshow(img)
    plt.axis("off")
    for j, props in enumerate(regions):
        # if props.area < 400 or props.area > 1600:
        if props.area < 450 or (props.area > 500 and props.area < 625) or (props.area > 700 and props.area < 1500) or props.area > 1600:
            continue
        minr, minc, maxr, maxc = props.bbox
        bx = (minc, maxc, maxc, minc, minc)
        by = (minr, minr, maxr, maxr, minr)
        plt.plot(bx, by, '-b', linewidth=2.5)

    # save the image
    plt.savefig(OUTPUT_PATH + "1_faces.png", bbox_inches='tight')
    plt.show()

    # code that draws bounding boxes of several regions on image 1_source.png
    # (used for the report)
    ############################################################################
    # for j, props in enumerate(regions):
    #     # if props.area < 450 or props.area > 1600:
    #     if props.area < 450 or (props.area > 500 and props.area < 625) or (props.area > 700 and props.area < 1500) or props.area > 1600:
    #         continue
    #     img_region = np.zeros((m, n))
    #     img_region[labels_cluster == (j + 1)] = 1
    #     plt.figure(figsize=(10, 10))
    #     plt.imshow(img_region, cmap=plt.cm.gray)
    #     plt.axis("off")
    #     minr, minc, maxr, maxc = props.bbox
    #     bx = (minc, maxc, maxc, minc, minc)
    #     by = (minr, minr, maxr, maxr, minr)
    #     plt.plot(bx, by, '-b', linewidth=2.5)
    #     plt.show()
    ############################################################################

def detect_faces_2(img, k):
    m = img.shape[0]
    n = img.shape[1]

    # create kmeans object
    kmeans = sklearn.cluster.KMeans(n_clusters=k)
    kmeans.fit(img.reshape((m * n, 3)))

    # get the clusters
    clusters = kmeans.labels_.reshape(m, n)

    plt.figure(figsize=(10, 10))
    plt.imshow(img)
    plt.axis("off")
    for i in range(k):
        # image with only selected cluster
        cluster_i = np.zeros((img.shape[0], img.shape[1]))
        cluster_i[clusters == i] = 1

        # partition the cluster into regions
        labels_cluster_i = skimage.measure.label(cluster_i)
        regions = skimage.measure.regionprops(labels_cluster_i)

        # bounding boxes of several regions
        for j, props in enumerate(regions):
            if props.area < 60000 or (props.area > 70000 and props.area < 140000) or (props.area > 160000 and props.area < 1000000) or props.area > 1100000:
                continue
            minr, minc, maxr, maxc = props.bbox
            bx = (minc, maxc, maxc, minc, minc)
            by = (minr, minr, maxr, maxr, minr)
            plt.plot(bx, by, '-b', linewidth=2.5)

    # save the image
    plt.savefig(OUTPUT_PATH + "2_faces.png", bbox_inches='tight')
    plt.show()

    # code that draws bounding boxes of several regions on image 2_source.png
    # (used for the report)
    ############################################################################
    # for i in range(k):
    #     # image with only selected cluster
    #     cluster_i = np.zeros((img.shape[0], img.shape[1]))
    #     cluster_i[clusters == i] = 1

    #     # partition the cluster into regions
    #     labels_cluster_i = skimage.measure.label(cluster_i)
    #     regions = skimage.measure.regionprops(labels_cluster_i)

    #     # bounding boxes of several regions
    #     for j, props in enumerate(regions):
    #         if props.area < 60000 or (props.area > 70000 and props.area < 140000) or (props.area > 160000 and props.area < 1000000) or props.area > 1100000:
    #             continue
    #         img_region = np.zeros((m, n))
    #         img_region[labels_cluster_i == (j + 1)] = 1
    #         plt.figure(figsize=(10, 10))
    #         plt.imshow(img_region, cmap=plt.cm.gray)
    #         plt.axis("off")
    #         minr, minc, maxr, maxc = props.bbox
    #         bx = (minc, maxc, maxc, minc, minc)
    #         by = (minr, minr, maxr, maxr, minr)
    #         plt.plot(bx, by, '-b', linewidth=2.5)
    #         plt.show()
    ############################################################################

def detect_faces_3(img, k, skin_color):
    m = img.shape[0]
    n = img.shape[1]

    # create kmeans object
    kmeans = sklearn.cluster.KMeans(n_clusters=k)
    kmeans.fit(img.reshape((m * n, 3)))

    # get the clusters
    clusters = kmeans.labels_.reshape(m, n)

    # get the index of cluster that has color closest to face skin color
    cluster_colors = np.copy(kmeans.cluster_centers_)
    closest_color_i = np.argmin(np.sum(np.abs(cluster_colors - skin_color), axis=1))

    # image with only selected cluster
    cluster = np.zeros((m, n))
    cluster[clusters == closest_color_i] = 1

    # partition the cluster into regions
    labels_cluster = skimage.measure.label(cluster)
    regions = skimage.measure.regionprops(labels_cluster)

    # find the largest connected region
    largest_region_area = 0
    largest_region_i = 0
    for i, props in enumerate(regions):
        if props.area > largest_region_area:
            largest_region_area = props.area
            largest_region_i = i

    # bounding box of selected region
    prop = regions[largest_region_i]
    minr, minc, maxr, maxc = prop.bbox
    bx = (minc, maxc, maxc, minc, minc)
    by = (minr, minr, maxr, maxr, minr)

    # save the image

    plt.figure(figsize=(10, 10))
    plt.imshow(img)
    plt.axis("off")
    plt.plot(bx, by, '-b', linewidth=2.5)
    plt.savefig(OUTPUT_PATH + "3_faces.png", bbox_inches='tight')
    plt.show()

def color_images_1(gray_img, rgb_img):
    """
    Alternative 1:
    - converts RGB source image to gray image
    - calculates the mean of R, G and B values converted to the same gray level
    - on target gray image
       - uses the gray level to get mapped R, G and B values
       - creates pseudo-colored RGB image
    """

    m = gray_img.shape[0]
    n = gray_img.shape[1]

    # convert the rgb image to gray image
    rgb_gray_img = skimage.util.img_as_ubyte(skimage.color.rgb2gray(rgb_img))

    # create a color map from gray to color
    # usage: (r, g, b) = color_map[g]
    color_map = np.empty((256, 3))

    # average of red, green and blue values of
    # the pixels converted to the same gray level
    for l in range(256):
        pixels_l = rgb_img[rgb_gray_img == l]
        if pixels_l.shape[0] == 0:
            color_map[l] = np.array([0, 0, 0])
        else:
            color_map[l] = np.mean(pixels_l, axis=0)

    color_map = color_map.astype(np.uint8)

    # create the pseudo-colored rgb image
    colored_img = np.empty((m, n, 3), dtype=np.uint8)
    for l in range(256):
        colored_img[gray_img == l] = color_map[l]

    return colored_img

def color_images_2(gray_img, rgb_img):
    """
    Alternative 2:
    - converts RGB source image to HSV
    - calculates the mean of H and S values with the same V value
    - on target gray image
       - uses the gray level as V value and gets mapped H and S values
       - creates pseudo-colored HSV image
       - converts it to RGB
    """

    m = gray_img.shape[0]
    n = gray_img.shape[1]

    # convert the rgb image to hsv image
    hsv_img = skimage.util.img_as_ubyte(skimage.color.rgb2hsv(rgb_img))

    # create a color map from gray to color
    # usage: (h, s, v) = color_map[g]
    color_map = np.empty((256, 3))

    # average of hue and saturation values of
    # the pixels with the same value value
    for l in range(256):
        pixels_l = hsv_img[hsv_img[:, :, 2] == l]
        if pixels_l.shape[0] == 0:
            color_map[l] = np.array([0, 0, l])
        else:
            color_map[l] = np.mean(pixels_l, axis=0)

    color_map = color_map.astype(np.uint8)

    # create the pseudo-colored hsv image
    colored_img = np.empty((m, n, 3))
    for l in range(256):
        colored_img[gray_img == l] = color_map[l]

    # convert to rgb
    colored_img = skimage.color.hsv2rgb(colored_img).astype(np.uint8)
    return colored_img

def color_images(gray_img, rgb_img, alt = 1):
    if alt == 1:
        return color_images_1(gray_img, rgb_img)
    elif alt == 2:
        return color_images_2(gray_img, rgb_img)
    else:
        return

def plot_channels(img):
    # plot RGB channels
    plt.figure()
    for i in range(3):
        ch_img = np.zeros_like(img)
        ch_img[:, :, i] = img[:, :, i]
        plt.subplot(1, 3, i + 1)
        plt.axis("off")
        plt.imshow(ch_img)
    plt.show()

    # plot HSV channels
    img = skimage.color.rgb2hsv(img)
    plt.figure()
    plt.subplot(1, 3, 1)
    plt.imshow(img[:, :, 0], cmap=plt.cm.hsv)
    plt.axis("off")
    plt.subplot(1, 3, 2)
    plt.imshow(img[:, :, 1], cmap=plt.cm.Greys)
    plt.axis("off")
    plt.subplot(1, 3, 3)
    plt.imshow(img[:, :, 2], cmap=plt.cm.gray)
    plt.axis("off")
    plt.show()
    return

def detect_edges(img, e_type = "rgb"):
    if e_type == "rgb":
        # detect edges on RGB channels
        img = skimage.util.img_as_float64(img)
        edge_img = np.empty_like(img)
        for i in range(3):
            edge_img[:, :, i] = skimage.filters.sobel(img[:, :, i])
    elif e_type == "hsv":
        # detect edges on HSV intensity channel
        img = skimage.color.rgb2hsv(img)
        edge_img = np.empty_like(img)
        edge_img[:, :, 0] = img[:, :, 0]
        edge_img[:, :, 1] = img[:, :, 1]
        edge_img[:, :, 2] = skimage.filters.sobel(img[:, :, 2])
        edge_img = skimage.color.hsv2rgb(edge_img)

    return edge_img


if __name__ == "__main__":
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)

    # QUESTION 1
    img = read_image(INPUT_PATH + "1_source.png")
    detect_faces_1(img[:, :, 0:3], k=4, skin_color=np.array([248, 230, 199]))

    img = read_image(INPUT_PATH + "2_source.png")
    detect_faces_2(img, k=4)

    img = read_image(INPUT_PATH + "3_source.png")
    detect_faces_3(img, k=4, skin_color=np.array([187, 154, 136]))

    # QUESTION 2
    for i in range(4):

        # Step 3
        gray_img = read_image(INPUT_PATH + f"{i+1}.png", rgb=False)
        src_img = read_image(INPUT_PATH + f"{i+1}_source.png")[:, :, 0:3]
        # use alternative 1 or 2
        alt = 1
        colored_img = color_images(gray_img, src_img, alt)
        write_image(colored_img, OUTPUT_PATH + f"{i+1}_colored.png")

        # Step 5
        plot_channels(colored_img)

        # Step 6
        # use e_type as "rgb" or "hsv"
        e_type = "rgb"
        edge_img = detect_edges(src_img, e_type)
        write_image(edge_img, OUTPUT_PATH + f"{i+1}_colored_edges.png")
