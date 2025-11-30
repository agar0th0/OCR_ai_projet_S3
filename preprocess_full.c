#include <stdio.h>
#include <stdlib.h>
#include <math.h>     
#include <string.h> 

#define STB_IMAGE_IMPLEMENTATION
#include "imports/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "imports/stb_image_write.h"

#define WHITE 255
#define BLACK 0


int main(int argc, char* argv[]) {


	if (argc != 2) {
		printf("Error, following format needed:\n./auto_rotator <filename_image.png>");
		return EXIT_FAILURE;
	}

	int width, height, channels;
	// load the binarized image (1 channel)
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

	// hough Transform: Angle Detection
	
	int num_angles= 90;
	
	double max_rho =sqrt(width * width + height * height);
	int num_rhos =(int)(max_rho * 2.0);
	
	double cx =width / 2.0;
	double cy =height / 2.0;

	// pre-calculate Sin/Cos tables for angles -45 to +44
	double* cos_table = malloc(num_angles * sizeof(double));
	double* sin_table = malloc(num_angles * sizeof(double));
	if (cos_table == NULL || sin_table == NULL) return EXIT_FAILURE;

	for (int theta_idx = 0; theta_idx < num_angles; theta_idx++) {
        // map index 0-89 to degrees -45 to +44
		double angle_rad = (theta_idx - 45.0) * M_PI / 180.0;
		cos_table[theta_idx] = cos(angle_rad);
		sin_table[theta_idx] = sin(angle_rad);
	}

	// create the 2D accumulator and initialize to 0
	int** accumulator = calloc(num_angles, sizeof(int*));
	if (accumulator == NULL) return EXIT_FAILURE;
	for (int i = 0; i < num_angles; i++) {
		accumulator[i] = calloc(num_rhos, sizeof(int));
		if (accumulator[i] == NULL) return EXIT_FAILURE;
	}
	
	//voting Phase
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (img[y * width + x] == BLACK) { // If pixel is black
				for (int theta_idx = 0; theta_idx < num_angles; theta_idx++) {
					double rho = (x - cx) * cos_table[theta_idx] 
							   + (y - cy) * sin_table[theta_idx];
					int rho_idx = (int)(rho + num_rhos / 2.0);
					accumulator[theta_idx][rho_idx]++;
				}
			}
		}
	}

	// find Peak Vote
	
	int max_votes = 0;
	int best_theta_idx = 0; 

	// search only in our limited range
	for (int theta_idx = 0; theta_idx < num_angles; theta_idx++) {
		for (int rho_idx = 0; rho_idx < num_rhos; rho_idx++) {
			if (accumulator[theta_idx][rho_idx] > max_votes) {
				max_votes = accumulator[theta_idx][rho_idx];
				best_theta_idx = theta_idx;
			}
		}
	}

	//rotation Calculation (With Reverse Mapping)

	// convert the winning index back to an angle in degrees
	double detected_angle_deg = (double)best_theta_idx - 45.0;
	printf("Detected angle: %.2f degrees\n", detected_angle_deg);

    // apply the inverse of the detected angle to straighten the image
	double teta = (detected_angle_deg * -1.0) * M_PI / 180.0;
	
	double cosinus = cos(teta);
	double sinus = sin(teta);

	// calculate the new bounding box dimensions
	int new_width  = (int)(width * fabs(cosinus) + height * fabs(sinus));
	int new_height = (int)(width * fabs(sinus) + height * fabs(cosinus));

	unsigned char *rotated = malloc(new_width * new_height * sizeof(unsigned char));
	if (rotated == NULL) return EXIT_FAILURE;
	
	// fill the new image with WHITE
	memset(rotated, 255, new_width * new_height);

	// find centers of both images for mapping
	double cx_orig = width / 2.0;
	double cy_orig = height / 2.0;
	double cx_new = new_width / 2.0;
	double cy_new = new_height / 2.0;

	//  reverse mapping loop
	for (int new_y = 0; new_y < new_height; new_y++) {
		for (int new_x = 0; new_x < new_width; new_x++) {

			double x_rot = new_x - cx_new;
			double y_rot = new_y - cy_new;

			// apply inverse rotation
			double x_orig_rel = x_rot * cosinus + y_rot * sinus;
			double y_orig_rel = -x_rot * sinus + y_rot * cosinus;

            // find the nearest source coordinate
			int x = (int)(x_orig_rel + cx_orig + 0.5); // Add 0.5 for rounding
			int y = (int)(y_orig_rel + cy_orig + 0.5);

			// check if the source pixel is within the original image bounds
			if (x >= 0 && x < width && y >= 0 && y < height) {
				
				// copy the source pixel color to the destination
				rotated[new_y * new_width + new_x] = img[y * width + x];
			}
           
		}
	}

	//save and clean up
	
	stbi_write_png("rotated_auto.png", new_width, new_height, 1, rotated, new_width);
	
	stbi_image_free(img);
	free(rotated);

	// Free Hough Transform memory
	for (int i = 0; i < num_angles; i++) {
		free(accumulator[i]);
	}
	free(accumulator);
	free(cos_table);
	free(sin_table);

	printf("Automatic rotation complete. Saved to 'rotated_auto.png'.\n");
	return 0;
}

