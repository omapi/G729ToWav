#include "transfer.h"

short ulaw_exp_table[256] = {
	 -32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,
	 -23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,
	 -15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,
	 -11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316,
	  -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140,
	  -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092,
	  -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004,
	  -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980,
	  -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436,
	  -1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924,
	   -876,  -844,  -812,  -780,  -748,  -716,  -684,  -652,
	   -620,  -588,  -556,  -524,  -492,  -460,  -428,  -396,
	   -372,  -356,  -340,  -324,  -308,  -292,  -276,  -260,
	   -244,  -228,  -212,  -196,  -180,  -164,  -148,  -132,
	   -120,  -112,  -104,   -96,   -88,   -80,   -72,   -64,
	    -56,   -48,   -40,   -32,   -24,   -16,    -8,     0,
	  32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956,
	  23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764,
	  15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412,
	  11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316,
	   7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140,
	   5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092,
	   3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004,
	   2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980,
	   1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436,
	   1372,  1308,  1244,  1180,  1116,  1052,   988,   924,
	    876,   844,   812,   780,   748,   716,   684,   652,
	    620,   588,   556,   524,   492,   460,   428,   396,
	    372,   356,   340,   324,   308,   292,   276,   260,
	    244,   228,   212,   196,   180,   164,   148,   132,
	    120,   112,   104,    96,    88,    80,    72,    64,
	     56,    48,    40,    32,    24,    16,     8,     0};

short nsearch(short val,short size)
{
	short i;
	short seg_end[8] = {0xff,0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff};

	for(i=0;i<size;i++){
		if(val<=seg_end[i])
			return i;
	}
	return size;
}

unsigned char linear2ulaw(short val)
{
	short mask;
	short seg;
	unsigned char uval;

	if(val<0){
		val = 0x84 - val;
		mask = 0x7f;
	}
	else{
		val += 0x84;
		mask = 0xff;
	}

	seg = nsearch(val,8);
	if(seg>=8)
		return (0x7f ^ mask);
	else{
		uval = (seg << 4) | ((val >> (seg+3)) & 0xf);
		return (uval ^ mask);
	}
}

int trans16(char *infile, char *tmpfile)
{
	unsigned char ucdata[256];
	short usdata[256];
	int count = 0;
	int i;
	FILE *fin = NULL, *fout = NULL;

	fin = fopen(infile, "rb");
	if(fin == NULL){
		return -1;
	}
	fout = fopen(tmpfile, "wb");
	if(fout==NULL){
		fclose(fin);
		return -1;
	}

	while((count=fread(&ucdata,1,256,fin)) > 0){
		for(i=0;i<count;i++)
			usdata[i] = ulaw_exp_table[ucdata[i]];
		fwrite(usdata, 2, count, fout);
	}
	fclose(fin);
	fclose(fout);
	return 0;
}

int trans8(char *tmpfile, char *outfile)
{
	unsigned char ucdata[256];
	short usdata[256];
	int count = 0;
	int i;
	FILE *fin = NULL, *fout = NULL;

	fin = fopen(tmpfile, "rb");
	if(fin == NULL){
		return -1;
	}
	fout = fopen(outfile, "wb");
	if(fout==NULL){
		fclose(fin);
		return -1;
	}

	while((count=fread(&usdata,2,256,fin)) > 0){
		for(i=0;i<count;i++)
			ucdata[i] = linear2ulaw(usdata[i]);
		fwrite(ucdata, 1, count, fout);
	}
	fclose(fin);
	fclose(fout);
	return 0;
}

//pcmfile for 8bit, transfer to wave file can be play
int PcmuToWave(char *pcmfile, char *wavfile)
{
	unsigned char heads[44];
	unsigned char pcmu_data[80];
	short wav_data[80];
	int count = 0, sub_size = 0;
	int i, total_size = 0;
	FILE *fin = NULL, *fout = NULL;
	struct stat fileInfo;

	if(stat(pcmfile,&fileInfo)!=0 || fileInfo.st_size==0){
		return -1;
	}
	sub_size = fileInfo.st_size;

	fin = fopen(pcmfile, "rb");
	if(fin == NULL){
		return -1;
	}
	fout = fopen(wavfile, "wb");
	if(!fout){
		fclose(fin);
		return -1;
	}

	//read wave header and skip, 12B
	memset(heads,0,44);
	memcpy(heads, "RIFF",4);
	*((unsigned long *)&heads[4]) = sub_size*2 + 44 - 8;//44B - 8
	memcpy(heads+8,"WAVE",4);

	//8+16 B sub chunck
	memcpy(heads+12, "fmt ",4);
	*((unsigned short *)&heads[16]) = 16;
	*((unsigned short *)&heads[18]) = 0;

	*((unsigned short *)&heads[20]) = 1; //pcm format: pcmu, 0x0001:microsoft pcm
	*((unsigned short *)&heads[22]) = 1; //numer of channel, if 1,mono, 2,stereo
	*((unsigned short *)&heads[24]) = 8000; //sample rate
	*((unsigned short *)&heads[26]) = 0; 
    
  	*((unsigned short *)&heads[28]) = 2*8000;
	*((unsigned short *)&heads[30]) = 0; 	
	*((unsigned short *)&heads[32]) = 2; //block align
	*((unsigned short *)&heads[34]) = 16; //bits per sample 


	//8B data header
	memcpy(heads+36, "data",4);
	*((unsigned long *)&heads[40]) = sub_size*2;

	fwrite(heads,1,44,fout);

	//read data
	while((count=fread(&pcmu_data,1,80,fin)) > 0){
		for(i=0;i<count;i++){
			wav_data[i] = ulaw_exp_table[pcmu_data[i]];
		}
		fwrite(wav_data,2,count,fout);
		total_size += count;
	}
	fclose(fin);
	fclose(fout);

	if(total_size != sub_size){
		return -1;
	}

	remove(pcmfile);
	return 0;
}

