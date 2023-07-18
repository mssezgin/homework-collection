# Mustafa Sezgin
# 2380863

import os
import numpy as np
import skimage.io
import skimage.util
import scipy.fftpack as fp
from scipy import signal
import matplotlib.pyplot as plt

INPUT_PATH = "./THE2_Images/"
OUTPUT_PATH = "./Outputs/"


def read_image(img_path, rgb = True):
    return skimage.io.imread(img_path, not rgb)

def write_image(img, output_path):
    # # skimage.io.imsave(output_path, (img * 255).astype(np.uint8))
    # img = img.astype(np.uint8)
    img = skimage.util.img_as_ubyte(img)
    skimage.io.imsave(output_path, img)
    return

def fourier_transform(img):
    imgf = fp.fftn(img)
    imgf = fp.fftshift(imgf)
    imgf = (10 * np.log(1 + np.abs(imgf))).astype(int)
    return imgf

def hadamard_transform(img):
    imgh = img
    return imgh

def cosine_transform(img):
    imgc = fp.dctn(img)
    imgc = (10 * np.log(1 + np.abs(imgc))).astype(int)
    return imgc

def circle_matrix(r):
    a = np.arange(-r, r + 1).reshape(2*r+1, 1)
    a = a * a
    b = a.T
    dist = np.sqrt(a + b)
    c = (dist - r < 0.1).astype(int)
    return c

