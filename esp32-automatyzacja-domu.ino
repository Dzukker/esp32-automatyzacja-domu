
#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <esp_err.h>
#include <lvgl.h>
#include "esp_lv_adapter_arduino.h"

#include <assert.h>

using namespace esp_panel::drivers;
using namespace esp_panel::board;

Board *board = nullptr;
lv_display_t *disp = nullptr;
lv_obj_t *screen = nullptr;

static uint8_t colorIndex = 0;

static const uint32_t colors[] = {
    0xFF0000,  // czerwony
    0x00FF00,  // zielony
    0x0000FF,  // niebieski
    0xFFFFFF,  // biały
    0x000000,  // czarny
    0xFFFF00,  // żółty
    0x00FFFF,  // cyjan
    0xFF00FF   // magenta
};

static const uint8_t COLOR_COUNT =
    sizeof(colors) / sizeof(colors[0]);

// Timer LVGL: zmiana koloru co sekundę
static void color_timer_cb(lv_timer_t *timer)
{
    (void)timer;

    lv_obj_set_style_bg_color(
        screen,
        lv_color_hex(colors[colorIndex]),
        LV_PART_MAIN
    );

    colorIndex = (colorIndex + 1) % COLOR_COUNT;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Start testu LCD + LVGL 9.5.0");

    // 1. Inicjalizacja płytki
    board = new Board();

    if (board == nullptr || !board->init()) {
        Serial.println("Blad inicjalizacji plytki");
        while (true) delay(1000);
    }

    // 2. Pobranie sterownika LCD
    LCD *lcd = board->getLCD();

    if (lcd == nullptr) {
        Serial.println("Nie znaleziono LCD");
        while (true) delay(1000);
    }

    // 3. Konfiguracja buforów RGB i ograniczenia tearingu
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

    // 4. Uruchomienie urządzeń płytki
    assert(board->begin());

    // 5. Inicjalizacja adaptera LVGL
    esp_lv_adapter_config_t adapter_config =
        ESP_LV_ADAPTER_DEFAULT_CONFIG();

    adapter_config.task_stack_size = 12 * 1024;
    adapter_config.task_priority = 2;
    adapter_config.task_core_id = ARDUINO_RUNNING_CORE;

    ESP_ERROR_CHECK(esp_lv_adapter_init(&adapter_config));

    // 6. Rejestracja LCD w LVGL
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

    // 7. Rejestracja dotyku
    if (board->getTouch() != nullptr) {
        esp_lv_adapter_touch_config_t touch_config =
            ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(
                disp, board->getTouch()
            );

        lv_indev_t *touch =
            esp_lv_adapter_register_touch(&touch_config);

        assert(touch != nullptr);
    }

    // 8. Uruchomienie zadania LVGL
    ESP_ERROR_CHECK(esp_lv_adapter_start());

    // 9. Utworzenie ekranu i timera
    ESP_ERROR_CHECK(esp_lv_adapter_lock(-1));


screen = lv_screen_active();

lv_obj_set_style_bg_opa(
    screen, LV_OPA_COVER, LV_PART_MAIN
);

lv_obj_set_style_bg_color(
    screen, lv_color_hex(colors[0]), LV_PART_MAIN
);

// Utwórz napis
lv_obj_t *label = lv_label_create(screen);

lv_label_set_text(label, "elo zelo");

lv_obj_set_style_text_color(
    label, lv_color_hex(0xFFFFFF), LV_PART_MAIN
);

lv_obj_set_style_text_font(
    label, &lv_font_montserrat_14, LV_PART_MAIN
);

// Wyśrodkuj napis
lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

// Zachowaj miganie kolorów
lv_timer_create(color_timer_cb, 1000, nullptr);

    esp_lv_adapter_unlock();

    Serial.println("Test kolorow uruchomiony");
}

void loop()
{
    // Adapter producenta sam obsługuje zadanie LVGL.
    delay(1000);
}