#include <stdlib.h>
#include <math.h>
#include <stdio.h>

double generation()
{
    return ((double)rand() / RAND_MAX) - 0.5;
}

double sigmoid(double n)
{
    double res = 1/ (1+exp(-n));
    return res;
}

const int num_inputs = 2;
const int num_hidden = 2;

double hidden_weights[2][2];
double hidden_biases[2];

double output_weights[2];
double output_bias;

double learning_rate = 0.8;

void initialize_weights_and_biases()
{
    hidden_weights[0][0] = generation();
    hidden_weights[0][1] = generation();
    hidden_weights[1][0] = generation();
    hidden_weights[1][1] = generation();


    hidden_biases[0] = generation();
    hidden_biases[1] = generation();

    output_weights[0] = generation();
    output_weights[1] = generation();

    output_bias = generation();

}

double tempo[2];
double forward_propagation(double begin[])
{
    double x = begin[0]*hidden_weights[0][0] +
    begin[1]*hidden_weights[0][1] + hidden_biases[0];
    double y = begin[0]*hidden_weights[1][0] +
    begin[1]*hidden_weights[1][1] + hidden_biases[1];

    tempo[0] = sigmoid(x);
    tempo[1] = sigmoid(y);

    double res = tempo[0] * output_weights[0] +
    tempo[1] * output_weights[1] + output_bias;
    return sigmoid(res);
}

double bina(double begin[])
{
        double output =  forward_propagation(begin);
        if (output >=0.5)
        {
                return 1.0;
        }
        else
        {
                return 0.0;
        }

}

double backpropagation(double begin[], double target)
{
    double output = forward_propagation(begin);
    double eps = 1e-10;
    double loss = -(output * log(output + eps) + (1 - output) * log(1 - output + eps));

    double output_gradient = output - target;

    double output_weight_gradients[2];
    output_weight_gradients[0] = output_gradient *
    output * (1 - output) * tempo[0];
    output_weight_gradients[1] = output_gradient *
    output * (1 - output) * tempo[1];
    output_weights[0] -= learning_rate *
    output_weight_gradients[0];
    output_weights[1] -= learning_rate *
    output_weight_gradients[1];


    double output_bias_gradient = output_gradient *
    output * (1 - output);
    output_bias -= learning_rate * output_bias_gradient;


    double hidden_gradients[2];
    for (int i = 0; i < 2; i++) {
        hidden_gradients[i] = output_gradient *
        output * (1 - output) * output_weights[i];
    }


    double hidden_weight_gradients[2][2];
    for (int i = 0; i < 2; i++) {
        double input_gradient = hidden_gradients[i] *
        tempo[i] * (1 - tempo[i]);
        for (int j = 0; j < 2; j++) {
            hidden_weight_gradients[j][i] = input_gradient * begin[j];
            hidden_weights[j][i] -= learning_rate *
            hidden_weight_gradients[j][i];
        }
        hidden_biases[i] -= learning_rate * input_gradient;
    }
    return loss;
}

int main()
{
    initialize_weights_and_biases();
    double inputs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double targets[4] = {1, 0, 0, 1};

    double sum_loss;
    int max_rest = 5;
    int restart =0;
    while (restart < max_rest){
            initialize_weights_and_biases();
            sum_loss = 0.0;
     for (int epoch = 0; epoch < 10000; epoch++)
     {

        for (int i = 0; i < 4; i++)
        {
            sum_loss+=backpropagation(inputs[i], targets[i]);
        }
        sum_loss /= 4.0;
        if (epoch % 1000 == 0)
        {
            printf("Restart %d /epoch %d / Average loss : %.6f\n",restart,epoch,sum_loss);

        }

        if(sum_loss < 0.01)
        {
                printf("Stop at Epoch %d (error = %.6f)\n",epoch,sum_loss);
                epoch = 10000;
                break;
        }

     }
     if(sum_loss >=0.01)
     {
             printf("Restart %d...\n",restart+1);
             restart++;
     }
     else
     {
             break;
     }


    }
    /*for (int epoch = 0; epoch < 10000; epoch++)
    {
            sum_loss = 0.0;
        for (int i = 0; i < 4; i++)
        {
            sum_loss+=backpropagation(inputs[i], targets[i]);
        }
        sum_loss /= 4.0;
        if (epoch % 1000 == 0)
        {
            printf("epoch %d / Average loss : %.6f\n",epoch,sum_loss);

        }

        if(sum_loss < 0.01)
        {
                printf("Stop at Epoch %d (error = %.6f)\n",epoch,sum_loss);
                break;
        }
    }*/


    for (int i = 0; i < 4; i++)
    {
        double output = bina(inputs[i]);
        printf("Input: (%.0f, %.0f) Output: %.4f\n",\
         inputs[i][0], inputs[i][1], output);
    }

    return 0;
}
