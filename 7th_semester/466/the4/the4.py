# Mustafa Sezgin
# 2380863

import os
import numpy as np
import skimage.io
import skimage.color
import skimage.draw
import skimage.future.graph
import skimage.measure
import skimage.morphology
import skimage.segmentation
import skimage.util
import sklearn.cluster
import matplotlib.pyplot as plt


INPUT_PATH = "./THE4_Images/"
OUTPUT_PATH = "./Outputs/"


def read_image(img_path, rgb = True):
    return skimage.io.imread(img_path, not rgb)

def write_image(img, output_path):
    # # skimage.io.imsave(output_path, (img * 255).astype(np.uint8))
    # img = img.astype(np.uint8)
    img = skimage.util.img_as_ubyte(img)
    skimage.io.imsave(output_path, img)
    return

def count_objects(img, i, thresh, r_selem):
    m = img.shape[0]
    n = img.shape[1]

    img_obj = img[:, :, 0]
    img_obj = np.zeros((m, n), dtype=np.uint8)
    img_obj[img[:, :, 0] > thresh] = 255

    selem = skimage.morphology.disk(r_selem)
    img_obj = skimage.morphology.opening(img_obj, selem)
    if i == 2:
        img_obj = skimage.morphology.closing(img_obj, selem)

    label_img_obj = skimage.measure.label(img_obj)
    n_obj = np.max(label_img_obj)
    return img_obj, n_obj

def draw_rag(img, img_label, img_seg, w_threshold=np.inf):
    rag = skimage.future.graph.rag_mean_color(img, img_label)
    regions = skimage.measure.regionprops(img_label)
    for region in regions:
        rag.nodes[region["label"]]["centroid"] = region["centroid"]

    img_rag = img_seg.copy()
    for edge in rag.edges:
        n1, n2 = edge
        r1, c1 = map(int, rag.nodes[n1]["centroid"])
        r2, c2 = map(int, rag.nodes[n2]["centroid"])

        line  = skimage.draw.line(r1, c1, r2, c2)
        circle1 = skimage.draw.disk((r1, c1), 2)
        circle2 = skimage.draw.disk((r2, c2), 2)

        if rag[n1][n2]["weight"] < w_threshold :
            img_rag[line] = 0, 255, 0
        img_rag[circle1] = 255, 255, 0
        img_rag[circle2] = 255, 255, 0

    return img_rag

def segment_mean_shift(img, img_slic, img_super, bandwidth):
    m = img.shape[0]
    n = img.shape[1]

    # perform mean shift segmentation
    img_flat = img_super.reshape((m * n, 3))
    # est_bandwidth = sklearn.cluster.estimate_bandwidth(img_flat, quantile=.06, n_samples=3000)
    mean_shift = sklearn.cluster.MeanShift(bandwidth=bandwidth, bin_seeding=True)
    mean_shift.fit(img_flat)

    # segment labels
    img_label = mean_shift.labels_.reshape((m, n))
    img_label += 1

    # color labeled segments with cluster colors
    img_seg_map = np.zeros_like(img_super)
    for i in range(len(mean_shift.cluster_centers_)):
        img_seg_map[img_label == i+1] = mean_shift.cluster_centers_[i]
    img_seg_map = img_seg_map.astype(np.uint8)

    # mark segment boundaries
    img_boundary = skimage.segmentation.mark_boundaries(img_seg_map, img_label, (0, 0, 0))
    img_boundary = skimage.util.img_as_ubyte(img_boundary)

    # TODO: tree relationship structure
    img_tree = img_seg_map

    # draw region adjacency graph
    img_region = skimage.measure.label(img_label)
    img_rag = draw_rag(img_super, img_region, img_seg_map, np.inf)

    # combine images
    img_pad = skimage.util.img_as_ubyte(np.ones((m, n//16, 3)))
    img_ms = np.concatenate((
        img,          img_pad,
        img_seg_map,  img_pad,
        img_boundary, img_pad,
        img_tree,     img_pad,
        img_rag
    ), axis=1)
    return img_ms

def segment_n_cut(img, img_slic, img_super, threshold):
    m = img.shape[0]
    n = img.shape[1]

    # create region adjancy graph
    rag = skimage.future.graph.rag_mean_color(img, img_slic, mode='similarity')

    # perform normalized cut segmentation 
    img_label = skimage.future.graph.ncut(img_slic, rag, thresh=threshold)
    img_label += 1

    # color labeled segments with average color
    img_seg_map = skimage.color.label2rgb(img_label, img, kind='avg')
    img_seg_map = img_seg_map.astype(np.uint8)

    # mark segment boundaries
    img_boundary = skimage.segmentation.mark_boundaries(img_seg_map, img_label, (0, 0, 0))
    img_boundary = skimage.util.img_as_ubyte(img_boundary)

    # TODO: tree relationship structure
    img_tree = img_seg_map

    # draw region adjacency graph
    img_rag = draw_rag(img_super, img_label, img_seg_map, np.inf)

    # combine images
    img_pad = skimage.util.img_as_ubyte(np.ones((m, n//16, 3)))
    img_nc = np.concatenate((
        img,          img_pad,
        img_seg_map,  img_pad,
        img_boundary, img_pad,
        img_tree,     img_pad,
        img_rag
    ), axis=1)
    return img_nc


if __name__ == "__main__":
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)

    # QUESTION 1
    params = [
        ("A1.png", 170, 16),
        ("A2.png", 144, 26),
        ("A3.png", 160, 26)
    ]
    for i, (img_name, thresh, r_selem) in enumerate(params):
        img = read_image(INPUT_PATH + img_name)
        img_obj, n_obj = count_objects(img, i+1, thresh, r_selem)
        print(f"The number of flowers in image {img_name} is {n_obj}")
        write_image(img_obj, OUTPUT_PATH + img_name)

    # Outputs
    # The number of flowers in image A1.png is 170
    # The number of flowers in image A2.png is 22
    # The number of flowers in image A3.png is 12


    # QUESTION 2
    for i in range(4):
        img = read_image(INPUT_PATH + f"B{i+1}.jpg")

        # generate superpixels using slic algorithm
        img_slic = skimage.segmentation.slic(img, compactness=16, n_segments=256)
        img_super = skimage.color.label2rgb(img_slic, img, kind='avg')
        img_super = img_super.astype(np.uint8)

        # mean shift
        for j, bandwidth in enumerate([8, 16, 32]):
            img_ms = segment_mean_shift(img, img_slic, img_super, bandwidth)
            write_image(img_ms, OUTPUT_PATH + f"B{i+1}_algorithm_meanshift_parameterset_{j+1}.png")

        # n-cut
        for j, threshold in enumerate([0.001, 0.01, 0.1]):
            img_nc = segment_n_cut(img, img_slic, img_super, threshold)
            write_image(img_nc, OUTPUT_PATH + f"B{i+1}_algorithm_ncut_parameterset_{j+1}.png")
