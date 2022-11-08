#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <thread>

struct GrayscaleParams {
	uint8_t* Input;         // tablica wej�ciowa z warto�ciami pikseli
	uint8_t* Output;        // tablica wyj�ciowa z warto�ciami pikseli
	uint32_t StartXCoord;   // pozycja startowa na tablicy wej. na osi X
	uint32_t StartYCoord;   // pozycja startowa na tablicy wej. na osi Y
	uint32_t AreaWidth;     // szeroko�� cz�ci obrazu do przetworzenia
	uint32_t AreaHeight;    // wysoko�� cz�ci obrazu do przetworzenia
	uint8_t  BytesPerPixel; // liczba bajt�w na kolor piksela (3 lub 4)
	uint32_t FullWidth;     // szeroko�� ca�ego obrazu
	uint32_t FullHeight;    // wysoko�� ca�ego obrazu
};

using namespace std;

void runGrayscale(GrayscaleParams* params, char* fileName, int threadsNumber, HWND hwnd, bool useASM, vector<thread>& threads, char* time, HWND progress);

void Grayscale(GrayscaleParams params);

