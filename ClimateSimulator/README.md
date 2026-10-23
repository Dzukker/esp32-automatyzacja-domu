# ClimateSimulator — M5Stack Fire

## Zachowanie interfejsu
- Po uruchomieniu ekran monitorowania pokazuje temperaturę, wilgotność, ciśnienie oraz szacowaną moc chłodzenia względem domyślnej temperatury 22°C.
- Naciśnięcie dowolnego przycisku otwiera konfigurację temperatury. Pierwsze naciśnięcie służy tylko do wejścia do menu.
- Menu temperatury: A zwiększa temperaturę o 0,5°C, B zmniejsza o 0,5°C, C zatwierdza.
- Menu czasu: A dodaje 10 minut, B odejmuje 10 minut (minimum 10 minut), C uruchamia sesję.
- W trakcie sesji wyświetlany jest czas pozostały, temperatura zmierzona, temperatura zadana i moc symulowanego chłodzenia.
- W trakcie sesji naciśnięcie C otwiera komunikat `Do you want to stop custom configuration?`. A wybiera YES, B wybiera NO, C potwierdza wybór. YES resetuje konfigurację i zatrzymuje sesję; NO wraca do aktywnej sesji.
- Po zakończeniu odliczania chłodzenie symulacyjne zostaje wyłączone. Dowolny przycisk wraca do ekranu monitorowania.

## Sprzęt i biblioteki
Zakłada się M5Stack Fire i czujnik ENV III podłączony do PORT A (I²C). W Arduino IDE zainstaluj M5Unified, M5Unit-ENV i Adafruit NeoPixel (dla pierścienia LED). Zweryfikuj pin danych pierścienia LED w `Config.h`.

## Migotanie ekranu
Ekran nie jest czyszczony przy każdym odświeżeniu. Pełne czyszczenie następuje wyłącznie przy zmianie trybu interfejsu, a wartości są odświeżane w swoich miejscach. Jeśli występuje jeszcze migotanie, może wymagać dalszego dopasowania do wersji biblioteki M5Unified i konkretnego wyświetlacza.

## Ważne
Moc chłodzenia jest symulowana programowo. Kod nie steruje wiatrakiem ani fizycznym urządzeniem chłodzącym.
