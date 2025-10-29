// to be clear this programme need to be run after the binarisation it will work on 
// image in black and white 
#define STB_IMAGE_IMPLEMENTATION
#include "imports/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "imports/stb_image_write.h"

int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("Error, following format needed:\n./rotator <filename_image.png> <degrees_to_rotate>");
		return EXIT_FAILURE;
	}

	int width, height, channels;
	unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 0);

	if (img == NULL) {
		printf("Error, image can not be loaded !");
		return EXIT_FAILURE;
	}

	if (channels != 1) {
		printf("Error : Image must be black and white !");
		stbi_image_free(img);
		return EXIT_FAILURE;
	}

	// our image is in black and white it means that we have only 1 channel 

	double teta = atof(argv[2]) * M_PI / 180.0; // we need to put it in radians

	// remove negative angle handling, we'll use the real cos/sin
	// if (argv[2][0] == '-') {
	// 	teta = -teta;
	// }

	double cosinus = cos(teta);
	double sinus = sin(teta);

	// the if statements were used like an abs() function for double
	// we only need abs() for new image dimensions
	int new_width  = (int)(width * fabs(cosinus) + height * fabs(sinus));
	int new_height = (int)(width * fabs(sinus) + height * fabs(cosinus));

	unsigned char *rotated = malloc(new_width * new_height * sizeof(unsigned char)); // here it's like * 1 because each pixel have one channel
	memset(rotated, 255, new_width * new_height);

	// we need the center of our image 

	double cx = width / 2.0;
	double cy = height / 2.0;

	// we need to go throught our image to fullfil the returned image
	for ( int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// we convert the position to be rotated
			double x_rot = (x - cx) * cosinus - (y - cy) * sinus;
			double y_rot = (x - cx) * sinus + (y - cy) * cosinus;

			int new_x = (int)(x_rot + new_width / 2.0); // new coo from the center of the rotated image 
			int new_y = (int)(y_rot + new_height / 2.0);	// this time
			
			if (new_x >= 0 && new_x < new_width && new_y >= 0 && new_y < new_height)
				rotated[new_y * new_width + new_x] = img[y * width + x]; 
		}
	}

	stbi_write_png("rotated.png", new_width, new_height, 1, rotated, new_width);
	
	stbi_image_free(img);
	free(rotated);
	return 0;
}

