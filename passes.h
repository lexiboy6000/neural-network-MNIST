


//non-normalized foward pass -->
void foward_pass(double* neurons, double* f_prime, double* input,
					double* weights, double* biases){

	//set 0th layer = input

	for(int i=0;i<network[0];i++){
		*(neurons + i) = *(input + i);
	}

	//******compute subsequent layers**********

	int n_index = 0;
	int n_index_next = network[0];
	int w_index = 0;
	int b_index = 0;

	//i represents the current layer
	//i_cur is the index of neurons in current layer
	//i_next is the index of neurons in the next layer

	for(int i=0;i<network_size-2;i++){ //for each layer

		for(int i_next=0; i_next<network[i+1];i_next++){ //for each index of next layer


			double f = 0;
			for(int i_cur=0;i_cur<network[i];i_cur++){ //for each index of cur layer
				f += *(weights + w_index + i_cur)*
						*(neurons + n_index + i_cur);
			}
			f += *(biases + b_index + i_next); // = i_next component( W*(neurons of layer i) + b )

				*(neurons + n_index_next + i_next) = sigmoid(f); 
				//PROBLEM!! f_prime doesn't actually have any data in first layer...
				*(f_prime + n_index_next + i_next) = sigmoid_prime(*(neurons + n_index_next + i_next));

			w_index += network[i];
		}

		b_index += network[i+1];
		n_index_next += network[i+1];
		n_index += network[i];

	}

	//last layer calculation .... using SOFTMAX NOT SIGMOID

	//first find f values & stabelize them...
	unsigned long smallest_double = 0xFFEFFFFFFFFFFFFF;
	double max_f = *(double*) &smallest_double;
	double f[network[network_size-1]];

	for(int i_next = 0;i_next<network[network_size-1];i_next++){ //for each i_next in last layer

		*(f+i_next) = 0;
		for(int i_cur=0;i_cur<network[network_size-2];i_cur++){ // for each i_cur in cur layer
			*(f+i_next) += *(weights + w_index + i_cur)*
							*(neurons + n_index + i_cur);
		}
		*(f+i_next) += *(biases + b_index + i_next); 

		if(*(f+i_next) > max_f) max_f = *(f+i_next);

		w_index += network[network_size-2];
	}

	double sum = 0;
	for(int i_next = 0;i_next<network[network_size-1];i_next++){

		*(f+i_next) -= max_f; //if f>710, e^f too big so we do this. 

		*(neurons + n_index_next + i_next) = my_exp(*(f+i_next));
		*(f_prime + n_index_next + i_next) = 1.0; //softmax_prime(*(neurons + n_index_next + i_next));

		sum += *(neurons + n_index_next + i_next);
	}
	for(int i_next = 0;i_next<network[network_size-1];i_next++){
		*(neurons + n_index_next + i_next) /= sum;
	}


}

void back_prop(double* Dw, double* Db, double* M_Initial,
				double* f_prime,double* neurons, double* weights, double* biases){

	double* output_f_prime = f_prime + numNeurons-network[network_size-1];
	//first copy M_Initial into M
	double* M = malloc(network[network_size-1]*sizeof(double));
	for(int m=0;m<network[network_size-1];m++){
		*(M+m) = *(M_Initial + m);
	}

	int n_index = numNeurons-1 - network[network_size-1];
	int f_prime_index = numNeurons-1;
	int w_index = numWeights-1;
	int b_index = numBiases-1;

	for(int i=network_size-1;i>1;i--){ 	//i = layers

		double* M_new = malloc(network[i-1]*sizeof(double));
		for(int m=0;m<network[i-1];m++) *(M_new + m) = 0;

		for(int i_cur = 0; i_cur < network[i]; i_cur++){ //current layer

			*(Db + b_index - i_cur) += *(M + network[i]-1 - i_cur)*
										*(f_prime + f_prime_index - i_cur);

			
			for(int i_prev=0;i_prev < network[i-1];i_prev++){ //prev layer

				*(Dw + w_index - i_prev) += *(Db + b_index - i_cur)*
											*(neurons + n_index - i_prev); 

			*(M_new + network[i-1]-1 - i_prev) += *(Db + b_index - i_cur)*
													*(weights + w_index - i_prev);
			}
			w_index -= network[i-1];
		}

		n_index -= network[i-1];
		b_index -= network[i];
		f_prime_index -= network[i];

		free(M); 
		M = M_new; 


	}


	//for i = 1 --> 

		for(int i_cur = 0; i_cur < network[1];i_cur++){

			*(Db + b_index - i_cur) += *(M + network[1]-1 - i_cur)*
										*(f_prime + f_prime_index - i_cur);
			for(int i_prev=0;i_prev < network[0];i_prev++){

				*(Dw + w_index - i_prev) += *(Db + b_index - i_cur)*
											*(neurons + n_index - i_prev);
			}

			w_index -= network[0];
		}

		free(M);


}
