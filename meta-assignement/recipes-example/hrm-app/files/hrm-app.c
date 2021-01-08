#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define q	11		    /* for 2^11 points */
#define N	2048		/* N-point FFT, iFFT */
#ifndef PI
# define PI	3.14159265358979323846264338327950288
#endif

typedef float real;
typedef struct{real Re; real Im;} complex;
struct timespec tw = {.tv_sec=0, .tv_nsec=20000000};
int ppg[N];
int fd = -1;
int k;
char valr[6];

void *lettura (void *arg){

read(fd,&valr,5);
ppg[k]=atoi(valr);
pthread_exit(NULL);
}


void fft( complex *v, int n, complex *tmp )
{
  if(n>1) {			/* otherwise, do nothing and return */
    int k,m;    complex z, w, *vo, *ve;
    ve = tmp; vo = tmp+n/2;
    for(k=0; k<n/2; k++) {
      ve[k] = v[2*k];
      vo[k] = v[2*k+1];
    }
    fft( ve, n/2, v );		/* FFT on even-indexed elements of v[] */
    fft( vo, n/2, v );		/* FFT on odd-indexed elements of v[] */
    for(m=0; m<n/2; m++) {
      w.Re = cos(2*PI*m/(double)n);
      w.Im = -sin(2*PI*m/(double)n);
      z.Re = w.Re*vo[m].Re - w.Im*vo[m].Im;	/* Re(w*vo[m]) */
      z.Im = w.Re*vo[m].Im + w.Im*vo[m].Re;	/* Im(w*vo[m]) */
      v[  m  ].Re = ve[m].Re + z.Re;
      v[  m  ].Im = ve[m].Im + z.Im;
      v[m+n/2].Re = ve[m].Re - z.Re;
      v[m+n/2].Im = ve[m].Im - z.Im;
    }
  }
  return;
}

int main(void)
{
  complex v[N], scratch[N];
  float abs[N];
  int m;
  int i;
  int minIdx, maxIdx;
   struct timeval stop, start,stop1, start1;
  pthread_t thread1;
  char *app_name = "heart monitor";
  char *dev_name = "/dev/hrm_dev";
  int iret1;
void *status;

while(1){ 
	if ((fd = open(dev_name, O_RDWR)) < 0){
	fprintf(stderr, "%s: unable to open %s: %s\n", app_name, dev_name, strerror(errno));
	return( 1 );}
       gettimeofday(&start1, NULL);

  for(k=0; k<N; k++){
	
   iret1=pthread_create( &thread1, NULL, lettura, NULL);
   nanosleep(&tw,&tw);
   pthread_join(thread1,&status);
   gettimeofday(&stop, NULL);
   printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 +     stop.tv_usec - start.tv_usec);
   gettimeofday(&start, NULL);
    
}
       gettimeofday(&stop1, NULL);
       printf("took %lu us\n", (stop1.tv_sec - start1.tv_sec) * 1000000 + stop1.tv_usec - start1.tv_usec);

	// Initialize the complex array for FFT computation
  for(k=0; k<N; k++) {
    v[k].Re = ppg[k];
    v[k].Im = 0;
  }

	// FFT computation
  fft( v, N, scratch );

	// PSD computation
  for(k=0; k<N; k++) {
	abs[k] = (50.0/2048)*((v[k].Re*v[k].Re)+(v[k].Im*v[k].Im));
  }

  minIdx = (0.5*2048)/50;   // position in the PSD of the spectral line corresponding to 30 bpm
  maxIdx = 3*2048/50;       // position in the PSD of the spectral line corresponding to 180 bpm

// Find the peak in the PSD from 30 bpm to 180 bpm
  m = minIdx;
  for(k=minIdx; k<(maxIdx); k++) {
    if( abs[k] > abs[m] )
	m = k;
  }

// Print the heart beat in bpm
  printf( "\n\n\n%d bpm\n\n\n", (m)*60*50/2048 );
close(fd);
}
  close(fd);
  exit(EXIT_SUCCESS);
}
