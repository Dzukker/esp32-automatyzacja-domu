
#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <esp_err.h>
#include <lvgl.h>
#include "esp_lv_adapter_arduino.h"
#include <assert.h>

#include "ui_components.h"

UI::Value temperatureCard;
UI::Value humidityCard;
UI::Button refreshButton;
float demoTemperature = 23.5f;

using namespace esp_panel::drivers;
using namespace esp_panel::board;

Board *board = nullptr;
lv_display_t *disp = nullptr;

// Elementy interfejsu
lv_obj_t *screen = nullptr;
lv_obj_t *temp_label = nullptr;
lv_obj_t *humidity_label = nullptr;
lv_obj_t *status_label = nullptr;

// Przykładowe dane — później podłączymy czujnik
float temperature = 23.5;
float humidity = 48.0;

// Deklaracje funkcji
void updateSensorValues();
static void sensor_timer_cb(lv_timer_t *timer);
void createSensorPanel();

void setup()
{
    Serial.begin(115200);
    Serial.println("Start panelu czujnikow");

    // 1. Inicjalizacja plytki
    board = new Board();

    if (board == nullptr || !board->init()) {
        Serial.println("Blad inicjalizacji plytki");
        while (true) delay(1000);
    }

    // 2. Pobranie LCD
    LCD *lcd = board->getLCD();

    if (lcd == nullptr) {
        Serial.println("Nie znaleziono LCD");
        while (true) delay(1000);
    }

    // 3. Konfiguracja buforow RGB
    const esp_lv_adapter_rotation_t rotation =
        ESP_LV_ADAPTER_ROTATE_0;

    const esp_lv_adapter_tear_avoid_mode_t tear_mode =
        ESP_LV_ADAPTER_TEAR_AVOID_MODE_DEFAULT_RGB;

    const uint8_t framebuffer_count =
        esp_lv_adapter_get_required_frame_buffer_count(
            tear_mode, rotation
        );

    auto *lcd_bus = lcd->getBus();

    if (lcd_bus->getBasicAttributes().type ==
        ESP_PANEL_BUS_TYPE_RGB) {
        lcd->configFrameBufferNumber(framebuffer_count);

        static_cast<BusRGB *>(lcd_bus)
            ->configRGB_BounceBufferSize(
                lcd->getFrameWidth() * 10
            );
    }

    assert(board->begin());

    // 4. Inicjalizacja adaptera LVGL
    esp_lv_adapter_config_t adapter_config =
        ESP_LV_ADAPTER_DEFAULT_CONFIG();

    adapter_config.task_stack_size = 12 * 1024;
    adapter_config.task_priority = 2;
    adapter_config.task_core_id = ARDUINO_RUNNING_CORE;

    ESP_ERROR_CHECK(esp_lv_adapter_init(&adapter_config));

    // 5. Rejestracja ekranu
    esp_lv_adapter_display_config_t display_config =
        ESP_LV_ADAPTER_DISPLAY_RGB_DEFAULT_CONFIG(
            lcd,
            lcd->getFrameWidth(),
            lcd->getFrameHeight(),
            rotation
        );

    display_config.profile.use_psram = true;

    disp = esp_lv_adapter_register_display(&display_config);
    assert(disp != nullptr);

    // 6. Rejestracja dotyku
    if (board->getTouch() != nullptr) {
        esp_lv_adapter_touch_config_t touch_config =
            ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(
                disp, board->getTouch()
            );

        lv_indev_t *touch =
            esp_lv_adapter_register_touch(&touch_config);

        assert(touch != nullptr);
    }

    // 7. Start zadania LVGL
    ESP_ERROR_CHECK(esp_lv_adapter_start());

    // 8. Tworzenie interfejsu
    ESP_ERROR_CHECK(esp_lv_adapter_lock(-1));

    createSensorPanel();

    // Pierwsza aktualizacja
    updateSensorValues();

    // Aktualizacja co 2 sekundy
    lv_timer_create(sensor_timer_cb, 2000, nullptr);

    esp_lv_adapter_unlock();

    Serial.println("Panel czujnikow uruchomiony");
}

void loop()
{
    // Adapter producenta obsluguje LVGL.
    delay(1000);
}

