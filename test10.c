#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

#define IMAGE_SIZE 32
#define INPUT_SIZE (IMAGE_SIZE * IMAGE_SIZE)
#define HIDDEN1_SIZE 256
#define HIDDEN2_SIZE 128
#define OUTPUT_SIZE 26
#define LEARNING_RATE 0.3
#define MAX_IMAGES_PER_LETTER 100  

double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

typedef struct {
    double **weights_input_hidden1;
    double *bias_hidden1;
    double **weights_hidden1_hidden2;
    double *bias_hidden2;
    double **weights_hidden2_output;
    double *bias_output;
    double *hidden1_layer;
    double *hidden2_layer;
    double *output_layer;
} NeuralNetwork;

double xavier_init(int fan_in, int fan_out) {
    double limit = sqrt(6.0 / (fan_in + fan_out));
    return ((double)rand() / RAND_MAX) * 2.0 * limit - limit;
}

void init_network(NeuralNetwork *nn) {
    srand(time(NULL));

    nn->weights_input_hidden1 = malloc(INPUT_SIZE * sizeof(double*));
    for (int i = 0; i < INPUT_SIZE; i++) {
        nn->weights_input_hidden1[i] = malloc(HIDDEN1_SIZE * sizeof(double));
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            nn->weights_input_hidden1[i][j] = xavier_init(INPUT_SIZE, HIDDEN1_SIZE);
        }
    }
    nn->bias_hidden1 = calloc(HIDDEN1_SIZE, sizeof(double));

    nn->weights_hidden1_hidden2 = malloc(HIDDEN1_SIZE * sizeof(double*));
    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        nn->weights_hidden1_hidden2[i] = malloc(HIDDEN2_SIZE * sizeof(double));
        for (int j = 0; j < HIDDEN2_SIZE; j++) {
            nn->weights_hidden1_hidden2[i][j] = xavier_init(HIDDEN1_SIZE, HIDDEN2_SIZE);
        }
    }
    nn->bias_hidden2 = calloc(HIDDEN2_SIZE, sizeof(double));

    nn->weights_hidden2_output = malloc(HIDDEN2_SIZE * sizeof(double*));
    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        nn->weights_hidden2_output[i] = malloc(OUTPUT_SIZE * sizeof(double));
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_hidden2_output[i][j] = xavier_init(HIDDEN2_SIZE, OUTPUT_SIZE);
        }
    }
    nn->bias_output = calloc(OUTPUT_SIZE, sizeof(double));

    nn->hidden1_layer = malloc(HIDDEN1_SIZE * sizeof(double));
    nn->hidden2_layer = malloc(HIDDEN2_SIZE * sizeof(double));
    nn->output_layer = malloc(OUTPUT_SIZE * sizeof(double));
}

void forward_pass(NeuralNetwork *nn, double *input) {
    for (int j = 0; j < HIDDEN1_SIZE; j++) {
        double sum = nn->bias_hidden1[j];
        for (int i = 0; i < INPUT_SIZE; i++) {
            sum += input[i] * nn->weights_input_hidden1[i][j];
        }
        nn->hidden1_layer[j] = sigmoid(sum);
    }

    for (int k = 0; k < HIDDEN2_SIZE; k++) {
        double sum = nn->bias_hidden2[k];
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            sum += nn->hidden1_layer[j] * nn->weights_hidden1_hidden2[j][k];
        }
        nn->hidden2_layer[k] = sigmoid(sum);
    }

    for (int m = 0; m < OUTPUT_SIZE; m++) {
        double sum = nn->bias_output[m];
        for (int k = 0; k < HIDDEN2_SIZE; k++) {
            sum += nn->hidden2_layer[k] * nn->weights_hidden2_output[k][m];
        }
        nn->output_layer[m] = sigmoid(sum);
    }
}

