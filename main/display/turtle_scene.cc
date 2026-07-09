#include "turtle_scene.h"

#include <cmath>
#include <cstring>
#include <esp_log.h>

#define TAG "TurtleScene"

TurtleScene::TurtleScene(lv_obj_t* parent, int width, int height)
    : width_(width), height_(height) {
    root_ = lv_obj_create(parent);
    lv_obj_set_size(root_, width_, height_);
    lv_obj_set_pos(root_, 0, 0);
    lv_obj_set_style_radius(root_, 0, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_set_style_pad_all(root_, 0, 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(root_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    CreateOceanBackground();
    CreateBubbles();
    CreateTurtle();
    CreateOverlays();

    anim_timer_ = lv_timer_create(AnimTimerCb, 80, this);
    lv_timer_pause(anim_timer_);
}

TurtleScene::~TurtleScene() {
    if (anim_timer_ != nullptr) {
        lv_timer_delete(anim_timer_);
        anim_timer_ = nullptr;
    }
    if (root_ != nullptr) {
        lv_obj_del(root_);
        root_ = nullptr;
    }
}

void TurtleScene::CreateOceanBackground() {
    surface_layer_ = lv_obj_create(root_);
    lv_obj_set_size(surface_layer_, width_, height_ / 3);
    lv_obj_set_pos(surface_layer_, 0, 0);
    lv_obj_set_style_radius(surface_layer_, 0, 0);
    lv_obj_set_style_border_width(surface_layer_, 0, 0);
    lv_obj_set_style_bg_color(surface_layer_, lv_color_hex(0x0077BE), 0);
    lv_obj_remove_flag(surface_layer_, LV_OBJ_FLAG_SCROLLABLE);

    mid_layer_ = lv_obj_create(root_);
    lv_obj_set_size(mid_layer_, width_, height_ / 3);
    lv_obj_set_pos(mid_layer_, 0, height_ / 3);
    lv_obj_set_style_radius(mid_layer_, 0, 0);
    lv_obj_set_style_border_width(mid_layer_, 0, 0);
    lv_obj_set_style_bg_color(mid_layer_, lv_color_hex(0x005599), 0);
    lv_obj_remove_flag(mid_layer_, LV_OBJ_FLAG_SCROLLABLE);

    deep_layer_ = lv_obj_create(root_);
    lv_obj_set_size(deep_layer_, width_, height_ / 3 + height_ % 3);
    lv_obj_set_pos(deep_layer_, 0, 2 * height_ / 3);
    lv_obj_set_style_radius(deep_layer_, 0, 0);
    lv_obj_set_style_border_width(deep_layer_, 0, 0);
    lv_obj_set_style_bg_color(deep_layer_, lv_color_hex(0x002244), 0);
    lv_obj_remove_flag(deep_layer_, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < 4; i++) {
        lv_obj_t* ray = lv_obj_create(root_);
        lv_obj_set_size(ray, 3, height_ / 2);
        lv_obj_set_pos(ray, 40 + i * 70, 0);
        lv_obj_set_style_radius(ray, 2, 0);
        lv_obj_set_style_border_width(ray, 0, 0);
        lv_obj_set_style_bg_color(ray, lv_color_hex(0x88CCFF), 0);
        lv_obj_set_style_bg_opa(ray, LV_OPA_20, 0);
        lv_obj_remove_flag(ray, LV_OBJ_FLAG_SCROLLABLE);
    }
}

void TurtleScene::CreateTurtle() {
    turtle_ = lv_obj_create(root_);
    lv_obj_set_size(turtle_, 110, 68);
    lv_obj_set_style_bg_opa(turtle_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(turtle_, 0, 0);
    lv_obj_set_style_pad_all(turtle_, 0, 0);
    lv_obj_remove_flag(turtle_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(turtle_, LV_ALIGN_CENTER, 0, 16);

    lv_obj_t* shell = lv_obj_create(turtle_);
    lv_obj_set_size(shell, 76, 50);
    lv_obj_set_pos(shell, 16, 6);
    lv_obj_set_style_radius(shell, 25, 0);
    lv_obj_set_style_border_width(shell, 2, 0);
    lv_obj_set_style_border_color(shell, lv_color_hex(0x2D5016), 0);
    lv_obj_set_style_bg_color(shell, lv_color_hex(0x4A7C23), 0);
    lv_obj_remove_flag(shell, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* pattern = lv_obj_create(shell);
    lv_obj_set_size(pattern, 42, 26);
    lv_obj_center(pattern);
    lv_obj_set_style_radius(pattern, 13, 0);
    lv_obj_set_style_border_width(pattern, 2, 0);
    lv_obj_set_style_border_color(pattern, lv_color_hex(0x6B9E3E), 0);
    lv_obj_set_style_bg_color(pattern, lv_color_hex(0x5A8F30), 0);
    lv_obj_remove_flag(pattern, LV_OBJ_FLAG_SCROLLABLE);

    head_ = lv_obj_create(turtle_);
    lv_obj_set_size(head_, 30, 24);
    lv_obj_set_pos(head_, -6, 18);
    lv_obj_set_style_radius(head_, 12, 0);
    lv_obj_set_style_border_width(head_, 0, 0);
    lv_obj_set_style_bg_color(head_, lv_color_hex(0x6B9E3E), 0);
    lv_obj_remove_flag(head_, LV_OBJ_FLAG_SCROLLABLE);

    eye_left_ = lv_obj_create(head_);
    lv_obj_set_size(eye_left_, 6, 6);
    lv_obj_set_pos(eye_left_, 5, 5);
    lv_obj_set_style_radius(eye_left_, 3, 0);
    lv_obj_set_style_border_width(eye_left_, 0, 0);
    lv_obj_set_style_bg_color(eye_left_, lv_color_hex(0x111111), 0);
    lv_obj_remove_flag(eye_left_, LV_OBJ_FLAG_SCROLLABLE);

    eye_right_ = lv_obj_create(head_);
    lv_obj_set_size(eye_right_, 6, 6);
    lv_obj_set_pos(eye_right_, 16, 5);
    lv_obj_set_style_radius(eye_right_, 3, 0);
    lv_obj_set_style_border_width(eye_right_, 0, 0);
    lv_obj_set_style_bg_color(eye_right_, lv_color_hex(0x111111), 0);
    lv_obj_remove_flag(eye_right_, LV_OBJ_FLAG_SCROLLABLE);

    flipper_left_ = lv_obj_create(turtle_);
    lv_obj_set_size(flipper_left_, 32, 13);
    lv_obj_set_pos(flipper_left_, 10, 48);
    lv_obj_set_style_radius(flipper_left_, 6, 0);
    lv_obj_set_style_border_width(flipper_left_, 0, 0);
    lv_obj_set_style_bg_color(flipper_left_, lv_color_hex(0x5A8F30), 0);
    lv_obj_remove_flag(flipper_left_, LV_OBJ_FLAG_SCROLLABLE);

    flipper_right_ = lv_obj_create(turtle_);
    lv_obj_set_size(flipper_right_, 32, 13);
    lv_obj_set_pos(flipper_right_, 64, 48);
    lv_obj_set_style_radius(flipper_right_, 6, 0);
    lv_obj_set_style_border_width(flipper_right_, 0, 0);
    lv_obj_set_style_bg_color(flipper_right_, lv_color_hex(0x5A8F30), 0);
    lv_obj_remove_flag(flipper_right_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* tail = lv_obj_create(turtle_);
    lv_obj_set_size(tail, 16, 11);
    lv_obj_set_pos(tail, 94, 26);
    lv_obj_set_style_radius(tail, 5, 0);
    lv_obj_set_style_border_width(tail, 0, 0);
    lv_obj_set_style_bg_color(tail, lv_color_hex(0x5A8F30), 0);
    lv_obj_remove_flag(tail, LV_OBJ_FLAG_SCROLLABLE);
}

void TurtleScene::CreateBubbles() {
    for (int i = 0; i < 6; i++) {
        bubbles_[i] = lv_obj_create(root_);
        int size = 6 + (i % 3) * 4;
        lv_obj_set_size(bubbles_[i], size, size);
        bubble_x_[i] = 30 + (i * 47) % (width_ - 40);
        bubble_y_[i] = height_ - 20 - (i * 37) % (height_ / 2);
        lv_obj_set_pos(bubbles_[i], bubble_x_[i], bubble_y_[i]);
        lv_obj_set_style_radius(bubbles_[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(bubbles_[i], 1, 0);
        lv_obj_set_style_border_color(bubbles_[i], lv_color_hex(0xAADDFF), 0);
        lv_obj_set_style_bg_color(bubbles_[i], lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(bubbles_[i], LV_OPA_30, 0);
        lv_obj_remove_flag(bubbles_[i], LV_OBJ_FLAG_SCROLLABLE);
    }
}

void TurtleScene::CreateOverlays() {
    zzz_label_ = lv_label_create(root_);
    lv_label_set_text(zzz_label_, "z z z");
    lv_obj_set_style_text_color(zzz_label_, lv_color_hex(0xAADDFF), 0);
    lv_obj_align(zzz_label_, LV_ALIGN_CENTER, 60, -30);
    lv_obj_add_flag(zzz_label_, LV_OBJ_FLAG_HIDDEN);

    thought_bubble_ = lv_obj_create(root_);
    lv_obj_set_size(thought_bubble_, 36, 24);
    lv_obj_align(thought_bubble_, LV_ALIGN_CENTER, -70, -40);
    lv_obj_set_style_radius(thought_bubble_, 8, 0);
    lv_obj_set_style_border_width(thought_bubble_, 2, 0);
    lv_obj_set_style_border_color(thought_bubble_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_color(thought_bubble_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(thought_bubble_, LV_OPA_70, 0);
    lv_obj_remove_flag(thought_bubble_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(thought_bubble_, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t* dots = lv_label_create(thought_bubble_);
    lv_label_set_text(dots, "...");
    lv_obj_set_style_text_color(dots, lv_color_hex(0x333333), 0);
    lv_obj_center(dots);
}

TurtleMood TurtleScene::EmotionToMood(const char* emotion) {
    if (emotion == nullptr) {
        return TurtleMood::Awake;
    }
    if (strcmp(emotion, "sleepy") == 0 || strcmp(emotion, "sleep") == 0) {
        return TurtleMood::Sleeping;
    }
    if (strcmp(emotion, "relaxed") == 0) {
        return TurtleMood::Relaxed;
    }
    if (strcmp(emotion, "listening") == 0) {
        return TurtleMood::Listening;
    }
    if (strcmp(emotion, "speaking") == 0) {
        return TurtleMood::Speaking;
    }
    if (strcmp(emotion, "happy") == 0 || strcmp(emotion, "laughing") == 0 || strcmp(emotion, "funny") == 0 ||
        strcmp(emotion, "winking") == 0 || strcmp(emotion, "cool") == 0 || strcmp(emotion, "delicious") == 0) {
        return TurtleMood::Happy;
    }
    if (strcmp(emotion, "sad") == 0 || strcmp(emotion, "crying") == 0 || strcmp(emotion, "embarrassed") == 0) {
        return TurtleMood::Sad;
    }
    if (strcmp(emotion, "thinking") == 0 || strcmp(emotion, "confused") == 0 || strcmp(emotion, "idle") == 0) {
        return TurtleMood::Thinking;
    }
    if (strcmp(emotion, "angry") == 0 || strcmp(emotion, "surprised") == 0) {
        return TurtleMood::Angry;
    }
    if (strcmp(emotion, "neutral") == 0 || strcmp(emotion, "microchip_ai") == 0) {
        return TurtleMood::Awake;
    }
    return TurtleMood::Relaxed;
}

void TurtleScene::SetMoodFromEmotion(const char* emotion) {
    if (power_save_) {
        return;
    }
    SetMood(EmotionToMood(emotion));
}

void TurtleScene::SetMood(TurtleMood mood) {
    if (power_save_) {
        mood = TurtleMood::Sleeping;
    }
    mood_ = mood;
    ApplyMoodVisuals();
    ESP_LOGI(TAG, "Mood set to %d", static_cast<int>(mood_));
}

void TurtleScene::SetPowerSave(bool on) {
    power_save_ = on;
    if (on) {
        SetMood(TurtleMood::Sleeping);
    } else {
        SetMood(TurtleMood::Relaxed);
    }
}

void TurtleScene::ApplyMoodVisuals() {
    TurtleMood mood = power_save_ ? TurtleMood::Sleeping : mood_;

    bool sleeping = mood == TurtleMood::Sleeping;
    bool thinking = mood == TurtleMood::Thinking;

    if (eye_left_ != nullptr) {
        if (sleeping) {
            lv_obj_add_flag(eye_left_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_remove_flag(eye_left_, LV_OBJ_FLAG_HIDDEN);
            int eye_size = (mood == TurtleMood::Listening) ? 8 : 6;
            lv_obj_set_size(eye_left_, eye_size, eye_size);
            lv_obj_set_style_radius(eye_left_, eye_size / 2, 0);
        }
    }
    if (eye_right_ != nullptr) {
        if (sleeping) {
            lv_obj_add_flag(eye_right_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_remove_flag(eye_right_, LV_OBJ_FLAG_HIDDEN);
            int eye_size = (mood == TurtleMood::Listening) ? 8 : 6;
            lv_obj_set_size(eye_right_, eye_size, eye_size);
            lv_obj_set_style_radius(eye_right_, eye_size / 2, 0);
        }
    }

    if (zzz_label_ != nullptr) {
        if (sleeping) {
            lv_obj_remove_flag(zzz_label_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(zzz_label_, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (thought_bubble_ != nullptr) {
        if (thinking) {
            lv_obj_remove_flag(thought_bubble_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(thought_bubble_, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (surface_layer_ != nullptr) {
        lv_obj_set_style_bg_color(surface_layer_,
            lv_color_hex(sleeping ? 0x003366 : 0x0077BE), 0);
    }
    if (mid_layer_ != nullptr) {
        lv_obj_set_style_bg_color(mid_layer_,
            lv_color_hex(sleeping ? 0x002244 : 0x005599), 0);
    }
    if (deep_layer_ != nullptr) {
        lv_obj_set_style_bg_color(deep_layer_,
            lv_color_hex(sleeping ? 0x001122 : 0x002244), 0);
    }
}

void TurtleScene::Show() {
    if (visible_ || root_ == nullptr) {
        return;
    }
    visible_ = true;
    anim_tick_ = 0;
    turtle_x_ = 0;
    turtle_y_ = 0;
    turtle_dir_ = 1;
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_background(root_);
    ApplyMoodVisuals();
    if (anim_timer_ != nullptr) {
        lv_timer_resume(anim_timer_);
    }
    ESP_LOGI(TAG, "Turtle scene active");
}

void TurtleScene::UpdateAnimation() {
    if (!visible_ || turtle_ == nullptr) {
        return;
    }

    TurtleMood mood = power_save_ ? TurtleMood::Sleeping : mood_;
    anim_tick_ = (anim_tick_ + 1) % 200;
    float phase = anim_tick_ * 3.14159f / 10.0f;

    int swim_speed = 2;
    int swim_range = 50;
    int bob_amount = 4;
    int flipper_amount = 8;

    switch (mood) {
        case TurtleMood::Sleeping:
            swim_speed = 1;
            swim_range = 20;
            bob_amount = 2;
            flipper_amount = 3;
            break;
        case TurtleMood::Listening:
            swim_speed = 1;
            swim_range = 15;
            bob_amount = 2;
            flipper_amount = 5;
            break;
        case TurtleMood::Speaking:
            swim_speed = 2;
            swim_range = 30;
            bob_amount = 8;
            flipper_amount = 12;
            break;
        case TurtleMood::Happy:
            swim_speed = 3;
            swim_range = 60;
            bob_amount = 10;
            flipper_amount = 14;
            break;
        case TurtleMood::Sad:
            swim_speed = 1;
            swim_range = 25;
            bob_amount = 2;
            flipper_amount = 4;
            break;
        case TurtleMood::Thinking:
            swim_speed = 0;
            swim_range = 0;
            bob_amount = 2;
            flipper_amount = 2;
            break;
        case TurtleMood::Angry:
            swim_speed = 4;
            swim_range = 70;
            bob_amount = 6;
            flipper_amount = 16;
            break;
        default:
            break;
    }

    if (swim_speed > 0) {
        turtle_x_ += turtle_dir_ * swim_speed;
        if (turtle_x_ > swim_range) {
            turtle_dir_ = -1;
        } else if (turtle_x_ < -swim_range) {
            turtle_dir_ = 1;
        }
    }

    float wave = std::sin(phase);
    int flipper_offset = static_cast<int>(wave * flipper_amount);
    int base_y = 16;
    if (mood == TurtleMood::Sad) {
        base_y = 28;
    } else if (mood == TurtleMood::Listening) {
        base_y = 8;
    }
    turtle_y_ = static_cast<int>(std::sin(phase * 0.5f) * bob_amount);

    lv_obj_align(turtle_, LV_ALIGN_CENTER, turtle_x_, base_y + turtle_y_);

    if (flipper_left_ != nullptr) {
        lv_obj_set_y(flipper_left_, 48 + flipper_offset);
    }
    if (flipper_right_ != nullptr) {
        lv_obj_set_y(flipper_right_, 48 - flipper_offset);
    }

    if (zzz_label_ != nullptr && mood == TurtleMood::Sleeping) {
        int zzz_offset = static_cast<int>(std::sin(phase * 0.3f) * 6);
        lv_obj_align(zzz_label_, LV_ALIGN_CENTER, 60 + zzz_offset, -30 - zzz_offset / 2);
    }

    if (thought_bubble_ != nullptr && mood == TurtleMood::Thinking) {
        int bubble_offset = static_cast<int>(std::sin(phase * 0.4f) * 4);
        lv_obj_align(thought_bubble_, LV_ALIGN_CENTER, -70, -40 + bubble_offset);
    }

    int bubble_speed = (mood == TurtleMood::Sleeping) ? 0 : 1;
    for (int i = 0; i < 6; i++) {
        if (bubbles_[i] == nullptr) {
            continue;
        }
        if (bubble_speed > 0) {
            bubble_y_[i] -= bubble_speed + (i % 2);
        }
        if (bubble_y_[i] < 10) {
            bubble_y_[i] = height_ - 10 - (i * 13) % 40;
            bubble_x_[i] = 20 + (i * 53 + anim_tick_ * 7) % (width_ - 40);
        }
        lv_obj_set_pos(bubbles_[i], bubble_x_[i], bubble_y_[i]);
    }
}

void TurtleScene::AnimTimerCb(lv_timer_t* timer) {
    auto* scene = static_cast<TurtleScene*>(lv_timer_get_user_data(timer));
    if (scene != nullptr) {
        scene->UpdateAnimation();
    }
}
