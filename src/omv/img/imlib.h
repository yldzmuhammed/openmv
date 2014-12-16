/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image library.
 *
 */
#ifndef __IMLIB_H__
#define __IMLIB_H__
#include <stdint.h>
#include <stdbool.h>
#include "array.h"
#include "fmath.h"
typedef struct point {
    uint16_t x;
    uint16_t y;
} point_t;

typedef struct size {
    int w;
    int h;
} wsize_t;

typedef struct rectangle {
    int x;
    int y;
    int w;
    int h;
} rectangle_t;

typedef struct blob {
    int x;
    int y;
    int w;
    int h;
    int c;
    int id;
} blob_t;

typedef struct color {
    union {
        uint8_t vec[3];
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        struct {
            int h;
            int s;
            int v;
        };
        struct {
            int8_t L;
            int8_t A;
            int8_t B;
        };
        struct {
            float x;
            float y;
            float z;
        };
    };
} color_t;

typedef struct image {
    int w;
    int h;
    int bpp;
    union {
        uint8_t *pixels;
        uint8_t *data;
    };
} image_t;

typedef struct integral_image {
    int w;
    int h;
    uint32_t *data;
} i_image_t;

typedef struct {
    union {
        struct {
            uint8_t c0;
            uint8_t c1;
            uint8_t c2;
            uint8_t c3;
        };

        struct {
            uint16_t s0;
            uint16_t s1;
        };
        struct {
            uint32_t i;
        };
    };
}vec_t;

typedef struct cluster {
    array_t *points;
    point_t centroid;
} cluster_t;

/* FAST/FREAK Keypoint */
typedef struct {
    uint16_t x;
    uint16_t y;
    float angle;
    uint8_t *desc;
} kp_t;

/* Haar cascade struct */
typedef struct cascade {
    /* Step size of filter window shifting */
    int step;
    /* Image standard deviation */
    int std;
    /* Detection threshold */
    float threshold;
    /* Scaling step size */
    float scale_factor;
    /* Number of stages in the cascade */
    int n_stages;
    /* Number of features in the cascade */
    int n_features;
    /* Number of rectangles in the cascade */
    int n_rectangles;
    /* Detection window size */
    struct size window;
    /* Grayscale image */
    struct image *img;
    /* Integral image */
    struct integral_image *sum;
    /* Haar cascade arrays */
    uint8_t *stages_array;
    int16_t *stages_thresh_array;
    int16_t *tree_thresh_array;
    int16_t *alpha1_array;
    int16_t *alpha2_array;
    int8_t *num_rectangles_array;
    int8_t *weights_array;
    int8_t *rectangles_array;
} cascade_t;

typedef enum interp {
    INTERP_NEAREST,
    INTERP_BILINEAR,
    INTERP_BICUBIC
} interp_t;

/* Point functions */
point_t *point_alloc(int x, int y);
int point_equal(point_t *p1, point_t *p2);
float point_distance(point_t *p1, point_t *p2);

/* Rectangle functions */
rectangle_t *rectangle_alloc();
rectangle_t *rectangle_clone(rectangle_t *r);
void rectangle_add(rectangle_t *r0, rectangle_t *r1);
void rectangle_div(rectangle_t *r0, int c);
int rectangle_intersects(rectangle_t *r0, rectangle_t *r1);
struct array *rectangle_merge(struct array *r);

/* Clustering functions */
array_t *cluster_kmeans(array_t *points, int k);

/* Dela E on RGB/HSV/LAB */
uint32_t imlib_lab_distance(struct color *c0, struct color *c1);
uint32_t imlib_rgb_distance(struct color *c0, struct color *c1);
uint32_t imlib_rgb_distance(struct color *c0, struct color *c1);

/* Color space conversion */
void imlib_rgb_to_lab(struct color *rgb, struct color *lab);
void imlib_rgb_to_hsv(struct color *rgb, struct color *hsv);

/* Image filtering functions */
int  imlib_image_mean(struct image *src);
void imlib_histeq(struct image *src);
void imlib_median_filter(image_t *src, int r);
void imlib_erode(image_t *src, int ksize);
void imlib_dilate(image_t *src, int ksize);
void imlib_morph(image_t *src, uint8_t *kernel, int k_size);
void imlib_invert(image_t *src);
void imlib_binary(image_t *src, int threshold);
void imlib_threshold(image_t *src, image_t *dst, color_t *color, int color_size, int threshold);
void imlib_rainbow(image_t *src, struct image *dst);
array_t *imlib_count_blobs(struct image *image);

/* Integral image functions */
void imlib_integral_image_alloc(struct integral_image *i_img, int w, int h);
void imlib_integral_image(struct image *src, struct integral_image *sum);
void imlib_integral_image_sq(struct image *src, struct integral_image *sum);
void imlib_integral_image_scaled(struct image *src, struct integral_image *sum);
uint32_t imlib_integral_lookup(struct integral_image *src, int x, int y, int w, int h);

/* Template matching */
float imlib_template_match(struct image *image, struct image *template, struct rectangle *r);

/* Haar/VJ */
int imlib_load_cascade(struct cascade* cascade, const char *path);
struct array *imlib_detect_objects(struct image *image, struct cascade* cascade);

/* FAST/FREAK Feature Extractor */
kp_t *fast_detect(image_t *image, int threshold, int *ret_num_corners, rectangle_t *roi);
void freak_find_keypoints(image_t *image, kp_t *kpts, int kpts_size, bool orient_normalized, bool scale_normalized);
int16_t *freak_match_keypoints(kp_t *kpts1, int kpts1_size, kp_t *kpts2, int kpts2_size, int threshold);
int freak_save_descriptor(kp_t *kpts, int kpts_size, const char *path);
int freak_load_descriptor(kp_t **kpts_out, int *kpts_size_out, const char *path);

/* LBP Operator */
void imlib_lbp_desc(image_t *image, int div, uint8_t *desc, rectangle_t *roi);
uint8_t *imlib_lbp_cascade(image_t *image, rectangle_t *roi);
int imlib_lbp_desc_distance(uint8_t *d0, uint8_t *d1);
int imlib_lbp_desc_load(const char *path, uint8_t **desc);

/* Drawing functions */
void imlib_draw_rectangle(struct image *image, struct rectangle *r);
void imlib_draw_circle(struct image *image, int cx, int cy, int r, color_t *c);
void imlib_draw_line(image_t *src, int x0, int y0, int x1, int y1);
void imlib_draw_string(image_t *src, int x, int y, const char *str, color_t *c);

/* Misc */
void imlib_scale(struct image *src, struct image *dst, interp_t interp);
void imlib_blit(struct image *src, struct image *dst, int x_off, int y_off);
void imlib_blend(struct image *src, struct image *dst, int x_off, int y_off, uint8_t alpha);
void imlib_subimage(struct image *src, struct image *dst, int x_off, int y_off);
void jpeg_compress(image_t *src, image_t *dst, int quality);

/* Image file functions */
int ppm_read(image_t *img, const char *path);
int ppm_write(image_t *img, const char *path);
int ppm_write_subimg(image_t *img, const char *path, rectangle_t *r);
int imlib_load_image(image_t *image, const char *path);
int imlib_save_image(image_t *image, const char *path, rectangle_t *r);
#endif //__IMLIB_H__
