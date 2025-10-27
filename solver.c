// the program solver take two arguments the name of the file whrer we have the grid and 
// the word to search
// if found we return the first position of the char of our word and the last position of his last char
// else we just have to printf \"Not Found\"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // for the function tolower

typedef struct {
	int a;
	int b;
} Couple;

int is_in(char letter, char* word, int len_word, int k) {
	if (tolower(letter) == tolower(word[k]) && k == (len_word-1)) {
		return 2; // end
	}
	if (tolower(letter) == tolower(word[k])) {
		return 1; // in the word
	}
	else {
		return 0; // out 
	}

}

int check(char* grid[], char* word, int i, int j, int i_diff, int j_diff, int n, int m, int len_word) {
	int start_i = i;
	int start_j = j;

	int k = 0;
	while ( i >=0 && i < n && j >= 0 && j < m && is_in(grid[i][j], word, len_word, k) != 0) {
		if (is_in(grid[i][j], word, len_word, k) == 2) {
			printf("(%i,%i)(%i,%i)",start_j,start_i,j,i);
			return 1;
		}
		k++;
		i += i_diff;
		j += j_diff;
	}
	return 0;
}


int main(int argc, char* argv[]) {

	int nb_args = argc - 1;

	if (argc < 3 || argc > 3) {
		printf("Error, we need to the following command\t:\n ./solver <filename_of_the_grid> <word_to_search> ");
		return EXIT_FAILURE;
	}
	// need to take the file and build a jagged array to have a grid to dive in 

	FILE *file_grid = fopen(argv[1], "r");

	if (!file_grid) {
		printf("Error 1, file can't be opened !");
		return EXIT_FAILURE;
	}

	char line[256];
	int n = 0;
	int m = 0;

	while (fgets(line, sizeof(line), file_grid)) {
		line[strcspn(line, "\n")] = '\0';

		if (n == 0) {
			m = strlen(line);
		}
		n++;
	}

	fclose(file_grid);

	char **grid = calloc(n, sizeof(char*));

	for (int i = 0; i < m; i++) {
		grid[i] = calloc(m, sizeof(char));
	}

	// need to fullfil our grid now 	

	FILE *fgrid = fopen(argv[1], "r");

	if (!fgrid) {
		printf("Error 2, file can't be opened !");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < n;i++) {
		char line[256]; 
		fgets(line, sizeof(line), fgrid);
		line[strcspn(line, "\n")] = '\0';
		int k = 0;
		while (line[k] != '\0') {
			grid[i][k] = line[k];
			k++;
		}
	}

	int len_word = strlen(argv[2]);

	char *word = calloc(len_word , sizeof(char));
	strcpy(word,argv[2]);

	Couple directions[8] = {
		{ 0, 1 },
		{ 0, -1 },
		{ 1, 0 },
		{ -1, 0 }, 
		{ 1, 1 },
		{ 1, -1 },
		{ -1, 1 },
		{ -1, -1 }
	};

	int working = 1;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			for (int k = 0; k < 8; k++) {
				int i_diff = directions[k].a;
				int j_diff = directions[k].b;
				if (check(grid, word, i, j, i_diff, j_diff, n, m, len_word) == 1) {
					working = 0;
				}
			}
		}
	}

	if (working != 0) {
		printf("Not found");
	}

	free(grid);
	free(word);
	return 0;
}