void backward_pass(NeuralNetwork *nn, double *input, double *target) {
    double *output_errors = malloc(OUTPUT_SIZE * sizeof(double));
    double *hidden2_errors = malloc(HIDDEN2_SIZE * sizeof(double));
    double *hidden1_errors = malloc(HIDDEN1_SIZE * sizeof(double));

    for (int m = 0; m < OUTPUT_SIZE; m++) {
        double error = target[m] - nn->output_layer[m];
        output_errors[m] = error * sigmoid_derivative(nn->output_layer[m]);
    }

    for (int k = 0; k < HIDDEN2_SIZE; k++) {
        double error = 0.0;
        for (int m = 0; m < OUTPUT_SIZE; m++) {
            error += output_errors[m] * nn->weights_hidden2_output[k][m];
        }
        hidden2_errors[k] = error * sigmoid_derivative(nn->hidden2_layer[k]);
    }

    for (int j = 0; j < HIDDEN1_SIZE; j++) {
        double error = 0.0;
        for (int k = 0; k < HIDDEN2_SIZE; k++) {
            error += hidden2_errors[k] * nn->weights_hidden1_hidden2[j][k];
        }
        hidden1_errors[j] = error * sigmoid_derivative(nn->hidden1_layer[j]);
    }

    for (int k = 0; k < HIDDEN2_SIZE; k++) {
        for (int m = 0; m < OUTPUT_SIZE; m++) {
            nn->weights_hidden2_output[k][m] +=
                LEARNING_RATE * output_errors[m] * nn->hidden2_layer[k];
        }
    }
    for (int m = 0; m < OUTPUT_SIZE; m++) {
        nn->bias_output[m] += LEARNING_RATE * output_errors[m];
    }

    for (int j = 0; j < HIDDEN1_SIZE; j++) {
        for (int k = 0; k < HIDDEN2_SIZE; k++) {
            nn->weights_hidden1_hidden2[j][k] +=
                LEARNING_RATE * hidden2_errors[k] * nn->hidden1_layer[j];
        }
    }
    for (int k = 0; k < HIDDEN2_SIZE; k++) {
        nn->bias_hidden2[k] += LEARNING_RATE * hidden2_errors[k];
    }

    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN1_SIZE; j++) {
            nn->weights_input_hidden1[i][j] +=
                LEARNING_RATE * hidden1_errors[j] * input[i];
        }
    }
    for (int j = 0; j < HIDDEN1_SIZE; j++) {
        nn->bias_hidden1[j] += LEARNING_RATE * hidden1_errors[j];
    }

    free(output_errors);
    free(hidden2_errors);
    free(hidden1_errors);
}

int load_image_pixels(const char *filename, double *pixels) {
    SDL_Surface *image = IMG_Load(filename);
    if (!image) {
        return 0;
    }

    SDL_Surface *scaled = SDL_CreateRGBSurface(0, IMAGE_SIZE, IMAGE_SIZE, 32, 0, 0, 0, 0);
    SDL_BlitScaled(image, NULL, scaled, NULL);

    SDL_Surface *formatted = SDL_ConvertSurfaceFormat(scaled, SDL_PIXELFORMAT_RGB888, 0);

    Uint8 *pixel_data = (Uint8*)formatted->pixels;
    int pitch = formatted->pitch;

    for (int y = 0; y < IMAGE_SIZE; y++) {
        for (int x = 0; x < IMAGE_SIZE; x++) {
            int offset = y * pitch + x * 3;
            Uint8 r = pixel_data[offset];
            Uint8 g = pixel_data[offset + 1];
            Uint8 b = pixel_data[offset + 2];

            int brightness = (r + g + b) / 3;
            pixels[y * IMAGE_SIZE + x] = (brightness > 60) ? 1.0 : 0.0;
        }
    }

    SDL_FreeSurface(image);
    SDL_FreeSurface(scaled);
    SDL_FreeSurface(formatted);

    return 1;
}

void create_target_vector(char letter, double *target) {
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        target[i] = 0.0;
    }

    if (letter >= 'A' && letter <= 'Z') {
        target[letter - 'A'] = 1.0;
    } else if (letter >= 'a' && letter <= 'z') {
        target[letter - 'a'] = 1.0;
    }
}

char predict_letter(NeuralNetwork *nn, double *pixels, int show_top3) {
    forward_pass(nn, pixels);

    typedef struct {
        char letter;
        double confidence;
    } Prediction;

    Prediction predictions[OUTPUT_SIZE];
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        predictions[i].letter = 'A' + i;
        predictions[i].confidence = nn->output_layer[i];
    }

    for (int i = 0; i < OUTPUT_SIZE - 1; i++) {
        for (int j = 0; j < OUTPUT_SIZE - i - 1; j++) {
            if (predictions[j].confidence < predictions[j + 1].confidence) {
                Prediction temp = predictions[j];
                predictions[j] = predictions[j + 1];
                predictions[j + 1] = temp;
            }
        }
    }

    if (show_top3) {
        printf("    TOP 3: ");
        for (int i = 0; i < 3; i++) {
            printf("%c (%.1f%%) ", predictions[i].letter, predictions[i].confidence * 100.0);
        }
        printf("\n");
    }

    return predictions[0].letter;
}

void train_with_images(NeuralNetwork *nn, const char *folder, int epochs) {
    double *pixels = malloc(INPUT_SIZE * sizeof(double));
    double *target = malloc(OUTPUT_SIZE * sizeof(double));

    printf(" Formation sur tous les PNG à partir de '%s'\n", folder);
    printf("Dossier: '%s', Epoques : %d\n\n", folder, epochs);

    for (int epoch = 0; epoch < epochs; epoch++) {
        double total_error = 0.0;
        int samples = 0;

        DIR *dir = opendir(folder);
        if (!dir) {
            printf("Erreur lors de l'ouverture du dossier %s\n", folder);
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            
            if (strstr(entry->d_name, ".png") == NULL &&
                strstr(entry->d_name, ".jpg") == NULL) continue;

            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", folder, entry->d_name);

            
            char letter = toupper(entry->d_name[0]);
            if (letter >= 'A' && letter <= 'Z') {
                if (load_image_pixels(filepath, pixels)) {
                    create_target_vector(letter, target);
                    forward_pass(nn, pixels);

                    
                    for (int i = 0; i < OUTPUT_SIZE; i++) {
                        double error = target[i] - nn->output_layer[i];
                        total_error += error * error;
                    }

                    backward_pass(nn, pixels, target);
                    samples++;
                }
            }
        }
        closedir(dir);

        if (samples > 0) {
            double avg_error = total_error / samples;
            if (epoch % 10 == 0 || epoch == epochs - 1) {
                printf("Epoch %3d/%d, Error: %.6f, Qualifié: %d fichiers\n",
                       epoch, epochs, avg_error, samples);
            }
        }
    }

    free(pixels);
    free(target);
}

