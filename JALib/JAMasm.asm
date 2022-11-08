; ====================================================================================
;	Autor: Patryk Kuban
;   Data utworzenia: 28.11.2021
; 
;	Plik biblioteki DLL. Zawiera funkcjê do konwersji obrazu kolorowego na monochromatyczny napisan¹ w asemblerze.
;
;	Aktualna wersja: 4
;	
;	Wersja 1:
;		 Przyjêcie parametrów wejœciowych
;	
;	Wersja 2:
;		 Implementacja konwersji obrazu kolorowego na monochromatyczny
;
;	Wersja 3:
;		 Poprawki zwi¹zane z przetwarzaniem fragmentu obrazu
;
;	Wersja 4:
;		 Odtwarzanie wartoœci rejestrów po wykonaniu funkcji grayscaleImage
;
;
; ====================================================================================

.data

	_0299		DD 0.299			
	_0587		DD 0.587
	_0114		DD 0.114			; sta³e u¿ywane do obliczania wartoœci koloru piksela wed³ug wzoru: R * 0.299 + G * 0.587 + B * 0.114

.code

; ====================================================================================
;
; G³ówna funkcja biblioteki napisana w ASM.
; Parametry wejœcia:
;	 rcx - wskaŸnik na strukturê param. wejœcia / wyjœcia
;
; Parametry wyjœcia:
;	 pole struktury Output, wskaŸnik na obszar pamiêciu (ju¿ zaalokowany) z kolorami pikseli
;
; Opis struktury:
;	 [rcx + 0]  - Input, wskaŸnik na kolory pikseli wejœciowe
;	 [rcx + 8]  - Output, wska¿nik na kolory pikseli wyjœciowe
;	 [rcx + 16] - StartCoordX, pozycja startowa piksela na osi X (do obs³ugi fragmentu obrazu)
;	 [rcx + 20] - StartCoordY, pozycja startowa piksela na osi Y (do obs³ugi fragmentu obrazu)
;	 [rcx + 24] - AreaWidth, szerokoœæ fragmentu obrazu w pikselach
;	 [rcx + 28] - AreaHeight, wysokoœæ fragmentu obrazu w pikselach
;	 [rcx + 32] - BytesPerPixel, liczba bajtów na piksel (3 lub 4)
;	 [rcx + 36] - FullWidth, szerokoœæ ca³ego obrazu
;	 [rcx + 40] - FullHeight, wysokoœæ ca³ego obrazu (nieu¿ywane)
;
; U¿ywane rejestry:
; rax, rbx, rcx, rdx, r8 - r12
;
; ====================================================================================

grayscaleImage proc

	push rbp						; Tworzenie ramki stosu
	mov rbp, rsp

	push rax						; Zapis wszystkich u¿ytych wartoœci rejestrów
	push rbx
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12

	xor r9, r9						; Wyzerowanie r9, bêdzie s³u¿y³ jako pozycja piksela Y w obrazie
	xor r8, r8						; Wyzerowanie r8, bêdzie s³u¿y³ jako pozycja piksela X w obrazie

	xor r10, r10
	mov r10d, [rcx + 24]			; Wyzerowanie r10 i wpisanie do niego szerokoœci obrazu w pikselach

	xor r11, r11
	mov r11d, [rcx + 28]			; Wyzerowanie r11 i wpisanie do niego wysokoœci obrazu w pikselach

