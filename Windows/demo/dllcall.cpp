#include <stdio.h>
#include <windows.h>

typedef void(*lpConvertFun)(char*, char*, char*, int);

int main(int argc, char *argv[])
{
	HINSTANCE hDll; //DLL¾ä±ú
	lpConvertFun ConvertFun; //º¯ÊýÖ¸Õë
	char strwav[256];
	char senddat[256];
	char recvdat[256];

	if(argc<3 || argc>4){
		printf("please input: dllcall.exe ~/*.dat [~/*.dat] 1\nif want to delete dat files, set 0\n");
		system("pause");
		return 1;
	}
	memset(senddat,0,256);
	memset(recvdat,0,256);
	strcpy(senddat,argv[1]);
	if(argc==4){
		strcpy(recvdat,argv[2]);
	}

	hDll = LoadLibrary("Transdat.dll");
	if (hDll != NULL)
	{
		ConvertFun = (lpConvertFun)GetProcAddress(hDll, "ConvertG729");
		if (ConvertFun != NULL)
		{
			memset(strwav,0,256);
			if(argc==4)
				ConvertFun(senddat, recvdat, strwav,1);
			else
				ConvertFun(senddat, NULL, strwav,1);
			printf("result: %s\n", strwav);
		}
		FreeLibrary(hDll);
	}
	return 0;
}