void save_network(NeuralNetwork *nn, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Enregistrer l'erreur %s\n", filename);
        return;
    }

    fwrite(nn->bias_hidden1, HIDDEN1_SIZE, sizeof(double), file);
    fwrite(nn->bias_hidden2, HIDDEN2_SIZE, sizeof(double), file);
    fwrite(nn->bias_output, OUTPUT_SIZE, sizeof(double), file);

    for (int i = 0; i < INPUT_SIZE; i++)
        fwrite(nn->weights_input_hidden1[i], HIDDEN1_SIZE, sizeof(double), file);
    for (int i = 0; i < HIDDEN1_SIZE; i++)
        fwrite(nn->weights_hidden1_hidden2[i], HIDDEN2_SIZE, sizeof(double), file);
    for (int i = 0; i < HIDDEN2_SIZE; i++)
        fwrite(nn->weights_hidden2_output[i], OUTPUT_SIZE, sizeof(double), file);

    fclose(file);
    printf("Enregistré %s\n", filename);
}

void load_network(NeuralNetwork *nn, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Fichier introuvable : %s\n", filename);
        return;
    }

    fread(nn->bias_hidden1, HIDDEN1_SIZE, sizeof(double), file);
    fread(nn->bias_hidden2, HIDDEN2_SIZE, sizeof(double), file);
    fread(nn->bias_output, OUTPUT_SIZE, sizeof(double), file);

    for (int i = 0; i < INPUT_SIZE; i++)
        fread(nn->weights_input_hidden1[i], HIDDEN1_SIZE, sizeof(double), file);
    for (int i = 0; i < HIDDEN1_SIZE; i++)
        fread(nn->weights_hidden1_hidden2[i], HIDDEN2_SIZE, sizeof(double), file);
    for (int i = 0; i < HIDDEN2_SIZE; i++)
        fread(nn->weights_hidden2_output[i], OUTPUT_SIZE, sizeof(double), file);

    fclose(file);
    printf("Chargé : %s\n", filename);
}

void test_with_folder(NeuralNetwork *nn, const char *folder) {
    printf("\n--- Reconnaissance à partir d'un dossier '%s' ---\n", folder);

    double *pixels = malloc(INPUT_SIZE * sizeof(double));

    DIR *dir = opendir(folder);
    if (!dir) {
        printf("Dossier introuvable : '%s'\n", folder);
        free(pixels);
        return;
    }

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        if (strstr(entry->d_name, ".png") == NULL &&
            strstr(entry->d_name, ".jpg") == NULL) continue;

        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", folder, entry->d_name);

        if (load_image_pixels(filepath, pixels)) {
            printf("\n Déposé %s\n", entry->d_name);
            char predicted = predict_letter(nn, pixels, 1);
            double confidence = nn->output_layer[predicted - 'A'] * 100.0;

            printf("  -> Reconnu: '%c' (certitude %.2f%%)\n", predicted, confidence);
            count++;
        }
    }

    closedir(dir);
    printf("\n=== Fichiers traités: %d ===\n", count);
    free(pixels);
}

void free_network(NeuralNetwork *nn) {
    for (int i = 0; i < INPUT_SIZE; i++) {
        free(nn->weights_input_hidden1[i]);
    }
    free(nn->weights_input_hidden1);
    free(nn->bias_hidden1);

    for (int i = 0; i < HIDDEN1_SIZE; i++) {
        free(nn->weights_hidden1_hidden2[i]);
    }
    free(nn->weights_hidden1_hidden2);
    free(nn->bias_hidden2);

    for (int i = 0; i < HIDDEN2_SIZE; i++) {
        free(nn->weights_hidden2_output[i]);
    }
    free(nn->weights_hidden2_output);
    free(nn->bias_output);

    free(nn->hidden1_layer);
    free(nn->hidden2_layer);
    free(nn->output_layer);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf(" Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    NeuralNetwork nn;
    init_network(&nn);

    printf("=== Reconnaissance de lettres (2600 photos) ===\n\n");
    //train_with_images(&nn, "TRAINING", 350);
    //save_network(&nn, "final_model.weights");

    
    load_network(&nn,"final_model.weights");
    
    test_with_folder(&nn, "TRAINING");

    

    free_network(&nn);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