def filter_ilp(img, r):
    m = img.shape[0]
    n = img.shape[1]
    ilpf = np.zeros((m, n))
    ilpf[m//2-r:m//2+r+1, n//2-r:n//2+r+1] = circle_matrix(r)
    ilpf = fp.ifftshift(ilpf)
    img_ilp = np.empty_like(img)
    for i in range(3):
        imgf = fp.fft2(img[:, :, i])
        imgf_ilp = imgf * ilpf
        img_ilp[:, :, i] = np.abs(fp.ifft2(imgf_ilp))
    return img_ilp

def filter_ihp(img, r):
    m = img.shape[0]
    n = img.shape[1]
    ilpf = np.zeros((m, n))
    ilpf[m//2-r:m//2+r+1, n//2-r:n//2+r+1] = circle_matrix(r)
    ihpf = 1 - ilpf
    ihpf = fp.ifftshift(ihpf)
    img_ihp = np.empty_like(img)
    for i in range(3):
        imgf = fp.fft2(img[:, :, i])
        imgf_ihp = imgf * ihpf
        img_ihp[:, :, i] = np.abs(fp.ifft2(imgf_ihp))
    return img_ihp

def filter_glp(img, sigma):
    m = img.shape[0]
    n = img.shape[1]
    glpf = np.outer(signal.gaussian(m, sigma), signal.gaussian(n, sigma))
    glpf = fp.ifftshift(glpf)
    imgf = np.empty_like(img, dtype=complex)
    imgf_glp = np.empty_like(img, dtype=complex)
    img_glp = np.empty_like(img)
    for i in range(3):
        imgf[:, :, i] = fp.fft2(img[:, :, i])
        imgf_glp[:, :, i] = imgf[:, :, i] * glpf
        img_glp[:, :, i] = np.abs(fp.ifft2(imgf_glp[:, :, i]))
    return img_glp

def filter_ghp(img, sigma):
    m = img.shape[0]
    n = img.shape[1]
    glpf = np.outer(signal.gaussian(m, sigma), signal.gaussian(n, sigma))
    ghpf = 1 - glpf
    ghpf = fp.ifftshift(ghpf)
    imgf = np.empty_like(img, dtype=complex)
    imgf_ghp = np.empty_like(img, dtype=complex)
    img_ghp = np.empty_like(img)
    for i in range(3):
        imgf[:, :, i] = fp.fft2(img[:, :, i])
        imgf_ghp[:, :, i] = imgf[:, :, i] * ghpf
        img_ghp[:, :, i] = np.abs(fp.ifft2(imgf_ghp[:, :, i]))
    return img_ghp

def filter_blp(img, r, order):
    m = img.shape[0]
    n = img.shape[1]
    u, v = np.indices((m, n))
    blpf = 1 / (1 + (np.sqrt((u - m/2)**2 + (v - n/2)**2) / r)**order)
    blpf = fp.ifftshift(blpf)
    imgf = np.empty_like(img, dtype=complex)
    imgf_blp = np.empty_like(img, dtype=complex)
    img_blp = np.empty_like(img)
    for i in range(3):
        imgf[:, :, i] = fp.fft2(img[:, :, i])
        imgf_blp[:, :, i] = imgf[:, :, i] * blpf
        img_blp[:, :, i] = np.abs(fp.ifft2(imgf_blp[:, :, i]))
    return img_blp

def filter_bhp(img, r, order):
    m = img.shape[0]
    n = img.shape[1]
    u, v = np.indices((m, n))
    blpf = 1 / (1 + (np.sqrt((u - m/2)**2 + (v - n/2)**2) / r)**order)
    bhpf = 1 - blpf
    bhpf = fp.ifftshift(bhpf)
    imgf = np.empty_like(img, dtype=complex)
    imgf_bhp = np.empty_like(img, dtype=complex)
    img_bhp = np.empty_like(img)
    for i in range(3):
        imgf[:, :, i] = fp.fft2(img[:, :, i])
        imgf_bhp[:, :, i] = imgf[:, :, i] * bhpf
        img_bhp[:, :, i] = np.abs(fp.ifft2(imgf_bhp[:, :, i]))
    return img_bhp


def diagonal_matrix(img, thickness):
    m = img.shape[0]
    n = img.shape[1]
    u, v = np.indices((m, n))
    u = u / (m-1)
    v = v / (n-1)
    # d = (np.abs(u - v) < thickness).astype(int)
    d = (np.abs(u - v) < thickness)
    return d

def filter_br_img4(img, thickness):
    d = diagonal_matrix(img, thickness)
    filter1 = (~np.logical_and(d, np.logical_xor(d, np.flip(d, axis=0)))).astype(int)
    filter2 = np.flip(filter1, axis=0)
    filter1 = fp.ifftshift(filter1)
    filter2 = fp.ifftshift(filter2)
    imgf = np.empty_like(img, dtype=complex)
    imgf_f = np.empty_like(img, dtype=complex)
    img_f = np.empty_like(img)
    # red channel
    imgf[:, :, 0] = fp.fft2(img[:, :, 0])
    imgf_f[:, :, 0] = imgf[:, :, 0] * filter2
    img_f[:, :, 0] = np.abs(fp.ifft2(imgf_f[:, :, 0]))
    # green channel
    imgf[:, :, 1] = fp.fft2(img[:, :, 1])
    imgf_f[:, :, 1] = imgf[:, :, 1] * filter1
    img_f[:, :, 1] = np.abs(fp.ifft2(imgf_f[:, :, 1]))
    # blue channel
    imgf[:, :, 2] = fp.fft2(img[:, :, 2])
    imgf_f[:, :, 2] = imgf[:, :, 2] * filter1
    img_f[:, :, 2] = np.abs(fp.ifft2(imgf_f[:, :, 2]))
    return img_f

def filter_bp_img4(img, thickness):
    d = diagonal_matrix(img, thickness)
    filter1 = (np.logical_and(d, np.logical_xor(d, np.flip(d, axis=0)))).astype(int)
    filter2 = np.flip(filter1, axis=0)
    filter1 = fp.ifftshift(filter1)
    filter2 = fp.ifftshift(filter2)
    imgf = np.empty_like(img, dtype=complex)
    imgf_f = np.empty_like(img, dtype=complex)
    img_f = np.empty_like(img)
    # red channel
    imgf[:, :, 0] = fp.fft2(img[:, :, 0])
    imgf_f[:, :, 0] = imgf[:, :, 0] * filter2
    img_f[:, :, 0] = np.abs(fp.ifft2(imgf_f[:, :, 0]))
    # green channel
    imgf[:, :, 1] = fp.fft2(img[:, :, 1])
    imgf_f[:, :, 1] = imgf[:, :, 1] * filter1
    img_f[:, :, 1] = np.abs(fp.ifft2(imgf_f[:, :, 1]))
    # blue channel
    imgf[:, :, 2] = fp.fft2(img[:, :, 2])
    imgf_f[:, :, 2] = imgf[:, :, 2] * filter1
    img_f[:, :, 2] = np.abs(fp.ifft2(imgf_f[:, :, 2]))
    return img_f

def filter_br_img5(img, thickness):
    m = img.shape[0]
    n = img.shape[1]
    filter = np.ones((m, n))
    filter[m//2-thickness:m//2+thickness+1, :] = 0
    filter = fp.ifftshift(filter)
    imgf = np.empty_like(img, dtype=complex)
    imgf_f = np.empty_like(img, dtype=complex)
    img_f = np.empty_like(img)
    # red channel
    imgf[:, :, 0] = fp.fft2(img[:, :, 0])
    imgf_f[:, :, 0] = imgf[:, :, 0] * filter
    img_f[:, :, 0] = np.abs(fp.ifft2(imgf_f[:, :, 0]))
    # green channel
    imgf[:, :, 1] = fp.fft2(img[:, :, 1])
    imgf_f[:, :, 1] = imgf[:, :, 1] * filter
    img_f[:, :, 1] = np.abs(fp.ifft2(imgf_f[:, :, 1]))
    # blue channel
    imgf[:, :, 2] = fp.fft2(img[:, :, 2])
    imgf_f[:, :, 2] = imgf[:, :, 2] * filter
    img_f[:, :, 2] = np.abs(fp.ifft2(imgf_f[:, :, 2]))
    return img_f

def filter_bp_img5(img, thickness):
    m = img.shape[0]
    n = img.shape[1]
    filter = np.zeros((m, n))
    filter[m//2-thickness:m//2+thickness+1, :] = 1
    filter = fp.ifftshift(filter)
    imgf = np.empty_like(img, dtype=complex)
    imgf_f = np.empty_like(img, dtype=complex)
    img_f = np.empty_like(img)
    # red channel
    imgf[:, :, 0] = fp.fft2(img[:, :, 0])
    imgf_f[:, :, 0] = imgf[:, :, 0] * filter
    img_f[:, :, 0] = np.abs(fp.ifft2(imgf_f[:, :, 0]))
    # green channel
    imgf[:, :, 1] = fp.fft2(img[:, :, 1])
    imgf_f[:, :, 1] = imgf[:, :, 1] * filter
    img_f[:, :, 1] = np.abs(fp.ifft2(imgf_f[:, :, 1]))
    # blue channel
    imgf[:, :, 2] = fp.fft2(img[:, :, 2])
    imgf_f[:, :, 2] = imgf[:, :, 2] * filter
    img_f[:, :, 2] = np.abs(fp.ifft2(imgf_f[:, :, 2]))
    return img_f


if __name__ == "__main__":
    if not os.path.exists(OUTPUT_PATH):
        os.makedirs(OUTPUT_PATH)

    # Step 2
    for img_name in ["1.png", "2.png"]:
        img = read_image(INPUT_PATH + img_name)

        # # fourier transform
        imgf = fourier_transform(img)
        write_image(imgf, OUTPUT_PATH + "F" + img_name)

        # hadamard transform
        imgh = hadamard_transform(img)
        write_image(imgh, OUTPUT_PATH + "H" + img_name)

        # cosine transform
        imgc = cosine_transform(img)
        write_image(imgc, OUTPUT_PATH + "C" + img_name)

    # Step 3
    img = read_image(INPUT_PATH + "3.png")
    for (i, r) in enumerate([64, 32, 8]):

        # ideal low pass filter
        img_ilp = filter_ilp(img, r)
        write_image(img_ilp, OUTPUT_PATH + "ILP_r" + (i+1) + ".png")

        # gaussian low pass filter
        sigma = r
        img_glp = filter_glp(img, sigma)
        write_image(img_glp, OUTPUT_PATH + "GLP_r" + (i+1) + ".png")

        # butterworth low pass filter
        img_blp = filter_blp(img, r, order=2)
        write_image(img_blp, OUTPUT_PATH + "BLP_r" + (i+1) + ".png")

    # Step 4
    img = read_image(INPUT_PATH + "3.png")
    for (i, r) in enumerate([128, 32, 8]):

        # ideal high pass filter
        img_ihp = filter_ihp(img, r)
        write_image(img_ihp, OUTPUT_PATH + "IHP_r" + (i+1) + ".png")

        # gaussian high pass filter
        sigma = r
        img_ghp = filter_ghp(img, sigma)
        write_image(img_ghp, OUTPUT_PATH + "GHP_r" + (i+1) + ".png")

        # butterworth high pass filter
        img_bhp = filter_bhp(img, r, order=2)
        write_image(img_bhp, OUTPUT_PATH + "BHP_r" + (i+1) + ".png")

    # Step 5
    img = read_image(INPUT_PATH + "4.png")
    img_f = filter_br_img4(img, thickness=0.025)
    write_image(img_f, OUTPUT_PATH + "BR_1.png")
    img_f = filter_bp_img4(img, thickness=0.025)
    write_image(img_f, OUTPUT_PATH + "BP_1.png")

    img = read_image(INPUT_PATH + "5.png")
    img_f = filter_br_img5(img, thickness=2)
    write_image(img_f, OUTPUT_PATH + "BR_2.png")
    img_f = filter_bp_img5(img, thickness=2)
    write_image(img_f, OUTPUT_PATH + "BP_2.png")

    # Step 6
