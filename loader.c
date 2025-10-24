
#define STB_IMAGE_IMPLEMENTATION
#include "imports/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "imports/stb_image_write.h"

int main(int argc, char *argv[]) {
	int width, height, channels;
	unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 0);

	if (img == NULL) {
		printf("Erreur, probleme de chargement de l'image !");
		exit(1);
	}
	
	// printf("SUCCESS");
	
	size_t img_size = width * height * channels;
	int gray_channels = 0;
	if (channels == 4)
		gray_channels = 2;
	else
		gray_channels = 1;
	
	size_t gray_img_size = width * height * gray_channels;
	unsigned char *gray_img = malloc(gray_img_size);
	if (gray_img == NULL) {
		printf("Erreur, probleme de chargement de l'image en gris !");
		exit(1);
	}
	// utilisation d'unsigned char car valeurs possibles de 0 a 255 utile dans notre cas RGBA
	for (unsigned char *p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
		// moyenne trop brute on doit utilisewr l'autre methode *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/ 3.0);
		*pg = (uint8_t)(0.299 * *p + 0.589 * *(p + 1) + 0.114 * *(p + 2));

		if (channels == 4) {
			*(pg + 1) = *(p + 3);
		}
	}

	stbi_write_png("converted_gray.png", width, height , gray_channels, gray_img, width * gray_channels);

	stbi_image_free(img);
	free(gray_img);
	return 0;
}
