#pragma once

#include <lvgl.h>

namespace UI {

// Kolory zapisujemy jako RGB, np. 0x203448.
struct Style {
    uint32_t background = 0x203448;
    uint32_t foreground = 0xFFFFFF;
    uint32_t border = 0x203448;
    uint32_t pressed = 0x31516B;
    int radius = 14;
    int border_width = 0;
    int padding = 12;
    const lv_font_t *font = &lv_font_montserrat_14;
};

// Uchwyty do elementów, których treść chcesz później aktualizować.
struct Value {
    lv_obj_t *root = nullptr;
    lv_obj_t *title = nullptr;
    lv_obj_t *value = nullptr;
    lv_obj_t *unit = nullptr;
};

struct Button {
    lv_obj_t *root = nullptr;
    lv_obj_t *label = nullptr;
};

using ClickCallback = void (*)(lv_event_t *event);

// Zwraca kontener interfejsu. Do niego dodawaj pozostałe elementy.
lv_obj_t *createScreen(
    lv_obj_t *parent = nullptr,
    uint32_t background = 0x101820
);

// Tworzy prosty tekst. Zwraca obiekt LVGL, żeby można było go później zmieniać.
lv_obj_t *createLabel(
    lv_obj_t *parent,
    const char *text,
    int x, int y,
    uint32_t color = 0xFFFFFF,
    const lv_font_t *font = &lv_font_montserrat_14
);

// Tworzy kartę z tytułem, wartością i opcjonalną jednostką.
// Po utworzeniu można aktualizować ją przez setValue().
Value createValueCard(
    lv_obj_t *parent,
    const char *title,
    const char *initial_value,
    const char *unit,
    int x, int y, int width, int height,
    const Style &style = Style{}
);

void setValue(Value &widget, const char *value);
void setTitle(Value &widget, const char *title);

// Tworzy klikalny przycisk. callback może być nullptr, jeśli przycisk
// ma być tylko wizualny. Kliknięcie wymaga poprawnie zarejestrowanego dotyku.
Button createButton(
    lv_obj_t *parent,
    const char *text,
    int x, int y, int width, int height,
    ClickCallback callback = nullptr,
    void *user_data = nullptr,
    const Style &style = Style{}
);

void setButtonText(Button &button, const char *text);

// Ustawia tło obiektu i opcjonalnie kolor tekstu.
void setBackground(lv_obj_t *object, uint32_t color);
void setTextColor(lv_obj_t *object, uint32_t color);

} // namespace UI
