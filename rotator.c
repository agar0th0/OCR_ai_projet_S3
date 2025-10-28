// program to rotate the image before the binarisation by preference

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "imports/stb_image.h"
#include "imports/stb_image_write.h"

typedef struct {
    unsigned char *data;
    int width, height, channels;
} Image;

Image *rotate_image(const Image *src, double angle_deg) {
    double angle = angle_deg * M_PI / 180.0;
    int w = src->width;
    int h = src->height;
    int c = src->channels;

    Image *rot = malloc(sizeof(Image));
    rot->width = w;
    rot->height = h;
    rot->channels = c;
    rot->data = calloc(w * h * c, 1);

    int cx = w / 2;
    int cy = h / 2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int src_x = (int)((x - cx) * cos(-angle) - (y - cy) * sin(-angle) + cx);
            int src_y = (int)((x - cx) * sin(-angle) + (y - cy) * cos(-angle) + cy);

            if (src_x >= 0 && src_x < w && src_y >= 0 && src_y < h) {
                for (int ch = 0; ch < c; ch++) {
                    rot->data[(y * w + x) * c + ch] =
                        src->data[(src_y * w + src_x) * c + ch];
                }
            }
        }
    }

    return rot;
}

