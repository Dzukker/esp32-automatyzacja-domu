#include "ui_components.h"

namespace UI {

static void applyBoxStyle(lv_obj_t *object, const Style &style)
{
    lv_obj_set_style_bg_color(object, lv_color_hex(style.background), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(object, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(object, lv_color_hex(style.border), LV_PART_MAIN);
    lv_obj_set_style_border_width(object, style.border_width, LV_PART_MAIN);
    lv_obj_set_style_radius(object, style.radius, LV_PART_MAIN);
    lv_obj_set_style_pad_all(object, style.padding, LV_PART_MAIN);
}

lv_obj_t *createScreen(lv_obj_t *parent, uint32_t background)
{
    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(root, 0, 0);
    lv_obj_set_style_bg_color(root, lv_color_hex(background), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(root, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(root, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    return root;
}

lv_obj_t *createLabel(
    lv_obj_t *parent, const char *text, int x, int y,
    uint32_t color, const lv_font_t *font)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text ? text : "");
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
    lv_obj_set_pos(label, x, y);
    return label;
}

Value createValueCard(
    lv_obj_t *parent,
    const char *title,
    const char *initial_value,
    const char *unit,
    int x, int y, int width, int height,
    const Style &style)
{
    Value widget;

    widget.root = lv_obj_create(parent);
    lv_obj_set_pos(widget.root, x, y);
    lv_obj_set_size(widget.root, width, height);
    applyBoxStyle(widget.root, style);
    lv_obj_clear_flag(widget.root, LV_OBJ_FLAG_SCROLLABLE);

    widget.title = lv_label_create(widget.root);
    lv_label_set_text(widget.title, title ? title : "");
    lv_obj_set_style_text_color(widget.title, lv_color_hex(style.foreground), LV_PART_MAIN);
    lv_obj_set_style_text_font(widget.title, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(widget.title, LV_ALIGN_TOP_LEFT, 0, 0);

    widget.value = lv_label_create(widget.root);
    lv_label_set_text(widget.value, initial_value ? initial_value : "--");
    lv_obj_set_style_text_color(widget.value, lv_color_hex(style.foreground), LV_PART_MAIN);
    lv_obj_set_style_text_font(widget.value, style.font ? style.font : &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(widget.value, LV_ALIGN_CENTER, 0, 5);

    widget.unit = nullptr;
    if (unit && unit[0] != '\0') {
        widget.unit = lv_label_create(widget.root);
        lv_label_set_text(widget.unit, unit);
        lv_obj_set_style_text_color(widget.unit, lv_color_hex(style.foreground), LV_PART_MAIN);
        lv_obj_set_style_text_font(widget.unit, &lv_font_montserrat_14, LV_PART_MAIN);
        lv_obj_align(widget.unit, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    }

    return widget;
}

void setValue(Value &widget, const char *value)
{
    if (widget.value) lv_label_set_text(widget.value, value ? value : "");
}

void setTitle(Value &widget, const char *title)
{
    if (widget.title) lv_label_set_text(widget.title, title ? title : "");
}

static void buttonEventThunk(lv_event_t *event)
{
    // Callback użytkownika jest przechowywany jako user_data.
    auto callback = reinterpret_cast<ClickCallback>(lv_event_get_user_data(event));
    if (callback) callback(event);
}

Button createButton(
    lv_obj_t *parent, const char *text,
    int x, int y, int width, int height,
    ClickCallback callback, void *user_data,
    const Style &style)
{
    Button button;
    button.root = lv_btn_create(parent);
    lv_obj_set_pos(button.root, x, y);
    lv_obj_set_size(button.root, width, height);
    applyBoxStyle(button.root, style);
    lv_obj_set_style_bg_color(button.root, lv_color_hex(style.pressed), LV_STATE_PRESSED);
    lv_obj_clear_flag(button.root, LV_OBJ_FLAG_SCROLLABLE);

    button.label = lv_label_create(button.root);
    lv_label_set_text(button.label, text ? text : "");
    lv_obj_set_style_text_color(button.label, lv_color_hex(style.foreground), LV_PART_MAIN);
    lv_obj_set_style_text_font(button.label, style.font ? style.font : &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_center(button.label);

    if (callback) {
        // Przechowujemy callback bezpośrednio w user_data zdarzenia.
        lv_obj_add_event_cb(button.root, buttonEventThunk, LV_EVENT_CLICKED,
                            reinterpret_cast<void *>(callback));
    }

    // user_data aplikacji można pobrać z obiektu przyciskiem:
    // lv_obj_set_user_data(button.root, user_data) nie jest używane,
    // bo dostępność tej funkcji zależy od konfiguracji LVGL.
    (void)user_data;
    return button;
}

void setButtonText(Button &button, const char *text)
{
    if (button.label) lv_label_set_text(button.label, text ? text : "");
}

void setBackground(lv_obj_t *object, uint32_t color)
{
    if (!object) return;
    lv_obj_set_style_bg_color(object, lv_color_hex(color), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(object, LV_OPA_COVER, LV_PART_MAIN);
}

void setTextColor(lv_obj_t *object, uint32_t color)
{
    if (!object) return;
    lv_obj_set_style_text_color(object, lv_color_hex(color), LV_PART_MAIN);
}

} // namespace UI
