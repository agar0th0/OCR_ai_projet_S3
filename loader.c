
// for the first step convert to gray 
#define STB_IMAGE_IMPLEMENTATION
#include "imports/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "imports/stb_image_write.h"

// for the second step we juste want some macros
#define WHITE 255
#define BLACK 0


int main(int argc, char *argv[]) {
	int width, height, channels;
	unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 0);

	if (img == NULL) {
		printf("Erreur, probleme de chargement de l'image !");
		exit(1);
	}
	
	// printf("SUCCESS");
	
	size_t img_size = width * height * channels;
	int gray_channels = 1; // only one component of color for the image output
	
	size_t gray_img_size = width * height * gray_channels;
	unsigned char *gray_img = malloc(gray_img_size); // channel as sizeof for the space required
	if (gray_img == NULL) {
		printf("Erreur, probleme de chargement de l'image en gris !");
		exit(1);
	}
	
	// first need to find our threshold to convert in black and white 

	long total = 0;
	for (unsigned char *p = img; p != img + img_size; p += channels) 
		total += (uint8_t)(0.299 * *p + 0.589 * *(p + 1) + 0.114 * *(p + 2));

	long threshold = total / (width * height);



	// unsigned char for value from 0 to 255
	for (unsigned char *p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
		
		*pg = (uint8_t)(0.299 * *p + 0.589 * *(p + 1) + 0.114 * *(p + 2));

		if (*pg > threshold) {
			*pg = WHITE;
		}
		else {
			*pg = BLACK;
		}

//		if (channels == 4) { // channel alpha unused for binarisation
//			*(pg + 1) = *(p + 3);
//		}
	}

	stbi_write_png("converted_black_and_white.png", width, height , gray_channels, gray_img, width * gray_channels);

	stbi_image_free(img);
	free(gray_img);

	return 0;
}