for_each_row:
	xor r8, r8
	for_each_col:
		
		; Poni¿ej operacje do wyznaczenia indeksu tablicy obrazu
		; Linia w c++: pixelIndex = params.BytesPerPixel * ((y + params.StartYCoord) * params.FullWidth + x + params.StartXCoord);

		mov rax, r9
		add eax, [rcx + 20]		; Wykonanie operacjiê: rax = y + params.StartYCoord

		xor rbx, rbx
		mov ebx, [rcx + 36]		; Mno¿enie powy¿szej wartoœæi przez params.FullWidth
		mul rbx

		mov rdx, r8
		add edx, [rcx + 16]		; Wykonanie operacji: rdx = x + params.StartXCoord

		add rax, rdx			; Wykonanie operacji: rax = (y + params.StartYCoord) * params.FullWidth + x + params.StartXCoord

		xor rbx, rbx
		mov bl, [rcx + 32]		; Pomno¿enie ca³oœci przez params.BytesPerPixel (pole jest 1 bajtowe)
		mul rbx

		; W rax jest indeks w tablicy kolorów do danego piksela

		add r8d, [rcx + 16]
		add r9d, [rcx + 20]

		mov r12, rax
		add r12, [rcx]			; Wykonanie operacjê r12 = pixelIndex + params.Input

		; W r12 teraz jest adres pojedynczego piksela

		push rax					; Zachowanie rax (indeks w tablicy Input i Output)
		call CalcPixelValue			; Funkcja do obliczenia wartoœci piksela dla kana³u R, wynik w rax
		pop rbx						; Odzyskanie rax w rbx

		mov rdx, [rcx + 8]
		mov [rdx + rbx], al			; Zapisanie wartoœci koloru R na wyjœcie

		push rbx					; Zachowanie rbx (indeks w tablicy Input i Output)
		call CalcPixelValue			; Funkcja do obliczenia wartoœci piksela dla kana³u G, wynik w rax
		pop rbx						; Odzyskanie rbx

		mov rdx, [rcx + 8]
		mov [rdx + rbx + 1], al		; Zapisanie wartoœci koloru G na wyjœcie 

		push rbx					; Zachowanie rbx (indeks w tablicy Input i Output)
		call CalcPixelValue			; Funkcja do obliczenia wartoœci piksela dla kana³u B, wynik w rax
		pop rbx						; Odzyskanie rbx

		mov rdx, [rcx + 8]
		mov [rdx + rbx + 2], al		; Zapisanie wartoœci koloru B na wyjœcie 

		sub r8d, [rcx + 16]
		sub r9d, [rcx + 20]

		; Sprawdzenie kana³u alpha.
		cmp BYTE PTR [rcx + 32], 3		; Jeœli liczba bajtów na piksel == 4,
		jg copy_alpha					; to trzeba skopiowaæ bajt z wartoœci¹ alpha
		jmp after_copy_alpha
	
	copy_alpha:
		xor rax, rax
		mov al, [rcx + rbx + 3]
		mov [rdx + rbx + 3], al		; Zapisanie bajtu wartoœci alpha na wyjœcie

	after_copy_alpha:
		inc r8							 
		cmp r8, r10					; Sterowanie pêtli for dla ka¿dej kolumny
		je after_each_col			; pikseli na obrazie (pozycja piksela X, Y => r8, r9)
		jmp for_each_col

	after_each_col:

	inc r9							 
	cmp r9, r11						; Sterowanie pêtli for dla ka¿dego
	je after_each_row				; rzêdu pikseli na obrazie
	jmp for_each_row				

after_each_row:
	
	pop r12			; Odzyskiwanie wszystkich u¿ytych wartoœci rejestrów
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rbx
	pop rax

	mov rsp, rbp		; Niszczenie ramki stosu
	pop rbp

	xor rax, rax		; Zwracanie wartoœci 0
	ret

grayscaleImage endp

; ====================================================================================
; 
; Funkcja pomocnicza do obliczenia wartoœci kana³u wskazanego piksela.
; Parametry wejœciowe:
;	 r12 - adres piksela z pliku wejœciowego
;
; Parametry wyjœciowe:
;	 rax - wartoœæ piksela (dla konkretnego kana³u RGB)
;
; Niszczone wartoœci rejestrów:
;	 rax
; 
; ====================================================================================

CalcPixelValue proc

	push r8					; Zapisanie rejestrów na stosie,
	push r9					; ¿eby przywróciæ ich stan po wykonaniu funkcji
	push r10
	
	xor rax, rax				; wyzerowanie rax, ¿eby póŸniej wpisaæ wynik obliczeñ

	xor r8, r8
	xor r9, r9
	xor r10, r10				; wyzerowanie rejestrów, bêd¹ s³u¿y³y do przechowania wartoœci RGB piksela na wejœciu
	
	movq xmm0, r8
	movq xmm1, r9
	movq xmm2, r10				; wyzerowanie rejestrów, bêd¹ s³u¿y³y do wyliczenia wartoœci RGB piksela na wyjœciu

	mov r8b, [r12]
	mov r9b, [r12 + 1]
	mov r10b, [r12 + 2]			; odczytanie wartoœci RGB z piksela

	cvtsi2ss xmm0, r8
	mulss xmm0, _0299			; Wykonanie operacji xmm0 = R * 0.299
	
	cvtsi2ss xmm1, r9
	mulss xmm1, _0587			; Wykonanie operacji xmm1 = G * 0.587
	
	cvtsi2ss xmm2, r10
	mulss xmm2, _0114			; Wykonanie operacji xmm2 = B * 0.114

	addss xmm0, xmm1
	addss xmm0, xmm2			; Wykonanie operacji xmm0 = xmm0 + xmm1 + xmm2, czyli xmm0 = R * 0.299 + G * 0.587 + B * 0.114
	
	cvttss2si rax, xmm0			; Zapisanie wyniku jako liczby ca³kowitej do rax

	pop r10
	pop r9				; Przywrócenie wartoœci rejestrów
	pop r8				; przed koñcem funkcji

	ret

CalcPixelValue endp


end		; Koniec pliku ASM