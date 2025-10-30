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
/*
struct form find_a_form(unsigned char *image, int x, int y, int width, int height) {
	struct form output;
	output.starting_point.x = x;
	output.starting_point.y = y;

	output.end_point_y = y;

	output.end_point_x = x;
	
	int start_x = output.starting_point.x;
	int start_y = output.starting_point.y;

	while (x+1 < width && y+1 < height && (image[y * width + start_x] == 0 || image[start_y * width + x] == 0)){
		if (image[start_y * width + x+1] == 0) { // we advance on the line
			output.end_point_x += 1;
			x++;
		}
		if (image[(y+1) * width + start_x] == 0) { // we advance on the column
			output.end_point_y += 1;
			y++;
		}
	}

	return output;
}

unoptimised O(n^^2)
*/
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

	
/*
	int nb_rows = 0;

	int on_off = 0;

	for (int y = placement_y; y < y_max; y++) { // one at a time first the number of columns
		if (img[y * width + placement_x] == 255) {
			if (on_off) {
				nb_rows++;
				on_off = 0; // for the first time we change colors because the line can be large
			}
		}
		else {
			on_off = 1;
		}
	}

	on_off = 0;
	int nb_cols = 0;

	for (int x = placement_x; x < x_max; x++) {
		if (img[placement_y * width + x] == 255) {
			if (on_off) {
				nb_cols++;
				on_off = 0; // same
			}
		}
		else {
			on_off = 1;
		}
	}
*/

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

	for (int y = y_start; y < y_max; y += height_case) {
		for (int x = x_start; x < x_max; x += width_case) {
			unsigned char *img_case = malloc(width_case * height_case);
			for (int dy = 0; dy < height_case; dy++) {
				for (int dx = 0; dx < width_case; dx++) {
					img_case[dy * width_case + dx] = img[(y + dy) * width + (x + dx)];
				}
			} // let's copy each pixel
			char buffer[60];
			sprintf(buffer, "letter_%i_%i.png",x ,y);
			stbi_write_png(buffer, width_case, height_case, 1, img_case, width_case);
			free(img_case);
		}
	}

	stbi_image_free(img);

	return 0;
}
