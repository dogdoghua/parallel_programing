__kernel void convolution(__global float *inputImg, int imageWidth, int imageHeight, 
                          __global float *filter, int filterWidth, __global float *outputImg) 
{
    // get grid index
    int idx = get_global_id(0);
    int row = idx / imageWidth;	
	  int col = idx % imageWidth;

	  int halfFilterSize = filterWidth / 2;
	  float sum = 0.0f;
	  int k, l;	

	  for(k = -halfFilterSize; k<= halfFilterSize; k++){
		    for(l = -halfFilterSize; l<= halfFilterSize; l++){
            
            if(row + k >= 0 && row + k < imageHeight && 
                col + l >= 0 && col + l < imageWidth){
                sum += inputImg[(row + k) * imageWidth + col + l] * 
                filter[(k + halfFilterSize) * filterWidth + l + halfFilterSize];
            }
            
		    }
  	}
  	outputImg[row * imageWidth + col] = sum;
}
