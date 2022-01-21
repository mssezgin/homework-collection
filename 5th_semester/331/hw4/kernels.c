/********************************************************
 * Kernels to be optimized for the Metu Ceng Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "Team",                     /* Team name */

    "Mustafa Sezgin",           /* First member full name */
    "e2380863",                 /* First member id */

    "",                         /* Second member full name (leave blank if none) */
    "",                         /* Second member id (leave blank if none) */

    "",                         /* Third member full name (leave blank if none) */
    ""                          /* Third member id (leave blank if none) */
};

/****************
 * EXPOSURE FUSION KERNEL *
 ****************/

/*******************************************************
 * Your different versions of the exposure fusion kernel go here 
 *******************************************************/

/* 
 * naive_fusion - The naive baseline version of exposure fusion
 */
char naive_fusion_descr[] = "naive_fusion: Naive baseline exposure fusion";
void naive_fusion(int dim, int *img, int *w, int *dst) {

    int i, j, k;
    for (k = 0; k < 4; k++) {
        for (i = 0; i < dim; i++) {
            for (j = 0; j < dim; j++) {
                dst[i*dim+j] += w[k*dim*dim+i*dim+j] * img[k*dim*dim+i*dim+j];
            }
        }
    }
}

/* 
 * fusion - Your current working version of fusion
 * IMPORTANT: This is the version you will be graded on
 */
char fusion_descr[] = "fusion: 2 by 1 loop unrolling, one loop from 0 to dim^2";
void fusion(int dim, int *img, int *w, int *dst) {

    int i, k;
    int dim2 = dim * dim;
    int* dst0;

    for (k = 0; k < 4; k++) {
        for (i = 0, dst0 = dst; i < dim2; i += 2, dst0 += 2, w += 2, img += 2) {
            *dst0 += *w * *img;
            *(dst0+1) += *(w+1) * *(img+1);
        }
    }
}

/*********************************************************************
 * register_fusion_functions - Register all of your different versions
 *     of the fusion kernel with the driver by calling the
 *     add_fusion_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_fusion_functions() {
    add_fusion_function(&naive_fusion, naive_fusion_descr);
    add_fusion_function(&fusion, fusion_descr);
}

/***************************
 * GAUSSIAN BLUR KERNEL *
 ***************************/

/******************************************************
 * Your different versions of the Gaussian blur functions go here
 ******************************************************/

/* 
 * naive_blur - The naive baseline version of Gussian blur
 */
char naive_blur_descr[] = "naive_blur: The naive baseline version of Gaussian blur";
void naive_blur(int dim, float *img, float *flt, float *dst) {
  
    int i, j, k, l;

    for (i = 0; i < dim-5+1; i++) {
        for (j = 0; j < dim-5+1; j++) {
            for (k = 0; k < 5; k++) {
                for (l = 0; l < 5; l++) {
                    dst[i*dim+j] = dst[i*dim+j] + img[(i+k)*dim+j+l] * flt[k*dim+l];
                }
            }
        }
    }
}

/* 
 * blur - Your current working version of Gaussian blur
 * IMPORTANT: This is the version you will be graded on
 */
char blur_descr[] = "blur: img[i][j..j+4] at the same iteration";
void blur(int dim, float *img, float *flt, float *dst) {

    int dim_dst = dim - 4;

    int kdim, k5dim;
    for (kdim = 0, k5dim = dim * 5; kdim < k5dim; kdim += dim) {

        float flt_k0 = flt[kdim];
        float flt_k1 = flt[kdim + 1];
        float flt_k2 = flt[kdim + 2];
        float flt_k3 = flt[kdim + 3];
        float flt_k4 = flt[kdim + 4];

        float* dst_ij = dst;
        float* dst_in = dst + dim_dst;
        float* dst_n0 = dst + dim * dim_dst;
        float* img_ij = img + kdim;

        while (dst_ij < dst_n0) {

            while (dst_ij < dst_in) {

                *dst_ij +=
                    *img_ij       * flt_k0 +
                    *(img_ij + 1) * flt_k1 +
                    *(img_ij + 2) * flt_k2 +
                    *(img_ij + 3) * flt_k3 +
                    *(img_ij + 4) * flt_k4;
                
                dst_ij++;
                img_ij++;
            }

            dst_ij += 4;
            dst_in += dim;
            img_ij += 4;
        }
    }
}

/*********************************************************************
 * register_blur_functions - Register all of your different versions
 *     of the gaussian blur kernel with the driver by calling the
 *     add_blur_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_blur_functions() {
    add_blur_function(&naive_blur, naive_blur_descr); 
    add_blur_function(&blur, blur_descr);
}
