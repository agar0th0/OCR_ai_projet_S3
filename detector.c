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


// the most difficult function of this project 

void extract_word_list(unsigned char *img, int width, int height, struct form grid) {
    // Step one Find probable word list area (simplified: scan for a large area of black pixels outside grid area)
    int x_min_grid = grid.starting_point.x;
    int y_min_grid = grid.starting_point.y;
    int x_max_grid = grid.end_point_x;
    int y_max_grid = grid.end_point_y;
    int min_x = width, min_y = height, max_x = 0, max_y = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Skip grid area
            if (x >= x_min_grid && x <= x_max_grid && y >= y_min_grid && y <= y_max_grid)
                continue;
            if (img[y * width + x] == 0) {
                if (x < min_x) min_x = x;
                if (y < min_y) min_y = y;
                if (x > max_x) max_x = x;
                if (y > max_y) max_y = y;
            }
        }
    }

    // Step two For each word in the list area, extract and save
    int height_zone = max_y - min_y;
    int width_zone = max_x - min_x;
    int lines = 0;
    int line_sum[height_zone];

    for (int y = min_y; y < max_y; ++y) {
        line_sum[y - min_y] = 0;
        for (int x = min_x; x < max_x; ++x) {
            if (img[y * width + x] == 0)
                line_sum[y - min_y]++;
        }
    }

    // Threshold for detecting line = number of black pixels per line
    int threshold = width_zone / 2;
    for (int y = 0; y < height_zone; y++) {
        if (line_sum[y] > threshold) lines++;
    }

    // Step word For each detected line, segment horizontally into letters and save
    for (int line_idx = 0, y = min_y; y < max_y; ++y) {
        if (line_sum[y - min_y] > threshold) {
            // Found a word Segment horizontally into letters (naive fixed width).
            int letter_x_start = min_x, letter_x_end = min_x;
            int in_letter = 0, letter_idx = 0;
            for (int x = min_x; x < max_x; ++x) {
                if (img[y * width + x] == 0 && !in_letter) {
                    in_letter = 1;
                    letter_x_start = x;
                }
                if ((img[y * width + x] == 255 || x == max_x - 1) && in_letter) {
                    in_letter = 0;
                    letter_x_end = x;
                    //  save image
                    char folder[60], file[80];
                    sprintf(folder, "detection/word_list/word_%d", line_idx);
                    mkdir(folder, 0777);
                    sprintf(file, "%s/letter_%d.png", folder, letter_idx);

                    int w = letter_x_end - letter_x_start;
                    int h = 16; // fixed height, adjust as needed
                    unsigned char *letter_img = malloc(w * h);

                    for (int dy = 0; dy < h; ++dy) {
                        for (int dx = 0; dx < w; ++dx) {
                            int yy = y - h/2 + dy;
                            int xx = letter_x_start + dx;
                            if (yy >= 0 && yy < height && xx >= 0 && xx < width)
                                letter_img[dy * w + dx] = img[yy * width + xx];
                            else
                                letter_img[dy * w + dx] = 255;
                        }
                    }
                    stbi_write_png(file, w, h, 1, letter_img, w);
                    free(letter_img);
                    letter_idx++;
                }
            }
            line_idx++;
        }
    }
}






int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Error, following format required:\n./detector <file_name_image.png>");
		return EXIT_FAILURE;
	}

	int width, height, channels;
	unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 0);

	if (img == NULL) {
		printf("Error, image can't be loaded !");
		return EXIT_FAILURE;
	}

	if (channels != 1) {
		printf("Error, the image need to be in black and white !");
		return EXIT_FAILURE;
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

	// we have the grid now we search the number of rows 
	// and the number of columns
	
	// first of all let's place ourselves on a white pixel 
	int placement_x = 0;
	int placement_y = 0;
	
	for (int y = y_start; y < y_max; y++) {
		while (placement_x < width && img[y * width + placement_x] != 255) {
			placement_x++;
		}
		if (placement_x >= width) {
			continue;
		}
		if (img[y * width + placement_x] == 255) {
			placement_y = y;
			break;
		}
	}

	int *row_sum = malloc((y_max - y_start) * sizeof(int)); 
	for (int y = y_start; y < y_max; y++) {
	    row_sum[y - y_start] = 0;
	    for (int x = x_start; x < x_max; x++) {
	        if (img[y * width + x] == 0)
	            row_sum[y - y_start]++;
	    }
	}

	int *col_sum = malloc((x_max - x_start) * sizeof(int));
	for (int x = x_start; x < x_max; x++) {
	    col_sum[x - x_start] = 0;
	    for (int y = y_start; y < y_max; y++) {
	        if (img[y * width + x] == 0)
	            col_sum[x - x_start]++;
	    }
	}
	
	int nb_rows = 0;
	int in_black = 0;
	int row_threshold = (x_max - x_start) / 2; 
	
	for (int y = 0; y < y_max - y_start; y++) {
	    if (row_sum[y] > row_threshold) {
	        if (!in_black) {
	            nb_rows++;
	            in_black = 1;
	        }
	    } else {
	        in_black = 0;
	    }
	}
	
	int nb_cols = 0;
	in_black = 0;
	int col_threshold = (y_max - y_start) / 2;

	for (int x = 0; x < x_max - x_start; x++) {
	    if (col_sum[x] > col_threshold) {
	        if (!in_black) {
       		     nb_cols++;
    		     in_black = 1;
		}
	    } else {
	        in_black = 0;
	    }
	}
	
	free(row_sum);
	free(col_sum);

	if (nb_rows == 0 || nb_cols == 0) {
	    printf("Error: couldn't detect grid lines correctly.\n");
	    stbi_image_free(img);
	    return EXIT_FAILURE;
	}

	// now we can have the size of each case where we have each letter
	
	int width_case = (x_max - x_start) / nb_cols;
	int height_case = (y_max - y_start) / nb_rows;

	// now let's generate each image

	mkdir("detection", 0777);
	mkdir("detection/letters_grid", 0777);
	mkdir("detection/list_of_words", 0777);

	for (int y = y_start; y < y_max; y += height_case) {
		for (int x = x_start; x < x_max; x += width_case) {
			unsigned char *img_case = malloc(width_case * height_case);
			for (int dy = 0; dy < height_case; dy++) {
				for (int dx = 0; dx < width_case; dx++) {
					img_case[dy * width_case + dx] = img[(y + dy) * width + (x + dx)];
				}
			} // let's copy each pixel
			char buffer[120];
			sprintf(buffer, "detection/letters_grid/letter_%i_%i.png",x ,y);
			stbi_write_png(buffer, width_case, height_case, 1, img_case, width_case);
			free(img_case);
		}
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
	return 0;
}