//g729file for OM, transfer to pcm file  8bit can be play on OM(need to transfer ?)
//nflag: input flag, if 0, delete file *.dat, or not delete
int G729ToPcmu(char *g729file, char *pcmfile, int nflag)
{
	FILE *fg729 = NULL;
	FILE *fpcm16 = NULL;
	char pcm16file[256];
	unsigned char	serial[L_FRAME_COMPRESSED];
	short synth[L_FRAME];
	int bfi;

	if(access(g729file,0)<0)
		return -1;

	memset(pcm16file,0,256);
	sprintf(pcm16file,"%s.tmp", g729file);

	fg729 = fopen(g729file, "rb");
	if(fg729==NULL)
		return -1;
	fpcm16 = fopen(pcm16file, "wb");
	if(fpcm16==NULL){
		fclose(fg729);
		return -1;
	}
	
	va_g729a_init_decoder();
	//convert g729 to pcm
	while(fread(serial, sizeof(char), L_FRAME_COMPRESSED, fg729)==L_FRAME_COMPRESSED){
		//printf("Decode frame %d\r", ++nb_frame);
		/*--------------------------------------------------------------*
		 * Bad frame                                                    *
		 *                                                              *
		 * bfi = 0 to indicate that we have a correct frame             *
		 * bfi = 1 to indicate that we lost the current frame           *
		 *--------------------------------------------------------------*/
		bfi = 0;
		/*--------------------------------------------------------------*
		 * Call the decoder.                                            *
		 *--------------------------------------------------------------*/
		va_g729a_decoder(serial, synth, bfi);
		/*--------------------------------------------------------------*
		 * Output synthesis to disk                                     *
		 *--------------------------------------------------------------*/
		fwrite(synth, sizeof(short), L_FRAME, fpcm16);
	}
	fclose(fpcm16);
	fclose(fg729);

	if(trans8(pcm16file,pcmfile)<0)
		return -1;

	remove(pcm16file);
	if(nflag==0){
		remove(g729file);
	}
	
	return 0; 
}

void Mix(char sourseFile[10][2], int number, char* objectFile)
{
	int const MAX = 32767;
    	int const MIN = -32768;
   	double f = 1;
    	int output;
    	int j = 0;
    	int temp = 0;

    	for(j = 0; j<number; j++){
       	temp += *(short*)(sourseFile[j]);
    	}
    	output = (int)(temp*f);
    	if(output>MAX){
        	f = (double)MAX/(double)(output);
        	output = MAX;
    	}
    	if(output<MIN){
        	f = (double)MIN/(double)(output);
        	output = MIN;
    	}
    	if(f<1){
       	f += ((double)1 - f)/(double)32;
    	}
    	*(short*)(objectFile) = (short)output;
}

