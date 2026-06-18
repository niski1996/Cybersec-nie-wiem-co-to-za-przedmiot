# CW1 — Modyfikacja programu binarnego

## Dane do sprawozdania
- Przedmiot: IO
- Ćwiczenie: CW1
- Temat: analiza i modyfikacja pliku wykonywalnego w hex edytorze
- Grupa: _uzupełnij_
- Członkowie grupy: _uzupełnij_
- Data wykonania: _uzupełnij_

## Cel ćwiczenia
Celem ćwiczenia jest przygotowanie programu w C/C++, który wyświetla napis `Hello World!`, skompilowanie go, analiza otrzymanego pliku wykonywalnego oraz ręczna zmiana zawartości binarnej tak, aby zmodyfikowany program wyświetlał napis `Hello User`.

Kod źródłowy programu znajduje się poniżej w treści sprawozdania, a zrzuty ekranu należy umieścić w folderze `resources/`.

## Wymagania z zadania
1. Napisać program w języku C lub C++, który wypisuje `Hello World!` na standardowe wyjście.
2. Skompilować program.
3. Przeanalizować plik wykonywalny za pomocą hex edytora.
4. Zmienić skompilowany program w formie binarnej tak, aby wypisywał `Hello User`.
5. Przygotować sprawozdanie i przekazać je w formacie PDF.
6. Sprawozdanie musi być podpisane przez wszystkich członków grupy.

## Plan wykonania krok po kroku
1. Przygotować prosty program źródłowy z komunikatem `Hello World!`.
2. Skompilować program i uruchomić go, aby potwierdzić poprawne działanie.
3. Otworzyć plik wykonywalny w hex edytorze.
4. Zlokalizować ciąg znaków `Hello World!` w binarce.
5. Zmienić tekst na `Hello User`, zachowując zgodność długości lub układu danych, jeśli będzie to konieczne.
6. Zapisać zmodyfikowany plik i ponownie go uruchomić.
7. Porównać zachowanie programu przed i po modyfikacji.
8. Opisać cały przebieg w sprawozdaniu.

## Przebieg realizacji
### 1. Program źródłowy
```c
#include <stdio.h>

int main(void) {
	puts("Hello World!");
	return 0;
}
```

Plik źródłowy: [hello.c](hello.c)

### 2. Kompilacja
Program skompilowano poleceniem:

```bash
gcc hello.c -o hello
```

Następnie uruchomiono plik wykonywalny i potwierdzono poprawne działanie programu. Wynik działania był następujący:

```text
Hello World!
```

Zrzut ekranu do wykonania: `resources/01-kompilacja.png`.

### 3. Analiza binarki
Do analizy pliku wykonywalnego użyto narzędzia hex/byte view. W binarce zlokalizowano napis `Hello World!` na offsecie `0x2004`.

Podgląd fragmentu pliku przed modyfikacją:

```text
00002000: 01 00 02 00 48 65 6c 6c 6f 20 57 6f 72 6c 64 21  ....Hello World!
```

Zrzut ekranu do wykonania: `resources/02-hex-przed.png`.

### 4. Modyfikacja binarna
Przygotowano kopię pliku wykonywalnego i zmieniono ciąg znaków `Hello World!` na `Hello User`, dopisując dwa bajty zerowe, aby zachować spójny układ danych w pliku.

Po modyfikacji fragment binarki wyglądał następująco:

```text
00002000: 01 00 02 00 48 65 6c 6c 6f 20 55 73 65 72 00 00  ....Hello User..
```

Po uruchomieniu zmodyfikowanego programu otrzymano wynik:

```text
Hello User
```

Zrzuty ekranu do wykonania:
- `resources/03-hex-po.png`
- `resources/04-uruchomienie-po.png`

## Wyniki
- Oryginalny program: `Hello World!`
- Zmodyfikowany program: `Hello User`

## Wnioski
Analiza pokazała, że prosty komunikat tekstowy w programie może zostać odnaleziony i zmieniony bez rekompilacji, bezpośrednio w pliku wykonywalnym. W praktyce oznacza to, że nawet niewielka binarna modyfikacja może zmienić zachowanie programu, o ile tekst i układ danych pozostają spójne.

## Załączniki
- Kod źródłowy programu
- Zrzuty ekranu z folderu `resources/`
- Ewentualne porównanie plików przed i po modyfikacji
