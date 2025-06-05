
//make prototypes because in subsequent, not preceeding file
void foward_pass(double* neurons, double* f_prime, double* input,
					double* weights, double* biases);
void back_prop(double* Dw, double* Db, double* M_Initial,
				double* f_prime,double* neurons, double* weights, double* biases);

double test_image(double* input,int label_val, double* neurons, double* f_prime,
					double* weights, double* biases);

double test_all(double* test_images,int* test_labels,
				double* neurons,double* f_prime,
				double* weights,double* biases);



void learn_batch(double* images, int* labels, int batch_val,
				double* neurons,double* f_prime, double* weights, double* biases,
				double* total_loss,double* quad_loss){

	double* Dw = malloc(numWeights*sizeof(double));
	double* Db = malloc(numBiases*sizeof(double));
	
	double M_Initial[network[network_size-1]];

	//make Dw, Db all zeros
	//memset thing apparently faster method...
	for(int i=0;i<numWeights;i++){
		*(Dw+i)=0;
	}
	for(int i=0;i<numBiases;i++){
		*(Db+i)=0;
	}

	//batch = images + batch_val*batch_size*network[0]
	//batch_label = labels+batch_val*batch_size

	//******compute Dw,Db**********//
	for(int i=0; i<batch_size; i++){ 

		int index = (batch_val*batch_size + i)%NUM_TRAIN;

		foward_pass(neurons,f_prime,images+index*network[0],
					weights,biases);
		D_cross_entropy(M_Initial,neurons,*(labels+index));
		back_prop(Dw,Db, M_Initial,
				f_prime,neurons,weights,biases);

		*total_loss += loss(neurons,*(labels+index));
		*quad_loss += quadratic_loss(neurons,*(labels+index));
	
	}

	//normalize ... could do in compute instead??
	for(int i=0;i<numWeights;i++) *(Dw+i) /= batch_size;
	for(int i=0;i<numBiases;i++) *(Db+i) /= batch_size;

	//learn -- update w,b
	for(int i=0;i<numWeights;i++) *(weights+i) -= learning_rate * *(Dw+i);
	for(int i=0;i<numBiases;i++) *(biases+i) -= learning_rate * *(Db+i);
	

	free(Dw);
	free(Db);

}


double* learn(double* images,int* labels,
				double* test_images,int* test_labels){

	//create the objects...

	size_create();

	double* neurons = malloc(numNeurons*sizeof(double));
	double* f_prime = malloc(numNeurons*sizeof(double));
	double* network_params = malloc((numWeights+numBiases)*sizeof(double));
		double* weights = network_params;
		double* biases  = network_params+numWeights;


	//first set all weights & biases to 0.5...
		for(int i=0;i<numWeights;i++){
			*(weights+i) = 0.1*(rand()%10)-0.5;
		}
		for(int i=0;i<numBiases;i++){
			*(biases+i) = 0.0;
		}


	//create & shuffle batches..

		//get num_batches
	int num_batches = NUM_TRAIN / batch_size; //what if extra batch???
	if( (NUM_TRAIN % batch_size) != 0) num_batches++;

		//make batch_order (diff for each epoch)
	unsigned int batch_order[num_batches]; //could make a short
	for(int i=0;i<num_batches;i++){
		*(batch_order+i) = i;
	}

	{ 
 		//before begin, find initial error ....
		double test_error = test_all(test_images,test_labels,neurons,f_prime,weights,biases);
		printf("initial test error: %.2f%% \n \n",test_error);

	}

	/*************LEARN!!!*****************/

	for(int epoch=0;epoch<num_epochs;epoch++){

		if((epoch % frequency) == 0 && epoch != 0){
			printf("dividing learning rate by %d \n",reduce_factor);
			learning_rate /= reduce_factor;
		}

		//instead shuffle data for each epoh in their entirety...
		//don't just shuffle batches --> shuffle every input data
		for(int i=0;i<NUM_TRAIN;i++){
			double copy_label = *(labels + i);
			double copy_image[SIZE];
			for(int j=0;j<SIZE;j++){
				*(copy_image+j) = *(images+i*SIZE+j);
			}

			int random = i+rand()%(NUM_TRAIN-i);

			*(labels+i) = *(labels + random);
			for(int j=0;j<SIZE;j++){
				*(images+i*SIZE+j) = *(images+random*SIZE+j);
			}

			*(labels + random) = copy_label;
			for(int j=0;j<SIZE;j++){
				*(images+random*SIZE+j) = *(copy_image+j);
			}

		}



		//learn for each batch...
		//images+batch_order[i]*batch_size*network[0],labels+batch_order[i]*batch_size, 

		double total_loss_val = 0;
		double* total_loss = &total_loss_val;

		double quad_loss_val = 0;
		double* quad_loss = &quad_loss_val;

		for(int i=0;i<num_batches;i++){ //DELETE??!

				learn_batch(images, labels, batch_order[i],
							neurons,f_prime,weights,biases,
							total_loss,quad_loss);

		}

		*total_loss /= num_batches * batch_size;
		*quad_loss /= num_batches * batch_size;

		//find error on test data

		double test_error = test_all(test_images,test_labels,neurons,f_prime,weights,biases);

		printf("epoch #%d complete |",(epoch+1));
		printf("cross-entropy loss = %.12f |",*total_loss);
		printf("quadratic loss = %.6f |",*quad_loss);
		printf("test error = %.2f%% \n",test_error);

	}




	free(neurons);
	free(f_prime);
	return network_params;
}



