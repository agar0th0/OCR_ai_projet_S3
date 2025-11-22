#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>


#define NUM_INPUTS 25 
#define NUM_HIDDEN 20 
#define NUM_OUTPUTS 4 


double train_inputs[NUM_OUTPUTS][NUM_INPUTS] = {
    {0,1,1,1,0, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1}, 
    {1,1,1,1,0, 1,0,0,0,1, 1,1,1,1,0, 1,0,0,0,1, 1,1,1,1,0}, 
    {0,1,1,1,1, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 0,1,1,1,1}, 
    {1,1,1,1,0, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,0}  
};

double train_targets[NUM_OUTPUTS][NUM_OUTPUTS] = {
    {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}
};


double hidden_weights[NUM_INPUTS][NUM_HIDDEN];
double hidden_biases[NUM_HIDDEN];
double hidden_outputs[NUM_HIDDEN];

double output_weights[NUM_HIDDEN][NUM_OUTPUTS];
double output_biases[NUM_OUTPUTS];
double final_outputs[NUM_OUTPUTS];

double learning_rate = 0.1;


double rand_weight() { return ((double)rand() / RAND_MAX) * 2.0 - 1.0; }
double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }
double sigmoid_derivative(double x) { return x*(1.0-x); }

void initialize() {
    srand(time(NULL));
    for(int i=0;i<NUM_INPUTS;i++)
        for(int j=0;j<NUM_HIDDEN;j++) hidden_weights[i][j]=rand_weight();
    for(int i=0;i<NUM_HIDDEN;i++){
        hidden_biases[i]=rand_weight();
        for(int j=0;j<NUM_OUTPUTS;j++) output_weights[i][j]=rand_weight();
    }
    for(int i=0;i<NUM_OUTPUTS;i++) output_biases[i]=rand_weight();
}

void forward_propagation(double inputs[]){
    for(int j=0;j<NUM_HIDDEN;j++){
        double sum=hidden_biases[j];
        for(int i=0;i<NUM_INPUTS;i++) sum+=inputs[i]*hidden_weights[i][j];
        hidden_outputs[j]=sigmoid(sum);
    }
    for(int k=0;k<NUM_OUTPUTS;k++){
        double sum=output_biases[k];
        for(int j=0;j<NUM_HIDDEN;j++) sum+=hidden_outputs[j]*output_weights[j][k];
        final_outputs[k]=sigmoid(sum);
    }
}

void back_propagation(double inputs[], double targets[]){
    double output_deltas[NUM_OUTPUTS], hidden_deltas[NUM_HIDDEN];
    for(int k=0;k<NUM_OUTPUTS;k++)
        output_deltas[k]=(targets[k]-final_outputs[k])*sigmoid_derivative(final_outputs[k]);
    for(int j=0;j<NUM_HIDDEN;j++){
        double error=0;
        for(int k=0;k<NUM_OUTPUTS;k++) error+=output_deltas[k]*output_weights[j][k];
        hidden_deltas[j]=error*sigmoid_derivative(hidden_outputs[j]);
    }
    for(int k=0;k<NUM_OUTPUTS;k++){
        output_biases[k]+=output_deltas[k]*learning_rate;
        for(int j=0;j<NUM_HIDDEN;j++) output_weights[j][k]+=output_deltas[k]*hidden_outputs[j]*learning_rate;
    }
    for(int j=0;j<NUM_HIDDEN;j++){
        hidden_biases[j]+=hidden_deltas[j]*learning_rate;
        for(int i=0;i<NUM_INPUTS;i++) hidden_weights[i][j]+=hidden_deltas[j]*inputs[i]*learning_rate;
    }
}


void draw_grid(SDL_Renderer* renderer, double input[], int cell_size){
    for(int y=0;y<5;y++){
        for(int x=0;x<5;x++){
            if(input[y*5+x]>0.5)
                SDL_SetRenderDrawColor(renderer,0,0,0,255); 
            else
                SDL_SetRenderDrawColor(renderer,255,255,255,255);
            SDL_Rect rect={x*cell_size, y*cell_size, cell_size, cell_size};
            SDL_RenderFillRect(renderer,&rect);
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderDrawRect(renderer,&rect);
        }
    }
}


int main(){
    initialize();
    
    
    for(int epoch=0;epoch<20000;epoch++){
        int sample=rand()%NUM_OUTPUTS;
        forward_propagation(train_inputs[sample]);
        back_propagation(train_inputs[sample],train_targets[sample]);
    }

    
    if(SDL_Init(SDL_INIT_VIDEO)<0){printf("SDL Error: %s\n",SDL_GetError()); return 1;}
    SDL_Window* window=SDL_CreateWindow("Lettre 5x5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    char letters[]={'A','B','C','D'};
    int running=1;
    SDL_Event event;

    while(running){
        SDL_PollEvent(&event);
        if(event.type==SDL_QUIT) running=0;

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        
        for(int i=0;i<NUM_OUTPUTS;i++){
            forward_propagation(train_inputs[i]);
            draw_grid(renderer, train_inputs[i], 80);
            SDL_RenderPresent(renderer);
            printf("Lettre: %c Prediction brute: ", letters[i]);
            for(int k=0;k<NUM_OUTPUTS;k++)
                printf("%.2f ", final_outputs[k]);
            int max_index=0; double max_val=0;
            for(int k=0;k<NUM_OUTPUTS;k++)
                if(final_outputs[k]>max_val){max_val=final_outputs[k]; max_index=k;}
            printf(" => Prediction: %c\n", letters[max_index]);
            SDL_Delay(1000);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
