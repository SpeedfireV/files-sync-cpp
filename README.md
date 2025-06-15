# Files Synchronizer


## Założenia projektu

Celem projektu było stworzenie aplikacji do synchronizacji plików pomiędzy różnymi lokalizacjami na dysku. Program ma umożliwiać automatyczne wykrywanie zmian w plikach oraz ich aktualizację w wybranych folderach, zapewniając spójność danych.

## Ogólne działanie programu – przewodnik dla użytkownika

1. Użytkownik uruchamia aplikację i wskazuje foldery, które mają być synchronizowane.
2. Program analizuje zawartość folderów, wykrywa różnice i proponuje działania synchronizacyjne.
3. Po akceptacji przez użytkownika, pliki są kopiowane, nadpisywane lub usuwane zgodnie z ustalonymi regułami.
4. Dane z synchronizacji są automatycznie zapisywane po zamknięciu programu by były dostępne przy następnym uruchomieniu

## Podział programu na klasy i ich działania

- [`FilesSynchronizer`](FilesSynchronizer/FilesSynchronizer.h): Zarządza synchronizacją - stanowi centralny interfejs do wykonywania operacji ma wszystkich innych klasach.
- [`FilesHandler`](FilesHandler/FilesHandler.h): Odpowiada za operacje na plikach i folderach, takie jak odczyt, zapis, kopiowanie oraz porównywanie plików.
- [`SyncJSON`](SyncJSON/SyncJSON.h): Obsługuje zapis i odczyt konfiguracji oraz stanu synchronizacji w formacie JSON, umożliwiając łatwe przechowywanie ustawień i historii.
- [`UpdateData`](UpdateData/UpdateData.h): Przechowuje i aktualizuje informacje o zmianach w plikach, wspierając proces wykrywania różnic i aktualizacji.
- [`FilesSynchronizerApp`](FilesSynchronizer/FilesSynchronizer.h): Pozwala na działanie programu z poziomu konsoli, interpretuje polecenia użytkownika, sprawdza je pod kątem błędów a następnie je wykonuje

## Sczegóły działania - interfejs użytkownika, możliwości, przypadki użycia

- Interfejs użytkownika jest konsolowy – użytkownik podaje ścieżki do folderów oraz potwierdza proponowane działania.
- Program obsługuje przypadki takie jak: dodanie nowego pliku, modyfikacja istniejącego, usunięcie pliku, konflikt wersji.
- Możliwe jest zapisanie konfiguracji synchronizacji do pliku JSON, co pozwala na łatwe ponowne uruchomienie synchronizacji z tymi samymi ustawieniami.
- Obsługiwane są błędy dostępu do plików oraz konflikty synchronizacji, o których użytkownik jest informowany.
- Program idealnie nada się do robienia regularnych kopii zapasowych najważniejszych folderów i plików na komputrerze

## Refleksje

Przed rozpoczęciem projektu rozplanowaliśmy całą strukturę rozrysowując schemat UML, jednak w trakcie realizacji projektu okazało się, że można wprowadzić liczne zmiany usprawniające działanie całej biblioteki.

Projekt zdecydowanie pomógł nam jeszcze bardziej zaznajomić się z obsługą plików oraz de/serializowaniem plików json w C++. 

Bibliotekę stworzyliśmy skupiając się na porównywaniu surowych informacji o datach modyfikacji oraz pliku SyncJson, ale alternatywą do tego podejścia mogłoby być utworzenie biblioteki bardziej bazującej na samych plikach oraz ścieżkach tworząc np. klasy typu Dir, File itp. oraz metody pozwalające np. porównywać te pliki, ich rozmiary, rozszerzenia itp. czego w naszej aplikacji nie ma.