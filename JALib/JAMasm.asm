; ====================================================================================
;	Autor: Patryk Kuban
;   Data utworzenia: 28.11.2021
; 
;	Plik biblioteki DLL. Zawiera funkcj� do konwersji obrazu kolorowego na monochromatyczny napisan� w asemblerze.
;
;	Aktualna wersja: 4
;	
;	Wersja 1:
;		 Przyj�cie parametr�w wej�ciowych
;	
;	Wersja 2:
;		 Implementacja konwersji obrazu kolorowego na monochromatyczny
;
;	Wersja 3:
;		 Poprawki zwi�zane z przetwarzaniem fragmentu obrazu
;
;	Wersja 4:
;		 Odtwarzanie warto�ci rejestr�w po wykonaniu funkcji grayscaleImage
;
;
; ====================================================================================

.data

	_0299		DD 0.299			
	_0587		DD 0.587
	_0114		DD 0.114			; sta�e u�ywane do obliczania warto�ci koloru piksela wed�ug wzoru: R * 0.299 + G * 0.587 + B * 0.114

.code

; ====================================================================================
;
; G��wna funkcja biblioteki napisana w ASM.
; Parametry wej�cia:
;	 rcx - wska�nik na struktur� param. wej�cia / wyj�cia
;
; Parametry wyj�cia:
;	 pole struktury Output, wska�nik na obszar pami�ciu (ju� zaalokowany) z kolorami pikseli
;
; Opis struktury:
;	 [rcx + 0]  - Input, wska�nik na kolory pikseli wej�ciowe
;	 [rcx + 8]  - Output, wska�nik na kolory pikseli wyj�ciowe
;	 [rcx + 16] - StartCoordX, pozycja startowa piksela na osi X (do obs�ugi fragmentu obrazu)
;	 [rcx + 20] - StartCoordY, pozycja startowa piksela na osi Y (do obs�ugi fragmentu obrazu)
;	 [rcx + 24] - AreaWidth, szeroko�� fragmentu obrazu w pikselach
;	 [rcx + 28] - AreaHeight, wysoko�� fragmentu obrazu w pikselach
;	 [rcx + 32] - BytesPerPixel, liczba bajt�w na piksel (3 lub 4)
;	 [rcx + 36] - FullWidth, szeroko�� ca�ego obrazu
;	 [rcx + 40] - FullHeight, wysoko�� ca�ego obrazu (nieu�ywane)
;
; U�ywane rejestry:
; rax, rbx, rcx, rdx, r8 - r12
;
; ====================================================================================

grayscaleImage proc

	push rbp						; Tworzenie ramki stosu
	mov rbp, rsp

	push rax						; Zapis wszystkich u�ytych warto�ci rejestr�w
	push rbx
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12

	xor r9, r9						; Wyzerowanie r9, b�dzie s�u�y� jako pozycja piksela Y w obrazie
	xor r8, r8						; Wyzerowanie r8, b�dzie s�u�y� jako pozycja piksela X w obrazie

	xor r10, r10
	mov r10d, [rcx + 24]			; Wyzerowanie r10 i wpisanie do niego szeroko�ci obrazu w pikselach

	xor r11, r11
	mov r11d, [rcx + 28]			; Wyzerowanie r11 i wpisanie do niego wysoko�ci obrazu w pikselach

