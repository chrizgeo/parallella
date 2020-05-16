/*  Program to Calculate Fractal image in Sequential manner */
/* 	Fractal images are calculated by calculating a simple equation many times
	feeding the answer back into the start  */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"
#include "common.h"

#include <e-loader.h>
#include "e-hal.h"

#define FAIL(...) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define SHM_OFFSET 0x01000000

e_platform_t platform;
e_epiphany_t dev;
e_mem_t      emem;

int compute_image(int max_iterations, shm_t* shm)
{
														/* Decalaration of Local variables */
	img *C;       

	/* Size of the Image Decalaration */
	int i, j, k, l;		/* loop variables for iteration */
	unsigned cores_needed = CORES;
	unsigned corenum = 0;
	int done[16], all_done;
	unsigned clr = 0;
	unsigned set = 1;
	double x_min, y_min;
	double x_max, y_max;
	double x_step, y_step;
	int data[N][N];				
	
	C = new_image(max_iterations,max_iterations);	 	/* allocate memory for empty image of size 64x64 */
	
	/* x and y are the real and imaginary part in the fractal image calculation */
	x_min = -1.5, y_min = -1.5;							/*  */
	x_max = 1.5, y_max = 1.5;
	x_step = (x_max-x_min)/max_iterations;
	y_step = (y_max-y_min)/max_iterations;	

	shm->x_min = x_min;
	shm->x_max = x_max;
	shm->x_step = x_step;
	/* divide tasks */
	for(int ii = 0; ii < CORES; ii++) {
		shm->y_min[ii] = y_min + ii*(N/CORES)*y_step;
		shm->y_max[ii] = shm->y_min[ii] + (N/CORES)*y_step;
	}

	/* Setting Pixels in Image data */
	for(k=0;k<max_iterations;k++)
	{
		for(l=0;l<max_iterations;l++)
	  	{
			data[k][l] = shm->image[k][l] = 0;
	  	}
	}

	if(E_ERR == e_write(&emem, 0, 0, (off_t)0, shm, sizeof(shm_t)))
		FAIL("Can't write to memory!\n");
  	
	  for (i=0; i<ROW; i++){
    	for (j=0; j<COL;j++){
			e_write(&dev, i, j, 0x7000, &clr, sizeof(clr));
			if(!cores_needed) {
				e_write(&dev, i, j, 0x6000, &clr, sizeof(clr));
			}
			else {
				//printf("Writing active %d on corenum %d with row %d and col %d \n", set, corenum, i, j);
				e_write(&dev, i, j, 0x6000, &set, sizeof(set));
				e_write(&dev, i, j, 0x5000, &corenum, sizeof(corenum));
				++corenum;
				--cores_needed;
			}
		}
	}
	printf("Starting execution on cores\n");
	// start cores
  	e_start_group(&dev);													

 	/* Check if all cores are done */
  	while(1){    
		printf(".");
    	all_done=0;
    	for (i=0; i<ROW; i++){
      		for (j=0; j<COL;j++){
				e_read(&dev, i, j, 0x7000, &done[i*COL + j], sizeof(int));
				all_done+=done[i*COL+j];
			}
		}
    	if(all_done==CORES){
			printf("\nAll cores done computation\n");
      		break;
  		}
	} 
	
	/* read shm , the row, col numbers does not matter since we read from memory buff*/
	if(E_ERR == e_read(&emem, 1, 2, (off_t)0, shm,	sizeof(shm_t)))
	 	FAIL("Can't poll!\n");
	
	for(int ii=0; ii<CORES; ii++) {
		/* print some info */
		printf("Info Core (%d %d) coreid %d corenum %d active %d \n y_min %f \n ", shm->row[ii], shm->col[ii], shm->coreid[ii], shm->corenum[ii], shm->active[ii], shm->y_min[ii]);
	}

	/* Setting Pixels in Image data */
	for(k=0;k<max_iterations;k++)
	{
		for(l=0;l<max_iterations;l++)
	  	{
			data[k][l] = shm->image[k][l];
	   		set_pixel(C, k, l, data[k][l]);
	  	}
	}
	save_image(C,"C.pgm",1);
	return 0;
}
int main (int argc, char *argv[])
{

	shm_t shm;
	FILE *fp;

	/* init board */
	if(E_OK != e_init(NULL))
		FAIL("Can't init!\n");
	/* reset epiphany chip */
	e_reset_system();
	/* get platform information for the rows and columns */
	e_get_platform_info(&platform);
	
	/* open workgroup */
	if(E_OK != e_open(&dev, 0, 0, ROW, COL))
		FAIL("Can't open workgroup!\n");
	
	/* initialize and allocate shared memory buffer */
	if(E_OK != e_alloc(&emem, SHM_OFFSET, sizeof(shm_t)))
		FAIL("Can't alloc!\n");
	/* =============================================================== */
	 /*load program to the subgroup */
	 /*Loading the program to all 16 cores irrespective of the number of cores used, until we find a better solution for indexing data */
	if(E_OK != e_load_group("e_main.elf", &dev, 0, 0, ROW, COL, E_FALSE))
	  	FAIL("Can't load! \n")
	
	if(!compute_image(N, &shm))
		printf("Image computation done\n");
	else 
		FAIL("Can't compute the image\n");

	printf("Total Execution Time: %llu us\n", shm.total_us);
	fp = fopen("time_log.csv", "a");
	fprintf(fp, "%d,%d,%d,%llu\n", N, N, CORES, shm.total_us); /* Width,Height,CORES,Cycles,US,clocktime */
	fclose(fp);
	printf("Program finished.\n");
	
		/* free shm buffer, close workgroup and finalize */
	if(E_OK != e_free(&emem)) FAIL("Can't free!\n");
	if(E_OK != e_close(&dev)) FAIL("Can't close!\n");
	if(E_OK != e_finalize())  FAIL("Can't finalize!\n");

	return(0);
}
