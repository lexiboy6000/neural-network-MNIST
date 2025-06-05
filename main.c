#include <stdio.h> // for printf, FILE handling, etc.
#include <stdlib.h>// for malloc() and rand()
#include "my_math.h"
//#include <math.h>

#include "network.h"
/* contains paramaters for network ---
int network_size
int network[]

double learning_rate;
int numEpochs;
int batch_size;

sigmoid()
loss()
D_loss()

*/

#include "training.h"
/*contains learning algorithm (grad descent) ---
size_create()
learn_batch()
learn()
*/

#include "passes.h"
/*contains foward_pass, back_prop*/


void print_images(int start_image,int num_print,
					double* images,int* labels){

	for(int i=SIZE*(start_image);i<SIZE*(start_image+num_print);i++){
		if(i%28 == 0){
			printf("\n");
		}
		if(i%SIZE == 0){
			printf("\n new image --- %d\n", labels[i/SIZE]);
		}
		if(images[i] > 0.5){
			printf("\033[31m");
			printf("%.2f ",images[i]);
		}
		else{
			printf("\033[0m");
			printf("%.2f ",images[i]);
		}
	}
	printf("\n");


}

//test_images, test_labels
double test_image(double* input,int label_val, double* neurons, double* f_prime,
					double* weights, double* biases){
	//testing algorithm on test_images, test_labels


	double wrong = 0.0;

	foward_pass(neurons,f_prime,input,weights,biases);
	double* output = neurons + numNeurons - network[network_size-1];

	int index_max=0;
	double max = output[0];
	for(int i=0;i<network[network_size-1];i++){
		if(output[i] > max){
			max = output[i];
			index_max = i;
		}
	}

	if(label_val != index_max) wrong = 1.0;

	return wrong;


}


double test_all(double* test_images,int* test_labels,
				double* neurons,double* f_prime,
				double* weights,double* biases){

	double error = 0;
	for(int i=0;i<NUM_TEST;i++){
		error += test_image(test_images+i*SIZE,*(test_labels+i),neurons,f_prime,weights,biases);
	}
	error /= NUM_TEST;
	error *= 100;

	return error;
}


int main() 
{


					/******************** GET image & label data************************/

	FILE* train_images = fopen("/home/alexanderstranzl/MNIST_AI/data/train-images.idx3-ubyte","rb");
	FILE* train_labels = fopen("/home/alexanderstranzl/MNIST_AI/data/train-labels.idx1-ubyte","rb");

	if(train_images == NULL){
		perror("failed train_images");
	}
	if(train_labels == NULL){
		perror("failed train_labels");
	}


	//shift initial data away...
	fseek(train_images,16,SEEK_CUR);
	fseek(train_labels,8,SEEK_CUR);

	int num_images = NUM_TRAIN*SIZE;

	//copy data
	unsigned char* image_char  = malloc(num_images);
	unsigned char* labels_char = malloc(NUM_TRAIN);

	fread(image_char,1,num_images,train_images);
	fread(labels_char,1,NUM_TRAIN,train_labels);

	fclose(train_images);
	fclose(train_labels);

	//put data into desired double,int form
	double* images = malloc(num_images*sizeof(double));
	int*    labels = malloc(NUM_TRAIN*sizeof(int));

	for(int i=0;i<num_images;i++){
		images[i] = (double) image_char[i] / 255;
	}
	for(int i=0;i<NUM_TRAIN;i++){
		labels[i] = (int) labels_char[i];
	}

	free(image_char);
	free(labels_char);
	
					/******************** GET TESTING image & label data************************/

	FILE* ftest_images = fopen("./data/t10k-images.idx3-ubyte","rb");
	FILE* ftest_labels = fopen("./data/t10k-labels.idx1-ubyte","rb");

	if(ftest_images == NULL){
		perror("failed to open ftest_images");
	}
	if(ftest_labels == NULL){
		perror("failed to open ftest_labels");
	}

	int num_test_images = NUM_TEST * SIZE;

	unsigned char* char_test_images = malloc(num_test_images);
	unsigned char* char_test_labels = malloc(NUM_TEST);

	fseek(ftest_images,16,SEEK_CUR);
	fseek(ftest_labels,8,SEEK_CUR);

	fread(char_test_images,1,num_test_images,ftest_images);
	fread(char_test_labels,1,NUM_TEST,ftest_labels);

	fclose(ftest_images);
	fclose(ftest_labels);


	double* test_images = malloc(num_test_images*sizeof(double));
	int* test_labels = malloc(NUM_TEST*sizeof(int));

	for(int i=0;i<num_test_images;i++){
		test_images[i] = (double) char_test_images[i] / 255;
	}
	for(int i=0;i<NUM_TEST;i++){
		test_labels[i] = (int) char_test_labels[i];
	}


						/******************TRAIN!!!!!!!**************************/

	double* D = learn(images,labels,test_images,test_labels);
		double* weights = D;
		double* biases = D+numWeights;

	free(images);
	free(labels);

						/********************FINAL TESTING**********************/
	
	double neurons[numNeurons];
	double f_prime[numNeurons];
	double* output = (neurons + numNeurons - network[network_size-1]);

	printf("\n first 20 test-image outputs & test-labels \n");
	for(int i=0;i<20;i++){
		foward_pass(neurons,f_prime,test_images+SIZE*i,weights,biases);
		for(int j=0;j<network[network_size-1];j++){
			printf("%f ",*(output+j));
		}
		printf("label: %d\n",*(test_labels+i));

	}

	double last_error = test_all(test_images,test_labels,neurons,f_prime,weights,biases);
	printf("\nFINAL percent error = %.2f%% \n",last_error);

	free(test_images);
	free(test_labels);


	free(D);
	return 0;
}
