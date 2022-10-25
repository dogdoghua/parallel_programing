#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    // maskIsNegative: if i-th value == 1 means that x[i] < 0
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    // make the negative value to positive
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x; (x<0)

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_int y, count;
  __pp_vec_int allOne = _pp_vset_int(1);
  __pp_vec_int allZero = _pp_vset_int(0);
   
  __pp_vec_float x, result;
  __pp_vec_float oneFloat = _pp_vset_float(1.f);
  
  __pp_vec_float maxVal = _pp_vset_float(9.999999f);
  __pp_mask maskAll, maskIseqZero, maskIsPositive, maskIsLarge, maskIsNotZero;

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    if(N % VECTOR_WIDTH != 0 && i==(N/VECTOR_WIDTH)*VECTOR_WIDTH){
      int remainPtr = N % VECTOR_WIDTH;
      maskAll = _pp_init_ones(remainPtr);
    }
    else{
      maskAll = _pp_init_ones();
    }
    
    
    // All zeros
    maskIseqZero = _pp_init_ones(0);
    maskIsPositive = _pp_init_ones(0);
    maskIsLarge = _pp_init_ones(0);

    _pp_vload_float(x, values + i, maskAll);                        // float x=values[i]
    _pp_vload_int(y, exponents + i, maskAll);                       // int y=exponents[i]

    _pp_veq_int(maskIseqZero, y, allZero, maskAll);                 // if(y==0){
    _pp_vstore_float(output + i, oneFloat, maskIseqZero);           //    output[i] = 1.f     

                                                                    // else{
    maskIsNotZero = _pp_mask_not(maskIseqZero);
    maskIsNotZero = _pp_mask_and(maskIsNotZero, maskAll);           // * avoid for out of bound issue 
    _pp_vmove_float(result, x, maskIsNotZero);                      //    float result = x
    _pp_vsub_int(count, y, allOne, maskIsNotZero);                  //    int count = y - 1

    _pp_vgt_int(maskIsPositive, count, allZero, maskIsNotZero);

    while(_pp_cntbits(maskIsPositive) > 0){                         //  while(count > 0)
      _pp_vmult_float(result, result, x, maskIsPositive);           //      result *= x
      _pp_vsub_int(count, count, allOne, maskIsPositive);           //       count--
      
      _pp_vgt_int(maskIsPositive, count, allZero, maskIsNotZero);
    }
    _pp_vgt_float(maskIsLarge, result, maxVal, maskIsNotZero);        //  if (result > 9.999999f)
    _pp_vmove_float(result, maxVal, maskIsLarge);                     //      result = 9.999999f 

    _pp_vstore_float(output + i, result, maskIsNotZero);              //  output[i] = result;

  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //
  __pp_vec_float val;
  float *sum_tmp = (float *)malloc(VECTOR_WIDTH * sizeof(float));
  float sum=0;

  __pp_mask maskAll;
  maskAll = _pp_init_ones();

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    _pp_vload_float(val, values+i, maskAll);    // load values[i ~ VECTOR_WIDTH + i] to val
    
    for(int k=VECTOR_WIDTH; k>=2; k/=2){
      _pp_hadd_float(val, val);
      _pp_interleave_float(val, val);
      _pp_vstore_float(sum_tmp, val, maskAll);
    }
    sum += sum_tmp[0];
  }

  return sum;
}