int addwav(char *pwav1,char *pwav2,char *pwav3)
{
	FILE *fp1=NULL, *fp2=NULL, *fpm=NULL;
	short data1, data2, date_mix;
    	int ret1, ret2;
	int nlen1,nlen2,nlen3;
    	char sourseFile[10][2];
	unsigned char heads[44];

    	fp1 = fopen(pwav1,"rb");
	if(fp1 == NULL){
		return -1;
	}
	fseek(fp1, 0, SEEK_END);
	nlen1=ftell(fp1);
	fclose(fp1);
    	fp2 = fopen(pwav2,"rb");
	if(fp2 == NULL){
		fclose(fp1);
		return -1;
	}
	fseek(fp2, 0, SEEK_END);
	nlen2=ftell(fp2);
	fclose(fp2);
    	fpm = fopen(pwav3,"wb");
	if(fpm == NULL){
		return -1;
	}
 
	if(nlen1>nlen2)
		nlen3 = nlen1;
	else
		nlen3 = nlen2;

	memset(heads,0,44);
	memcpy(heads, "RIFF",4);
	*((unsigned long *)&heads[4]) = nlen3 + 44 - 8;//44B - 8
	memcpy(heads+8,"WAVE",4);

	//8+16 B sub chunck
	memcpy(heads+12, "fmt ",4);
	*((unsigned short *)&heads[16]) = 16;
	*((unsigned short *)&heads[18]) = 0;

	*((unsigned short *)&heads[20]) = 1; //pcm format: pcmu, 0x0001:microsoft pcm
	*((unsigned short *)&heads[22]) = 1; //numer of channel, if 1,mono, 2,stereo
	*((unsigned short *)&heads[24]) = 8000; //sample rate
	*((unsigned short *)&heads[26]) = 0; 
    
  	*((unsigned short *)&heads[28]) = 2*8000;
	*((unsigned short *)&heads[30]) = 0; 	
	*((unsigned short *)&heads[32]) = 2; //block align
	*((unsigned short *)&heads[34]) = 16; //bits per sample 

	//8B data header
	memcpy(heads+36, "data",4);
	*((unsigned long *)&heads[40]) = nlen3;

	fwrite(heads,1,44,fpm);

	fp1 = fopen(pwav1,"rb");
	memset(heads,0,44);
	fread(&heads, 44, 1, fp1);
    	fp2 = fopen(pwav2,"rb");
	memset(heads,0,44);
	fread(&heads, 44, 1, fp2);
    	while(1){
        	ret1 = fread(&data1, 2, 1, fp1);
        	ret2 = fread(&data2, 2, 1, fp2);
        	*(short*)sourseFile[0] = data1;
        	*(short*)sourseFile[1] = data2;
        	if(ret1>0 && ret2>0){
            		Mix(sourseFile,2,(char*)&date_mix);
            		if(date_mix>pow(2.0, 16-1) || date_mix<-pow(2.0, 16-1)){
	                	//printf("mix error\n");
				fclose(fp1);
				fclose(fp2);
				fclose(fpm);
				return -1;
			}
        	}
	        else if((ret1 > 0) && (ret2 == 0)){
	            	date_mix = data1;
	        }
	        else if((ret2 > 0) && (ret1 == 0)){
	            	date_mix = data2;
	        }
	        else if((ret1 == 0) && (ret2 == 0)){
	            	break;
	        }
        	fwrite(&date_mix, 2, 1, fpm);
    	}
    	fclose(fp1);
    	fclose(fp2);
    	fclose(fpm);
	
	remove(pwav1);
	remove(pwav2);
	return 0;
}

void ConvertG729(char *pdatfile1, char *pdatfile2, char *pwavfile, int nflag)
{
	char f1[256],f2[256],f3[256],f4[256],f5[256];
	int nres,nlen;
	FILE *fp =NULL;

	if(pdatfile1==NULL && pdatfile2==NULL)
		return;
	if(pdatfile1 && !strstr(pdatfile1,".dat"))
		return;
	if(pdatfile2 && !strstr(pdatfile2,".dat"))
		return;

	memset(f1,0,256);
	memset(f2,0,256);
	memset(f3,0,256);
	memset(f4,0,256);
	memset(f5,0,256);

	if(pdatfile1 && pdatfile2){
		nlen = strlen(pdatfile1);
		strncpy(f1,pdatfile1,nlen-4);//.dat
		strcpy(f4,f1);
		strcat(f1,".wav");
		strcat(f4,".raw");
		G729ToPcmu(pdatfile1,f4,nflag);
		PcmuToWave(f4,f1);

		nlen = strlen(pdatfile2);
		strncpy(f2,pdatfile2,nlen-4);//.dat
		strcpy(f5,f2);
		strcat(f2,".wav");
		strcat(f5,".raw");
		G729ToPcmu(pdatfile2,f5,nflag);
		PcmuToWave(f5,f2);
		
		nlen = strlen(pdatfile1);
		while(*(pdatfile1+nlen)!='_')nlen--;
		strncpy(f3,pdatfile1,nlen);
		strcat(f3,".wav");
		fp = fopen(f3,"rb");
		if(fp!=NULL){
			fclose(fp);
			strcpy(pwavfile,f3);
			return;
		}
		nres = addwav(f1,f2,f3);
		if(nres<0)
			return;
		strcpy(pwavfile,f3);
	}
	else if(pdatfile1){
		nlen = strlen(pdatfile1);
		while(*(pdatfile1+nlen)!='_')nlen--;
		strncpy(f1,pdatfile1,nlen);
		strcpy(f4,f1);
		strcat(f1,".wav");
		strcat(f4,".raw");
		G729ToPcmu(pdatfile1,f4,nflag);
		PcmuToWave(f4,f1);
		strcpy(pwavfile,f1);
	}
	else if(pdatfile2){
		nlen = strlen(pdatfile2);
		while(*(pdatfile2+nlen)!='_')nlen--;
		strncpy(f2,pdatfile2,nlen);
		strcpy(f5,f2);
		strcat(f2,".wav");
		strcat(f5,".raw");
		G729ToPcmu(pdatfile2,f5,nflag);
		PcmuToWave(f5,f2);
		strcpy(pwavfile,f2);
	}
}