for_each_row:
	xor r8, r8
	for_each_col:
		
		; Poni�ej operacje do wyznaczenia indeksu tablicy obrazu
		; Linia w c++: pixelIndex = params.BytesPerPixel * ((y + params.StartYCoord) * params.FullWidth + x + params.StartXCoord);

		mov rax, r9
		add eax, [rcx + 20]		; Wykonanie operacji�: rax = y + params.StartYCoord

		xor rbx, rbx
		mov ebx, [rcx + 36]		; Mno�enie powy�szej warto��i przez params.FullWidth
		mul rbx

		mov rdx, r8
		add edx, [rcx + 16]		; Wykonanie operacji: rdx = x + params.StartXCoord

		add rax, rdx			; Wykonanie operacji: rax = (y + params.StartYCoord) * params.FullWidth + x + params.StartXCoord

		xor rbx, rbx
		mov bl, [rcx + 32]		; Pomno�enie ca�o�ci przez params.BytesPerPixel (pole jest 1 bajtowe)
		mul rbx

		; W rax jest indeks w tablicy kolor�w do danego piksela

		add r8d, [rcx + 16]
		add r9d, [rcx + 20]

		mov r12, rax
		add r12, [rcx]			; Wykonanie operacj� r12 = pixelIndex + params.Input

		; W r12 teraz jest adres pojedynczego piksela

		push rax					; Zachowanie rax (indeks w tablicy Input i Output)
		call CalcPixelValue			; Funkcja do obliczenia warto�ci piksela dla kana�u R, wynik w rax
		pop rbx						; Odzyskanie rax w rbx

		mov rdx, [rcx + 8]
		mov [rdx + rbx], al			; Zapisanie warto�ci koloru R na wyj�cie

		push rbx					; Zachowanie rbx (indeks w tablicy Input i Output)
		call CalcPixelValue			; Funkcja do obliczenia warto�ci piksela dla kana�u G, wynik w rax
		pop rbx						; Odzyskanie rbx

		mov rdx, [rcx + 8]
		mov [rdx + rbx + 1], al		; Zapisanie warto�ci koloru G na wyj�cie 

		push rbx					; Zachowanie rbx (indeks w tablicy Input i Output)
		call CalcPixelValue			; Funkcja do obliczenia warto�ci piksela dla kana�u B, wynik w rax
		pop rbx						; Odzyskanie rbx

		mov rdx, [rcx + 8]
		mov [rdx + rbx + 2], al		; Zapisanie warto�ci koloru B na wyj�cie 

		sub r8d, [rcx + 16]
		sub r9d, [rcx + 20]

		; Sprawdzenie kana�u alpha.
		cmp BYTE PTR [rcx + 32], 3		; Je�li liczba bajt�w na piksel == 4,
		jg copy_alpha					; to trzeba skopiowa� bajt z warto�ci� alpha
		jmp after_copy_alpha
	
	copy_alpha:
		xor rax, rax
		mov al, [rcx + rbx + 3]
		mov [rdx + rbx + 3], al		; Zapisanie bajtu warto�ci alpha na wyj�cie

	after_copy_alpha:
		inc r8							 
		cmp r8, r10					; Sterowanie p�tli for dla ka�dej kolumny
		je after_each_col			; pikseli na obrazie (pozycja piksela X, Y => r8, r9)
		jmp for_each_col

	after_each_col:

	inc r9							 
	cmp r9, r11						; Sterowanie p�tli for dla ka�dego
	je after_each_row				; rz�du pikseli na obrazie
	jmp for_each_row				

after_each_row:
	
	pop r12			; Odzyskiwanie wszystkich u�ytych warto�ci rejestr�w
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

	xor rax, rax		; Zwracanie warto�ci 0
	ret

grayscaleImage endp

; ====================================================================================
; 
; Funkcja pomocnicza do obliczenia warto�ci kana�u wskazanego piksela.
; Parametry wej�ciowe:
;	 r12 - adres piksela z pliku wej�ciowego
;
; Parametry wyj�ciowe:
;	 rax - warto�� piksela (dla konkretnego kana�u RGB)
;
; Niszczone warto�ci rejestr�w:
;	 rax
; 
; ====================================================================================

CalcPixelValue proc

	push r8					; Zapisanie rejestr�w na stosie,
	push r9					; �eby przywr�ci� ich stan po wykonaniu funkcji
	push r10
	
	xor rax, rax				; wyzerowanie rax, �eby p�niej wpisa� wynik oblicze�

	xor r8, r8
	xor r9, r9
	xor r10, r10				; wyzerowanie rejestr�w, b�d� s�u�y�y do przechowania warto�ci RGB piksela na wej�ciu
	
	movq xmm0, r8
	movq xmm1, r9
	movq xmm2, r10				; wyzerowanie rejestr�w, b�d� s�u�y�y do wyliczenia warto�ci RGB piksela na wyj�ciu

	mov r8b, [r12]
	mov r9b, [r12 + 1]
	mov r10b, [r12 + 2]			; odczytanie warto�ci RGB z piksela

	cvtsi2ss xmm0, r8
	mulss xmm0, _0299			; Wykonanie operacji xmm0 = R * 0.299
	
	cvtsi2ss xmm1, r9
	mulss xmm1, _0587			; Wykonanie operacji xmm1 = G * 0.587
	
	cvtsi2ss xmm2, r10
	mulss xmm2, _0114			; Wykonanie operacji xmm2 = B * 0.114

	addss xmm0, xmm1
	addss xmm0, xmm2			; Wykonanie operacji xmm0 = xmm0 + xmm1 + xmm2, czyli xmm0 = R * 0.299 + G * 0.587 + B * 0.114
	
	cvttss2si rax, xmm0			; Zapisanie wyniku jako liczby ca�kowitej do rax

	pop r10
	pop r9				; Przywr�cenie warto�ci rejestr�w
	pop r8				; przed ko�cem funkcji

	ret

CalcPixelValue endp


end		; Koniec pliku ASM