// Tworzenie elementow interfejsu
void createSensorPanel()
{
    screen = lv_screen_active();

    lv_obj_set_style_bg_color(
        screen, lv_color_hex(0x101820), LV_PART_MAIN
    );
    lv_obj_set_style_bg_opa(
        screen, LV_OPA_COVER, LV_PART_MAIN
    );

    // Tytul
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "PANEL CZUJNIKOW");
    lv_obj_set_style_text_color(
        title, lv_color_hex(0xFFFFFF), LV_PART_MAIN
    );
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 35);

    // Karta temperatury
    lv_obj_t *temp_card = lv_obj_create(screen);
    lv_obj_set_size(temp_card, 330, 180);
    lv_obj_align(temp_card, LV_ALIGN_CENTER, -175, -20);
    lv_obj_set_style_bg_color(
        temp_card, lv_color_hex(0x203448), LV_PART_MAIN
    );
    lv_obj_set_style_border_width(
        temp_card, 0, LV_PART_MAIN
    );
    lv_obj_set_style_radius(
        temp_card, 18, LV_PART_MAIN
    );

    lv_obj_t *temp_title = lv_label_create(temp_card);
    lv_label_set_text(temp_title, "TEMPERATURA");
    lv_obj_set_style_text_color(
        temp_title, lv_color_hex(0xB8DFFF), LV_PART_MAIN
    );
    lv_obj_align(temp_title, LV_ALIGN_TOP_MID, 0, 15);

    temp_label = lv_label_create(temp_card);
    lv_obj_set_style_text_color(
        temp_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN
    );
    lv_obj_set_style_text_font(
        temp_label, &lv_font_montserrat_30, LV_PART_MAIN
    );
    lv_obj_align(temp_label, LV_ALIGN_CENTER, 0, 20);

    // Karta wilgotnosci
    lv_obj_t *humidity_card = lv_obj_create(screen);
    lv_obj_set_size(humidity_card, 330, 180);
    lv_obj_align(humidity_card, LV_ALIGN_CENTER, 175, -20);
    lv_obj_set_style_bg_color(
        humidity_card, lv_color_hex(0x203448), LV_PART_MAIN
    );
    lv_obj_set_style_border_width(
        humidity_card, 0, LV_PART_MAIN
    );
    lv_obj_set_style_radius(
        humidity_card, 18, LV_PART_MAIN
    );

    lv_obj_t *humidity_title = lv_label_create(humidity_card);
    lv_label_set_text(humidity_title, "WILGOTNOSC");
    lv_obj_set_style_text_color(
        humidity_title, lv_color_hex(0x8FFFE0), LV_PART_MAIN
    );
    lv_obj_align(humidity_title, LV_ALIGN_TOP_MID, 0, 15);

    humidity_label = lv_label_create(humidity_card);
    lv_obj_set_style_text_color(
        humidity_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN
    );
    lv_obj_set_style_text_font(
        humidity_label, &lv_font_montserrat_30, LV_PART_MAIN
    );
    lv_obj_align(humidity_label, LV_ALIGN_CENTER, 0, 20);

    // Status danych
    status_label = lv_label_create(screen);
    lv_obj_set_style_text_color(
        status_label, lv_color_hex(0xD0D0D0), LV_PART_MAIN
    );
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -35);
}

// Aktualizacja wartosci — tutaj pozniej odczytamy prawdziwy czujnik
void updateSensorValues()
{
    // Przykladowe dane testowe
    temperature += 0.3;
    if (temperature > 26.0) temperature = 22.0;

    humidity += 1.0;
    if (humidity > 60.0) humidity = 40.0;

    // Wyswietlenie wartosci
    char temp_text[32];
    char humidity_text[32];

    snprintf(temp_text, sizeof(temp_text),
             "%.1f °C", temperature);

    snprintf(humidity_text, sizeof(humidity_text),
             "%.1f %%", humidity);

    lv_label_set_text(temp_label, temp_text);
    lv_label_set_text(humidity_label, humidity_text);
    lv_label_set_text(
        status_label, "TRYB TESTOWY - dane przykladowe"
    );

    Serial.printf(
        "Temperatura: %.1f C, wilgotnosc: %.1f %%\n",
        temperature, humidity
    );
}

// Callback timera LVGL
static void sensor_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    updateSensorValues();
}