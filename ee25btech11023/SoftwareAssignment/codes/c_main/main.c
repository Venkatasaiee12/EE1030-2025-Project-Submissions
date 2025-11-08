#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../c_libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../c_libs/stb_image_write.h"

void print_time(double calctime){  //function to calculate time
    if ( calctime>59){
        int minutes =(int) calctime/60;
        printf("Total reconstruction time: %d min %.2f sec\n",minutes,calctime-minutes*60.0);
    }
    else printf("Total reconstruction time:%.2f\n", calctime);
}
void makezero(double *a, int M){  //make zeroes for all elements in given array
    for(int i = 0; i < M; i++) a[i] = 0.0;
}
void matrix_vector_multiply(double **A_row, double *v, double *temp, int M, int N){  //multiply A with v and store it in temp array
    for(int i = 0; i < M; i++)
        for(int j = 0; j < N; j++)
            temp[i] += A_row[i][j] * v[j];
}

void transpose_multiply(double **A_row, double *temp, double *v, int M, int N){  //multiply A transpose with temp
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++)
            v[i] += A_row[j][i] * temp[j];
}
void normalize_vector(double *v, int N){  // normalize the vector
    double norm = 0.0;
    for(int i = 0; i < N; i++) norm += v[i] * v[i];
    norm = sqrt(norm);
    if(norm < 1e-9) norm = 1e-9;   // to prevent overflow
    for(int i = 0; i < N;i++) v[i] /= norm;
}

void power_iteration(double **A_row, double *v, double *temp, int M, int N){ //power iteration 20 times
    for(int p = 0; p < 20; p++){
        makezero(temp,M);
        matrix_vector_multiply(A_row,v,temp,M,N);
        makezero(v,N);
        transpose_multiply(A_row,temp,v,M,N);
        normalize_vector(v,N);
    }
}
void compute_u(double **A_row, double *v, double *u, int M,int N){  //multiply A with new eigen vector v
    makezero(u,M);
    for(int i = 0; i <M; i++)
        for(int j =0; j<N;j++)
            u[i] += A_row[i][j] * v[j];
}

void out_matrix(double **A_row, double *u, double *v, double *output, int M, int N){  //deflate
    for(int i = 0; i < M;i++)
        for(int j = 0; j < N; j++){
            double val = u[i] * v[j];
            output[i*N + j] += val;
            A_row[i][j] -= val;
        }
}

void reconstruct(double *input, double * output,int M, int N, int k){
    double *u = malloc(M * sizeof(double));
    double *v = malloc(N * sizeof(double));
    double *temp = malloc(M * sizeof(double));
    double *A = malloc(M * N * sizeof(double));
    double *A_row[M];

    for(int i = 0; i < M; i++) A_row[i] = A + i * N;  //A_row matrix points to memory adress of starting of each row of A
    for(int i = 0; i < M*N; i++) A[i] = input[i];

    for(int l = 0; l < k; l++){
        for(int i = 0; i < N; i++) v[i] = (double)rand() / RAND_MAX;  //initialise a random vector between 0.0 and 1.0
        power_iteration(A_row,v,temp,M,N);
        compute_u(A_row,v,u,M,N);     //deflate the original matrix
        out_matrix(A_row, u,v,output,M,N);   //add it into new matrix
    }
    free(u);
    free(v); free(temp); free(A);
}

int main(int argc, char *argv[]){
    int M, N, ch;
    unsigned char *img = stbi_load(argv[1],&N,&M,&ch,0);
    if(!img) return 1;

    int target_ch = 1;
    if(argc > 3) target_ch = atoi(argv[3]); //convert char to int

    char form[10];
    char name[256];
    sscanf(argv[1], "%[^.].%s",name,form); //get the format of input image

    if(target_ch == 1 && ch == 3){
        unsigned char *gray_img = malloc(M * N);
        for(int i = 0; i < M*N; i++) gray_img[i] = img[i*3];  //remove duplicated channels

        stbi_image_free(img);
        img = gray_img;  //generate a clean img matrix
        ch = 1;  //make it true grayscale
    }

    double *input = malloc(M*N*ch*sizeof(double));
    for(int i = 0;i < M*N*ch;i++) input[i] = img[i];  //make unsigned char to double by assigning a new array

    stbi_image_free(img);
    double calctime=0.0;
    int k_vals[] = {5, 20, 50, 100};
    for(int j = 0; j < 4; j++){
        int k = k_vals[j];
        double *out = calloc(M*N*ch,sizeof(double));  //allocate and make them 0
        calctime=0;
        if(ch == 3){
            printf("k=%d:\n", k);
            for(int c = 0; c < 3; c++){
                double *in = malloc(M * N *sizeof(double));
                double *o = calloc(M * N,sizeof(double));

                for(int i = 0;i <M*N;i++) in[i] = input[i*3 + c]; //send the channels one by one
                clock_t start = clock();

                reconstruct(in,o,M, N,k);  //resconstruct the channel
                clock_t end =clock();
                calctime+=(double)(end-start)/CLOCKS_PER_SEC;

                double error = 0.0;
                for(int i = 0;i< M*N;i++){   //find error in that channel there itself
                    double diff = in[i] - o[i];
                    error += diff*diff;
                }
                printf("Error in Channel %d: %.6f\n",c+1,sqrt(error)); // print the error
                if(c==2) print_time(calctime);
                for(int i = 0;i <M*N;i++) out[i*3 + c]= o[i];  //add them to output matrix again
                free(in); free(o);
            }
        } else {
            clock_t start = clock();
            reconstruct(input,out,M,N,k); // if not rgb, do it directly
            clock_t end =clock();
            calctime=(double)(end-start)/CLOCKS_PER_SEC;

            double error = 0.0;
            for(int i = 0;i < M*N;i++){   //calculate error in channel
                double diff = input[i] - out[i];
                error += diff*diff;
            }
            printf("k=%d:\nError:%.6f\n",k,sqrt(error));  //print the channel
            print_time(calctime);
        }
        unsigned char *result = malloc(M *N * ch);
        for(int i = 0; i < M*N*ch; i++){
            int p = (int)(out[i]+0.5);
            result[i] = (p > 255)? 255 : (p <0) ? 0 : p;   //handle out of bounding pixel values
        }

        char file[256];
        if(!strcmp(form,"png")){  //based on given jpg or png , handle output extension
            sprintf(file,"%s_k%d.png", argv[2], k);
            stbi_write_png(file,N,M,ch,result,N*ch);
        } else {
            sprintf(file,"%s_k%d.jpg", argv[2], k);
            stbi_write_jpg(file,N,M,ch,result,50);  //set quality to 50 for jpg
        }
        printf("Saved: %s\n\n", file);

        free(result);
        free(out);
    }
    free(input);
    return 0;

}
