//#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "transfer.h"

int main(int argc, char *argv[])
{
	char fbuf1[256];
	char fbuf2[256];
	char fbuf3[256];
	int nset = 1;
	
	if(argc<3 || argc>4){
		printf("args error, format: \n");
		printf("please input: ./tconv ~/*.dat [~/*.dat] 1\nif want to delete dat files, set 0\n");
		return 1;
	}
	memset(fbuf1,0,256);
	memset(fbuf2,0,256);
	memset(fbuf3,0,256);
	if(argc==3){
		strcpy(fbuf1,argv[1]);
		nset = atoi(argv[2]);
		ConvertG729(fbuf1, NULL, fbuf3, nset);
	}
	else{
		strcpy(fbuf1,argv[1]);
		strcpy(fbuf2,argv[2]);
		nset = atoi(argv[3]);
		ConvertG729(fbuf1, fbuf2, fbuf3, nset);
	}
	
	printf("result: %s\n", fbuf3);
	return 0;
}

