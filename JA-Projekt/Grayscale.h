#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <thread>

struct GrayscaleParams {
	uint8_t* Input;         // tablica wejœciowa z wartoœciami pikseli
	uint8_t* Output;        // tablica wyjœciowa z wartoœciami pikseli
	uint32_t StartXCoord;   // pozycja startowa na tablicy wej. na osi X
	uint32_t StartYCoord;   // pozycja startowa na tablicy wej. na osi Y
	uint32_t AreaWidth;     // szerokoœæ czêœci obrazu do przetworzenia
	uint32_t AreaHeight;    // wysokoœæ czêœci obrazu do przetworzenia
	uint8_t  BytesPerPixel; // liczba bajtów na kolor piksela (3 lub 4)
	uint32_t FullWidth;     // szerokoœæ ca³ego obrazu
	uint32_t FullHeight;    // wysokoœæ ca³ego obrazu
};

using namespace std;

void runGrayscale(GrayscaleParams* params, char* fileName, int threadsNumber, HWND hwnd, bool useASM, vector<thread>& threads, char* time, HWND progress);

void Grayscale(GrayscaleParams params);

