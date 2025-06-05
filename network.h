#define SIZE 784
#define NUM_TRAIN 60000
#define NUM_TEST 10000



/***************creating network-objects*************/

const int network_size = 3;
const int network[] = {784,30,10};

//hmmmmm, not convinced this is right
double learning_rate = 0.5;
int num_epochs = 32;
int batch_size = 10;


int numNeurons;
int numWeights;
int numBiases;


void size_create(){

	//neuron values
	numNeurons = 0;
	for(int i=0;i<network_size;i++){
		numNeurons += network[i];
	}
	//weights
	numWeights = 0;
	for(int i=0;i<network_size-1; i++){
		numWeights += network[i]*network[i+1];
	}
	//biases
	numBiases = numNeurons - network[0];

}


/*******************algorithm params****************/


//notes / idea --> NORMALIZE output?, NORMALIZE neurons??

//use exp() for now....

double sigmoid(double x){
	return 1 / (1+my_exp(-x)); 
	//return x;
}
double sigmoid_prime(double sigmoid_x){
	return sigmoid_x*(1-sigmoid_x);
	//return sigmoid_x;
}

//double softmax(double x){ //NOT NORMALIZED

//	return exp(x);

//}

double softmax_prime(double norm_softmax_x){ //assuming softmax_x properly normalized
	return norm_softmax_x * (1 - norm_softmax_x);
}


double loss(double* neurons,int label){


	double* output = neurons + numNeurons - network[network_size-1];

	return -my_log(*(output + label) + 1e-9);
}

void D_cross_entropy(double* M_Initial,double* neurons,/*double* f_prime,*/int label){

	double* output = neurons + numNeurons - network[network_size-1];
	//double* output_f_prime = f_prime + numNeurons - network[network_size-1];

	int y;
	for(int i=0;i<network[network_size-1];i++){
		if(label == i) y = 1;
		else y=0;
		*(M_Initial + i) = (*(output + i) - y); // divided by 1.0 = *(output_f_prime + i);
	}

}








//quadratic loss
double quadratic_loss(double* neurons,int label){

	double* output = neurons + numNeurons-network[network_size-1]; //wrong index??

	double error = 0;
	for(int i=0;i<network[network_size-1];i++){
		if(i == label){
			error +=  square(*(output+i) - 1);
		}
		else{
			error += square(*(output+i));
		}
	}

	return 0.5*error;
}


/*
//derivative of loss (M_Initial)
void quadratic_D_loss(double* M_Initial, double* neurons, int label){

	double* output = neurons + numNeurons-network[network_size-1]; //wrong index??

	for(int i=0;i<network[network_size-1];i++){

		if(i == label) *(M_Initial+i) = *(output+i) - 1;
		else *(M_Initial+i) = *(output+i);
	}

}


*/