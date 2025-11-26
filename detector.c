// this programm will detect each position of:
// 	the grid
// 	the list of words
// 	the letters in the grid
// 	the words in the list
// 	the letters in the words of the list
// and after that this program will save each letters
// in some folders as image, 
// those will be used by our neural network later

#define STB_IMAGE_IMPLEMENTATION
#include "imports/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "imports/stb_image_write.h"

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct coord {
	int x;
	int y;
};

struct form {
	struct coord starting_point;
	int end_point_y;
	int end_point_x;
	// we don't need coord end_point; because this is just an addition 
	// between the two coord on top.
};

struct form find_a_form(unsigned char *image, int x, int y, int width, int height) {
    struct form output;
    output.starting_point.x = x;
    output.starting_point.y = y;
    output.end_point_x = x;
    output.end_point_y = y;

    int xx = x;
    while (xx + 1 < width && image[y * width + (xx + 1)] == 0) {
        xx++;
    }
    output.end_point_x = xx;

    int yy = y;
    while (yy + 1 < height && image[(yy + 1) * width + x] == 0) {
        yy++;
    }
    output.end_point_y = yy;

    return output;
} // optimised O(n) a lot better 

void grid_to_letters_correct(char *filename){

	int width, height, channels;
	unsigned char *img = stbi_load(filename, &width, &height, &channels, 0);

	if (img == NULL) {
		printf("Error, image can't be loaded !");
		return;
	}

	if (channels != 1) {
		printf("Error, the image need to be in black and white !");
		return;
	}

	// we need two things first to find the grid in our image and so on until having the letters
	// and do the same thing for the list of words we will first focus on the grid 
	
	// first we need to go throughout each pixel of our image
	
	struct form grid; // the biggest form (like square or rectangle) will be our grid.
	
	grid.starting_point.x = 0;
	grid.starting_point.y = 0;
	grid.end_point_y = 0;
	grid.end_point_x = 0;
	

	// we will find the biggest so our grid
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (img[y * width + x] == 0) {
				struct form tmp = find_a_form(img, x, y, width, height);
				
				int L_x_curr = grid.end_point_x - grid.starting_point.x;
				int l_y_curr = grid.end_point_y - grid.starting_point.y;

				int L_x_tmp = tmp.end_point_x - tmp.starting_point.x;
				int l_y_tmp = tmp.end_point_y - tmp.starting_point.y;
				
				int A_curr = L_x_curr * l_y_curr;
				int A_tmp = L_x_tmp * l_y_tmp;

				if (A_tmp > A_curr) {
					grid.starting_point = tmp.starting_point;
					grid.end_point_x = tmp.end_point_x;
					grid.end_point_y = tmp.end_point_y;
				}
			}
		}
	}

	int x_start = grid.starting_point.x;
	int y_start = grid.starting_point.y;
	int x_max = grid.end_point_x;
	int y_max = grid.end_point_y;

	// we have our range so we need to navigate in it and deduce each slot
	

	








	mkdir("detection", 0777);
	mkdir("detection/letters_grid", 0777);
	
	int actual_row = 1; 
	
	for (int y = y_start; y  + height_case < real_ymax; y += height_case) {
		char dir[100];
		sprintf(dir,"detection/letters_grid/row_%i", actual_row);
		mkdir(dir,0777);
		int actual_col = 1;
		for (int x = x_start; x + width_case < real_xmax; x += width_case) {
			unsigned char *img_case = malloc(width_case * height_case);
			
			for (int dy = 0; dy < height_case; dy++) {
				for (int dx = 0; dx < width_case; dx++) {
					img_case[dy * width_case + dx] = img[(y + dy) * width + (x + dx)];
				}
			} // let's copy each pixel
			
			char buffer[120];
			sprintf(buffer, "detection/letters_grid/row_%i/letter_%i.png", actual_row, actual_col);
			stbi_write_png(buffer, width_case, height_case, 1, img_case, width_case);
			free(img_case);
			actual_col++;
		}
		actual_row++;
	}

	// after all this let's do the same for the list of words ahhhhhhhh
	// but here we have no grid 

	// three step find the area of the list
	// find the area of each word 
	// find and sauv each letter
	//
	//
	// Detection

	// find the area of the list of words 
	// after find the area of each word 
	// 	find the area of each letter 
	// 	create a folder for the actual word and put in the image of the letters

	// I had a problem with the way to analyze character because I have delete my code about
	// the list of words because I know that doesn't 
	stbi_image_free(img);
}





int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Error, following format required:\n./detector <file_name_image.png>");
		return EXIT_FAILURE;
	}

	grid_to_letters_correct(argv[1]);

	return 0;
}

