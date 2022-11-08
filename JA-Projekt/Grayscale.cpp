#include <Windows.h>
#include <stdlib.h>
#include <vector>
#include <thread>
#include "Grayscale.h"

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <vector>
#include <thread>

// wska�nik na procedur� w ASM
typedef int(*grayscaleFuncASM)(GrayscaleParams* params);
// wska�nik na funkcj� w CPP
typedef void(*grayscaleFuncCPP)(GrayscaleParams* params);

using namespace std;

void runGrayscale(GrayscaleParams* params, char* fileName, int threadsNumber, HWND hwnd, bool useASM, vector<thread>& threads, char* time, HWND progress)
{
	uint8_t *input, *output;
	uint32_t fullWidth, fullHeight, bytesPerPixel;

	SetWindowText(progress, "Otwieranie pliku wej�ciowego");
	int width, height, channels;
	unsigned char* img = stbi_load(fileName, &width, &height, &channels, 0);
	if (img == NULL) {
		MessageBox(hwnd, "Otwarcie obrazu nie powiod�o si�", "B��d", MB_OK);
		return;
	}
	input = (uint8_t*)img;
	fullWidth = (uint32_t)width;
	fullHeight = (uint32_t)height;
	bytesPerPixel = (uint32_t)channels;


	SetWindowText(progress, "Alokacja pami�ci dla pliku wyj�ciowego");
	size_t imageSize = fullWidth * fullHeight * bytesPerPixel;

	unsigned char* grayImg = (unsigned char*)malloc(imageSize);
	output = (uint8_t*)grayImg;

	if (grayImg == NULL) {
		MessageBox(hwnd, "Alokacja miejsca na szary obraz nie powiod�a si�", "B��d", MB_OK);
		return;
	}

	SetWindowText(progress, "Podzia� obrazu na cz�ci");

	for (int i = 0; i < threadsNumber; i++) {
		params[i].Input = input;
		params[i].Output = output;
		params[i].FullHeight = fullHeight;
		params[i].FullWidth = fullWidth;
		params[i].BytesPerPixel = bytesPerPixel;
	}

	if (fullHeight > fullWidth) {
		uint32_t AreaHeight = floor(fullHeight / threadsNumber);
		for (int i = 0; i < threadsNumber - 1; i++) {
			params[i].StartXCoord = 0;
			params[i].StartYCoord = AreaHeight * i;
			params[i].AreaHeight = AreaHeight;
			params[i].AreaWidth = fullWidth;
		}
		params[threadsNumber - 1].StartXCoord = 0;
		params[threadsNumber - 1].StartYCoord = AreaHeight * (threadsNumber - 1);
		params[threadsNumber - 1].AreaHeight = fullHeight - AreaHeight * (threadsNumber - 1);
		params[threadsNumber - 1].AreaWidth = fullWidth;
	}
	else {
		uint32_t AreaWidth = floor(fullWidth / threadsNumber);
		for (int i = 0; i < threadsNumber - 1; i++) {
			params[i].StartXCoord = AreaWidth * i;
			params[i].StartYCoord = 0;
			params[i].AreaHeight = fullHeight;
			params[i].AreaWidth = AreaWidth;
		}
		params[threadsNumber - 1].StartXCoord = AreaWidth * (threadsNumber - 1);
		params[threadsNumber - 1].StartYCoord = 0;
		params[threadsNumber - 1].AreaHeight = fullHeight;
		params[threadsNumber - 1].AreaWidth =  fullWidth - AreaWidth * (threadsNumber - 1);
	}

	SetWindowText(progress, "transformacja obrazu wej�ciowego na monochromatyczny");

	if (useASM == true) {

		// dynamiczne �adowanie biblioteki
		HINSTANCE hlib;
		hlib = LoadLibrary("JALib");

		if (hlib == NULL) {  // j�sli nie uda�o si� za�adowa� biblioteki
			MessageBox(hwnd, "Otwarcie DLL nie powiod�o si�", "B��d", MB_OK);
			return;
		}
		else {
			// �adowanie procedury z biblioteki DLL
			grayscaleFuncASM p = (grayscaleFuncASM)GetProcAddress(hlib, "grayscaleImage");
			if (p == NULL) { // j�sli nie uda�o si� za�adowa� procedury
				MessageBox(hwnd, "Za�adowanie procedury bibliotecznej nie powiod�o si�", "B��d", MB_OK);
				return;
			}
			else {
				clock_t start, end;
				start = clock();

				for (int i = 0; i < threadsNumber; i++) {
					threads.push_back(thread(p, &params[i]));
				}
				for (auto& t : threads) {
					t.join();
				}
				threads.clear();

				end = clock();
				double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
				snprintf(time, sizeof(time), "%f", cpu_time_used);
			}
		}
		FreeLibrary(hlib);

	}
	else {
		// dynamiczne �adowanie biblioteki
		HINSTANCE hlib2;
		hlib2 = LoadLibrary("JALib2");

		if (hlib2 == NULL) { // j�sli nie uda�o si� za�adowa� biblioteki
			MessageBox(hwnd, "Otwarcie DLL nie powiod�o si�", "B��d", MB_OK);
			return;
		}
		else {
			// �adowanie funkcji z biblioteki DLL
			grayscaleFuncCPP p2 = (grayscaleFuncCPP)GetProcAddress(hlib2, "Grayscale");
			if (p2 == NULL) { // j�sli nie uda�o si� za�adowa� funkcji
				MessageBox(hwnd, "Za�adowanie funkcji bibliotecznej nie powiod�o si�", "B��d", MB_OK);
				return;
			}
			else {
				clock_t start, end;
				start = clock();

				for (int i = 0; i < threadsNumber; i++) {
					threads.push_back(thread(p2, &params[i]));
				}
				for (auto& t : threads) {
					t.join();
				}
				threads.clear();

				end = clock();
				double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
				snprintf(time, sizeof(time), "%f", cpu_time_used);
			}
		}
		FreeLibrary(hlib2);
	}
	
	SetWindowText(progress, "Zapis monochromatycznego obrazu do pliku wyj�ciowego");

	char* gray = (char*)"gray";
	char* grayFileName = (char*)malloc(strlen(fileName) + strlen(gray));
	strcpy(grayFileName, gray);
	strcat(grayFileName, fileName);

	stbi_write_jpg(grayFileName, fullWidth, fullHeight, bytesPerPixel, output, 100);

	stbi_image_free(img);
	stbi_image_free(grayImg);


	SetWindowText(progress, "Program zako�czy� dzia�anie");

	return;
}
