#include <stdio.h>
#include <stdlib.h>

int dead_compute01( int *a, int size){

   int j=0 ;
   long count=0;

   for (j=0; j<size*3; j++){

	int i, tmp=0;

	for (i = 0; i<size; i++){
		a[i] = i*i-tmp;
	        tmp += i;
	}

	printf("%s: %d\t", __FUNCTION__, a[(i+j)/4]);
	if(j%10 == 0) printf("\n");

	for (i =0 ; i< size; i++){
		a[i] = i*i/2;
                tmp = tmp + 2*i -200;
	}

	count += tmp * 2 - tmp / 3;

    }
	return 0;
}

int main(){
	int size = 2000;
	int *a = malloc (size * sizeof(int));
	dead_compute01( a, size);
	int i;
	for (i = size-1; i>=0; i--){
		printf("a[%d]=%d\t", i, a[i]);
		if(i%10==0) printf("\n");
	}
	free(a);
	return 0;
}
