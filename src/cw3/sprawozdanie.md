# CW3 — Ukryty napis po podaniu hasła w Javie

## Dane do sprawozdania
- Przedmiot: IO
- Ćwiczenie: CW3
- Temat: analiza programu Java z ukrytym hasłem i tekstem
- Członkowie grupy: Bojda Dominika, Jarząb Julia, Ziobro Karol, Smutek Mirosław
- Data wykonania: 12.06.2026

## Cel ćwiczenia
Celem ćwiczenia było przygotowanie programu w Javie, który po podaniu poprawnego hasła wyświetla ukryty napis. Program został zapisany tak, aby hasło oraz tekst nie były łatwe do odczytania bezpośrednio w pliku źródłowym i w skompilowanym pliku klasy. Następnie przeprowadzono uruchomienie programu oraz analizę sposobu działania mechanizmu sprawdzania hasła.

Kod źródłowy programu znajduje się poniżej w treści sprawozdania, a zrzuty ekranu należy umieścić w folderze `resources/`.

## Wymagania z zadania
1. Napisać program w Javie, który po podaniu hasła wypisuje ukryty napis.
2. Skompilować program.
3. Zastosować prostą obfuskację danych, aby utrudnić ich odczytanie.
4. Uruchomić program i sprawdzić działanie poprawnego oraz błędnego hasła.
5. Przeanalizować wynik działania i opisać zastosowaną metodę ukrycia danych.
6. Przygotować sprawozdanie w formacie PDF.

## Plan wykonania krok po kroku
1. Przygotować prosty program źródłowy z hasłem i ukrytym tekstem.
2. Ukryć dane w programie w sposób utrudniający ich odczyt.
3. Skompilować program i uruchomić go, aby potwierdzić poprawne działanie.
4. Wykonać zrzuty ekranu z uruchomienia i z analizy binarnej/bytecode, jeśli są wymagane.
5. Opisać wynik działania oraz wnioski.

## Przebieg realizacji
### 1. Program źródłowy
```java
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {
    private static int byteKey(int index, int seed) {
        return (seed + index * 17) & 0xFF;
    }

    private static void xorDecode(char[] buffer, int seed) {
        for (int index = 0; index < buffer.length; index++) {
            buffer[index] = (char) (buffer[index] ^ byteKey(index, seed));
        }
    }

    public static void main(String[] args) throws IOException {
        final int seed = 0x4D;
        char[] password = { 0x003E, 0x003B, 0x0004, 0x00F2, 0x00F4, 0x00D6 };
        char[] message = { 0x0018, 0x0035, 0x001D, 0x00F9, 0x00E5, 0x00DB, 0x0093, 0x00B0, 0x00B0, 0x008D, 0x0084, 0x007C };

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

        System.out.print("Podaj haslo: ");
        String input = reader.readLine();
        if (input == null) {
            return;
        }

        input = input.strip();
        xorDecode(password, seed);

        if (input.equals(new String(password))) {
            xorDecode(message, seed);
            System.out.println(new String(message));
        } else {
            System.out.println("Bledne haslo");
        }
    }
}
```

Plik źródłowy: [Main.java](Main.java)

### 2. Kompilacja
Program można skompilować poleceniem:

```bash
javac Main.java
```

Uruchomienie programu wykonuje się poleceniem:

```bash
java Main
```

Po podaniu poprawnego hasła program wyświetla ukryty tekst, a po podaniu błędnego hasła zwraca komunikat `Bledne haslo`.

![Uruchomienie programu](resources/working_example.png)

### 3. Analiza danych
W programie hasło i tekst zostały zapisane jako zaszyfrowane wartości znakowe w tablicach `char[]`. Do ich odczytu wykorzystywana jest funkcja `xorDecode`, która stosuje klucz zależny od indeksu. Dzięki temu dane nie występują jako jawny napis w kodzie źródłowym ani w prostym podglądzie pliku klasy.

Przykładowy zrzut binarnej reprezentacji lub analizy bytecode należy umieścić w folderze `resources/`.

![Zaszyfrowane dane](resources/zaszyfrowany_xxd.png)

### 4. Sprawdzenie działania
W trakcie testów program został uruchomiony z poprawnym hasłem oraz z błędnym hasłem. W pierwszym przypadku wyświetlany jest ukryty tekst, a w drugim komunikat o błędnym haśle.

Przykład analizy i wywołania programu można pokazać na zrzutach z `resources/`.

![Analiza w GDB lub podobnym narzędziu](resources/gdb_rsi.png)
![Podgląd deasemblacji lub bytecode](resources/objdump_strcmp.png)
![Wynik po obejściu zabezpieczenia](resources/r2_zlamane%20zabezpieczenia.png)

## Wyniki
- Oryginalny program: po podaniu poprawnego hasła wyświetla ukryty tekst.
- Obfuskacja: dane wejściowe są zapisane jako zakodowane wartości i odszyfrowywane dopiero w czasie działania programu.
- Zmodyfikowany program: można wymusić wyświetlenie ukrytego tekstu po obejściu warunku logicznego.

## Wnioski
Ćwiczenie pokazało, że w Javie również można zastosować prostą obfuskację danych, która utrudnia szybki odczyt hasła i tekstu. Taki mechanizm zwiększa trudność analizy statycznej, ale nie stanowi realnej ochrony przed osobą, która potrafi przeanalizować bytecode lub śledzić działanie programu w debuggerze.

Najważniejszym wnioskiem jest to, że ukrywanie danych w tablicach i stosowanie prostego XOR-a nie zabezpiecza programu, a jedynie podnosi koszt analizy. Jeśli logika sprawdzania hasła jest dostępna w kodzie, można ją odtworzyć, obejść albo wymusić wykonanie właściwej gałęzi programu.

Poprawnym rozwiazaniem będzie tutaj użycie samego hasła jako klucza deszyfrującego.
W takim wypadku jednynym problemem będzie odczytanie wartosci bezpośrenio z pamieci urządzenia np. Sprectre / Meltdown lub poprzez inny proces/użytkownika mającego dostęp do tej pamieci. 


## Załączniki
- Kod źródłowy programu
- Zrzuty ekranu z folderu `resources/`
- Ewentualne notatki z analizy bytecode lub działania programu
