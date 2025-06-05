const double LOG_TWO = 0.6931471805599453;
const int factorial[] = {1,1,2,6,24,120,720,5040,40320,362880,3628800};
const int num_factorial = 11;
const int num_newton_itt = 5; //for log(mantissa) --> could prob make smaller

double square(double x){
	return x*x;
}


double abs_double(double x){
	double result = x; //nessesary cuz need the address

	unsigned long* bits = (unsigned long*) &result;

	*bits &= 0x7FFFFFFFFFFFFFFF; //2^63 - 1

	return result;

}


double int_exp(double x, int power){

	double result = 1;
	for(int i=0;i<power;i++){
		result*=x;
	}

	return result;
}

double taylor_exp(double a){

	if(a == 0)
		return 1;
	else{
		double result=0;

		for(int n=0;n<num_factorial;n++){ 
			result += int_exp(a,n) / factorial[n];
		}

		return result;

	}

}

/*the idea--

 (i)	we compute e^x = 2^r * e^a where...
		double x = (integer r)*ln(2) + (remainder a)

(ii)	(integer r) <= 1023 since that is what a double can handle
		--> so can put in a short/int/long

		2^r = bit shift (integer r - 1023) into exponant of double

(iii)	0 <(remainder a) < ln(2) = 0.69, hence, applying taylor series
		will conv. very quickly:

		e^a = taylor_exp(a)

NOTE: can actually improve by guarenteeing mag(remainder a) < ln(2)/2
(hint: round r to nearest long rather than always casting down (floor))
	
*/
double abs_exp(double x){ //inserted x MUST be positive

	long r = (long) (x/LOG_TWO);
	double a = x - r*LOG_TWO;

	//if(r > 1023) //FIX --> what if e^(-inf), then should = 0
		//perror("exponantiation too large OR zero");

	long placeholder = (r+1023)<<52;

	double two_r = *(double*) &placeholder;

	double e_a = taylor_exp(a);

	return two_r*e_a;

}

double my_exp(double x){ //combine with abs_exp to account for 0 = 1/overflow

	if(x == 0)
		return 1;
	else if(x < 0)
		return 1/abs_exp(abs_double(x)); //will say exp too large when 0 --> overflows ALLOWED!
	else
		return abs_exp(x);
}


// making the my_log(x) function....

int get_float_exp(double x){

	double copy = x;
	unsigned long* ptr = (unsigned long*) &copy;

	//*ptr &= 0xFFF0000000000000; 
	*ptr >>= 52;
	*ptr -= 1023;

	return (int) *ptr;
}

double get_float_man(double x){

	double copy = x;
	unsigned long* ptr = (unsigned long*) &copy;

	*ptr &= 0x000FFFFFFFFFFFFF; //sets exponant = 0
	*ptr |= 0x3FF0000000000000; //puts 1023 into exponant

	return copy;
}

double newton_log(double x){

	double z = 0;

	for(int i=0;i<num_newton_itt;i++){
		z = z + x*my_exp(-z) - 1;
	}

	return z;

}

double my_log(double x){

	if(x < 0) perror("ln(x) is undefined");
	if(x == 1) return 0;
	if(x == 2) return 1; 

	return newton_log(get_float_man(x)) + get_float_exp(x) * LOG_TWO;
}

//the famous fast inverse sqrt algorithm

double fast_inv_sqrt(double x) {
    double xhalf = 0.5 * x;
    unsigned long i;

    // Treat the bits of the double as a 64-bit int
    i = *(unsigned long*)&x;

    // Magic number for double precision
    i = 0x5fe6eb50c7b537a9 - (i >> 1);

    // Convert back to double
    x = *(double*)&i;

    // TWO iteration of Newton-Raphson to improve precision 
    x = x * (1.5 - xhalf * x * x);
    x = x * (1.5 - xhalf * x * x); // can possibly delete 2nd itt

    return x;
}