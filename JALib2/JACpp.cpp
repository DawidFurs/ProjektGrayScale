#include "pch.h"
#include <cstdint>

// struktura u¿ywana w projekcie
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

// funkcja w C++, wywo³ywana w programie
extern "C" {

	void __declspec(dllexport) Grayscale(GrayscaleParams params)
	{
		for (uint32_t x = 0; x < params.AreaWidth; x++)
		{
			for (uint32_t y = 0; y < params.AreaHeight; y++)
			{
				int pixelIndex = params.BytesPerPixel * ((y + params.StartYCoord) * params.FullWidth + x + params.StartXCoord);

				*(params.Output + pixelIndex) = (uint8_t)(*(params.Input + pixelIndex) * 0.299 + *(params.Input + 1 + pixelIndex) * 0.587 + *(params.Input + 2 + pixelIndex) * 0.114);

				*(params.Output + pixelIndex + 1) = (uint8_t)(*(params.Input + pixelIndex) * 0.299 + *(params.Input + 1 + pixelIndex) * 0.587 + *(params.Input + 2 + pixelIndex) * 0.114);
				*(params.Output + pixelIndex + 2) = (uint8_t)(*(params.Input + pixelIndex) * 0.299 + *(params.Input + 1 + pixelIndex) * 0.587 + *(params.Input + 2 + pixelIndex) * 0.114);
				if (params.BytesPerPixel == 4) {
					*(params.Output + pixelIndex + 3) = *(params.Input + pixelIndex + 3);
				}
			}
		}

	}

}