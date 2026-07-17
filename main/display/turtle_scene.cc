#include "turtle_scene.h"

#include <cmath>
#include <cstring>
#include <esp_log.h>

#define TAG "TurtleScene"

// Franklin-inspired palette
static constexpr uint32_t kSkin = 0x6FAE45;       // soft cartoon green
static constexpr uint32_t kSkinDark = 0x4F8A2E;
static constexpr uint32_t kShellYellow = 0xE8C547;
static constexpr uint32_t kShellBrown = 0x8B5A2B;
static constexpr uint32_t kShellRim = 0x3D6B1F;
static constexpr uint32_t kEyeWhite = 0xFFF8E7;
static constexpr uint32_t kEyeBrown = 0x5C3317;
static constexpr uint32_t kPupil = 0x1A1008;
static constexpr uint32_t kMouth = 0x3A1F14;
static constexpr uint32_t kNeckerchief = 0xD62828;
static constexpr uint32_t kCheek = 0xE89A7A;

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

    CreateBackground();
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

void TurtleScene::CreateBackground() {
    // Soft pond / sky backdrop — keeps focus on the talking turtle
    sky_ = lv_obj_create(root_);
    lv_obj_set_size(sky_, width_, height_ * 2 / 5);
    lv_obj_set_pos(sky_, 0, 0);
    lv_obj_set_style_radius(sky_, 0, 0);
    lv_obj_set_style_border_width(sky_, 0, 0);
    lv_obj_set_style_bg_color(sky_, lv_color_hex(0x87CEEB), 0);
    lv_obj_remove_flag(sky_, LV_OBJ_FLAG_SCROLLABLE);

    water_ = lv_obj_create(root_);
    lv_obj_set_size(water_, width_, height_ - height_ * 2 / 5);
    lv_obj_set_pos(water_, 0, height_ * 2 / 5);
    lv_obj_set_style_radius(water_, 0, 0);
    lv_obj_set_style_border_width(water_, 0, 0);
    lv_obj_set_style_bg_color(water_, lv_color_hex(0x3A8FBF), 0);
    lv_obj_remove_flag(water_, LV_OBJ_FLAG_SCROLLABLE);

    // Simple shore strip
    lv_obj_t* shore = lv_obj_create(root_);
    lv_obj_set_size(shore, width_, 10);
    lv_obj_set_pos(shore, 0, height_ * 2 / 5 - 4);
    lv_obj_set_style_radius(shore, 0, 0);
    lv_obj_set_style_border_width(shore, 0, 0);
    lv_obj_set_style_bg_color(shore, lv_color_hex(0xC2A26B), 0);
    lv_obj_remove_flag(shore, LV_OBJ_FLAG_SCROLLABLE);
}

void TurtleScene::CreateTurtle() {
    // Face-forward "speaker" bust — large Franklin-style head + shell behind
    turtle_ = lv_obj_create(root_);
    lv_obj_set_size(turtle_, 200, 190);
    lv_obj_set_style_bg_opa(turtle_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(turtle_, 0, 0);
    lv_obj_set_style_pad_all(turtle_, 0, 0);
    lv_obj_remove_flag(turtle_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(turtle_, LV_ALIGN_CENTER, 0, 8);

    // Shell sits behind the head (yellow/brown Franklin shell)
    shell_ = lv_obj_create(turtle_);
    lv_obj_set_size(shell_, 150, 110);
    lv_obj_set_pos(shell_, 25, 70);
    lv_obj_set_style_radius(shell_, 55, 0);
    lv_obj_set_style_border_width(shell_, 4, 0);
    lv_obj_set_style_border_color(shell_, lv_color_hex(kShellRim), 0);
    lv_obj_set_style_bg_color(shell_, lv_color_hex(kShellYellow), 0);
    lv_obj_remove_flag(shell_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* plate = lv_obj_create(shell_);
    lv_obj_set_size(plate, 78, 58);
    lv_obj_center(plate);
    lv_obj_set_style_radius(plate, 18, 0);
    lv_obj_set_style_border_width(plate, 3, 0);
    lv_obj_set_style_border_color(plate, lv_color_hex(kShellBrown), 0);
    lv_obj_set_style_bg_color(plate, lv_color_hex(0xD4A93A), 0);
    lv_obj_remove_flag(plate, LV_OBJ_FLAG_SCROLLABLE);

    // Side plates
    for (int i = 0; i < 2; i++) {
        lv_obj_t* side = lv_obj_create(shell_);
        lv_obj_set_size(side, 28, 36);
        lv_obj_set_pos(side, i == 0 ? 12 : 110, 34);
        lv_obj_set_style_radius(side, 10, 0);
        lv_obj_set_style_border_width(side, 2, 0);
        lv_obj_set_style_border_color(side, lv_color_hex(kShellBrown), 0);
        lv_obj_set_style_bg_color(side, lv_color_hex(0xD4A93A), 0);
        lv_obj_remove_flag(side, LV_OBJ_FLAG_SCROLLABLE);
    }

    // Arms / flippers at sides of shell
    arm_left_ = lv_obj_create(turtle_);
    lv_obj_set_size(arm_left_, 34, 22);
    lv_obj_set_pos(arm_left_, 8, 120);
    lv_obj_set_style_radius(arm_left_, 11, 0);
    lv_obj_set_style_border_width(arm_left_, 0, 0);
    lv_obj_set_style_bg_color(arm_left_, lv_color_hex(kSkin), 0);
    lv_obj_remove_flag(arm_left_, LV_OBJ_FLAG_SCROLLABLE);

    arm_right_ = lv_obj_create(turtle_);
    lv_obj_set_size(arm_right_, 34, 22);
    lv_obj_set_pos(arm_right_, 158, 120);
    lv_obj_set_style_radius(arm_right_, 11, 0);
    lv_obj_set_style_border_width(arm_right_, 0, 0);
    lv_obj_set_style_bg_color(arm_right_, lv_color_hex(kSkin), 0);
    lv_obj_remove_flag(arm_right_, LV_OBJ_FLAG_SCROLLABLE);

    // Big Franklin head (dominant face)
    head_ = lv_obj_create(turtle_);
    lv_obj_set_size(head_, 124, 112);
    lv_obj_set_pos(head_, 38, 4);
    lv_obj_set_style_radius(head_, 56, 0);
    lv_obj_set_style_border_width(head_, 3, 0);
    lv_obj_set_style_border_color(head_, lv_color_hex(kSkinDark), 0);
    lv_obj_set_style_bg_color(head_, lv_color_hex(kSkin), 0);
    lv_obj_remove_flag(head_, LV_OBJ_FLAG_SCROLLABLE);

    // No cheek patches — they read as muddy blobs on the LCD
    cheek_left_ = nullptr;
    cheek_right_ = nullptr;

    // Brow ridges (hidden unless annoyed)
    brow_left_ = lv_obj_create(head_);
    lv_obj_set_size(brow_left_, 30, 5);
    lv_obj_set_pos(brow_left_, 18, 24);
    lv_obj_set_style_radius(brow_left_, 2, 0);
    lv_obj_set_style_border_width(brow_left_, 0, 0);
    lv_obj_set_style_bg_color(brow_left_, lv_color_hex(kSkinDark), 0);
    lv_obj_remove_flag(brow_left_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(brow_left_, LV_OBJ_FLAG_HIDDEN);

    brow_right_ = lv_obj_create(head_);
    lv_obj_set_size(brow_right_, 30, 5);
    lv_obj_set_pos(brow_right_, 76, 24);
    lv_obj_set_style_radius(brow_right_, 2, 0);
    lv_obj_set_style_border_width(brow_right_, 0, 0);
    lv_obj_set_style_bg_color(brow_right_, lv_color_hex(kSkinDark), 0);
    lv_obj_remove_flag(brow_right_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(brow_right_, LV_OBJ_FLAG_HIDDEN);

    // Big cute eyes — bright white sclera + dark pupils
    // Zero padding: LVGL defaults push children toward bottom-right
    eye_left_ = lv_obj_create(head_);
    lv_obj_set_size(eye_left_, 36, 36);
    lv_obj_set_pos(eye_left_, 18, 30);
    lv_obj_set_style_radius(eye_left_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_left_, 2, 0);
    lv_obj_set_style_border_color(eye_left_, lv_color_hex(kSkinDark), 0);
    lv_obj_set_style_bg_color(eye_left_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_pad_all(eye_left_, 0, 0);
    lv_obj_set_style_clip_corner(eye_left_, true, 0);
    lv_obj_remove_flag(eye_left_, LV_OBJ_FLAG_SCROLLABLE);

    pupil_left_ = lv_obj_create(eye_left_);
    lv_obj_set_size(pupil_left_, 14, 14);
    lv_obj_set_style_radius(pupil_left_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(pupil_left_, 0, 0);
    lv_obj_set_style_pad_all(pupil_left_, 0, 0);
    lv_obj_set_style_bg_color(pupil_left_, lv_color_hex(kPupil), 0);
    lv_obj_remove_flag(pupil_left_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(pupil_left_);

    lv_obj_t* glint_l = lv_obj_create(pupil_left_);
    lv_obj_set_size(glint_l, 5, 5);
    lv_obj_set_style_radius(glint_l, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(glint_l, 0, 0);
    lv_obj_set_style_pad_all(glint_l, 0, 0);
    lv_obj_set_style_bg_color(glint_l, lv_color_hex(0xFFFFFF), 0);
    lv_obj_remove_flag(glint_l, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(glint_l, LV_ALIGN_TOP_LEFT, 2, 2);

    eye_right_ = lv_obj_create(head_);
    lv_obj_set_size(eye_right_, 36, 36);
    lv_obj_set_pos(eye_right_, 70, 30);
    lv_obj_set_style_radius(eye_right_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(eye_right_, 2, 0);
    lv_obj_set_style_border_color(eye_right_, lv_color_hex(kSkinDark), 0);
    lv_obj_set_style_bg_color(eye_right_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_pad_all(eye_right_, 0, 0);
    lv_obj_set_style_clip_corner(eye_right_, true, 0);
    lv_obj_remove_flag(eye_right_, LV_OBJ_FLAG_SCROLLABLE);

    pupil_right_ = lv_obj_create(eye_right_);
    lv_obj_set_size(pupil_right_, 14, 14);
    lv_obj_set_style_radius(pupil_right_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(pupil_right_, 0, 0);
    lv_obj_set_style_pad_all(pupil_right_, 0, 0);
    lv_obj_set_style_bg_color(pupil_right_, lv_color_hex(kPupil), 0);
    lv_obj_remove_flag(pupil_right_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(pupil_right_);

    lv_obj_t* glint_r = lv_obj_create(pupil_right_);
    lv_obj_set_size(glint_r, 5, 5);
    lv_obj_set_style_radius(glint_r, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(glint_r, 0, 0);
    lv_obj_set_style_pad_all(glint_r, 0, 0);
    lv_obj_set_style_bg_color(glint_r, lv_color_hex(0xFFFFFF), 0);
    lv_obj_remove_flag(glint_r, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(glint_r, LV_ALIGN_TOP_LEFT, 2, 2);

    // Soft cute smile
    mouth_ = lv_obj_create(head_);
    mouth_closed_h_ = 8;
    mouth_open_h_ = 24;
    lv_obj_set_size(mouth_, 32, mouth_closed_h_);
    lv_obj_set_pos(mouth_, 46, 82);
    lv_obj_set_style_radius(mouth_, 12, 0);
    lv_obj_set_style_border_width(mouth_, 0, 0);
    lv_obj_set_style_bg_color(mouth_, lv_color_hex(kMouth), 0);
    lv_obj_remove_flag(mouth_, LV_OBJ_FLAG_SCROLLABLE);

    // Red neckerchief under chin (Franklin signature)
    neckerchief_ = lv_obj_create(turtle_);
    lv_obj_set_size(neckerchief_, 54, 18);
    lv_obj_set_pos(neckerchief_, 73, 108);
    lv_obj_set_style_radius(neckerchief_, 6, 0);
    lv_obj_set_style_border_width(neckerchief_, 0, 0);
    lv_obj_set_style_bg_color(neckerchief_, lv_color_hex(kNeckerchief), 0);
    lv_obj_remove_flag(neckerchief_, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* knot = lv_obj_create(turtle_);
    lv_obj_set_size(knot, 14, 14);
    lv_obj_set_pos(knot, 93, 118);
    lv_obj_set_style_radius(knot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(knot, 0, 0);
    lv_obj_set_style_bg_color(knot, lv_color_hex(0xA81C1C), 0);
    lv_obj_remove_flag(knot, LV_OBJ_FLAG_SCROLLABLE);
}

void TurtleScene::CreateOverlays() {
    // On root so it isn't clipped by the turtle container; aligned to head each frame
    zzz_label_ = lv_label_create(root_);
    lv_label_set_text(zzz_label_, "Zzz");
    lv_obj_set_style_text_color(zzz_label_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(zzz_label_, LV_OPA_COVER, 0);
    lv_obj_add_flag(zzz_label_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(zzz_label_);

    thought_bubble_ = lv_obj_create(root_);
    lv_obj_set_size(thought_bubble_, 52, 40);
    lv_obj_align(thought_bubble_, LV_ALIGN_CENTER, 90, -55);
    lv_obj_set_style_radius(thought_bubble_, 16, 0);
    lv_obj_set_style_border_width(thought_bubble_, 2, 0);
    lv_obj_set_style_border_color(thought_bubble_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_color(thought_bubble_, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(thought_bubble_, LV_OPA_90, 0);
    lv_obj_remove_flag(thought_bubble_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(thought_bubble_, LV_OBJ_FLAG_HIDDEN);

    thought_text_ = lv_label_create(thought_bubble_);
    lv_label_set_text(thought_text_, "...?");
    lv_obj_set_style_text_color(thought_text_, lv_color_hex(0x333333), 0);
    lv_obj_center(thought_text_);
}

bool TurtleScene::IsDrowsyMood(TurtleMood mood) const {
    return mood == TurtleMood::Sleeping || mood == TurtleMood::Relaxed || mood == TurtleMood::Awake;
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
    // If mid-annoyance, queue the real mood for after the reaction
    if (touch_react_ticks_ > 0) {
        mood_before_touch_ = EmotionToMood(emotion);
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
    touch_react_ticks_ = 0;
    if (on) {
        SetMood(TurtleMood::Sleeping);
    } else {
        SetMood(TurtleMood::Relaxed);
    }
}

void TurtleScene::OnTouchReaction() {
    if (power_save_ || !visible_) {
        return;
    }
    if (touch_react_ticks_ <= 0) {
        mood_before_touch_ = mood_;
    }
    touch_react_ticks_ = 28;  // ~2.2s of annoyance
    SetMood(TurtleMood::Angry);
    ESP_LOGI(TAG, "Touch -> annoyed");
}

void TurtleScene::ApplyMoodVisuals() {
    TurtleMood mood = power_save_ ? TurtleMood::Sleeping : mood_;
    bool drowsy = IsDrowsyMood(mood);
    bool listening = mood == TurtleMood::Listening || mood == TurtleMood::Thinking;
    bool annoyed = mood == TurtleMood::Angry;
    bool speaking = mood == TurtleMood::Speaking;
    bool sad = mood == TurtleMood::Sad;

    // Eyes
    if (eye_left_ != nullptr && eye_right_ != nullptr) {
        if (mood == TurtleMood::Sleeping) {
            // Closed sleepy lines instead of brown clipped pupils
            lv_obj_remove_flag(eye_left_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(eye_right_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(eye_left_, 34, 6);
            lv_obj_set_size(eye_right_, 34, 6);
            lv_obj_set_pos(eye_left_, 20, 48);
            lv_obj_set_pos(eye_right_, 72, 48);
            lv_obj_set_style_radius(eye_left_, 3, 0);
            lv_obj_set_style_radius(eye_right_, 3, 0);
            lv_obj_set_style_bg_color(eye_left_, lv_color_hex(kSkinDark), 0);
            lv_obj_set_style_bg_color(eye_right_, lv_color_hex(kSkinDark), 0);
            if (pupil_left_ != nullptr) {
                lv_obj_add_flag(pupil_left_, LV_OBJ_FLAG_HIDDEN);
            }
            if (pupil_right_ != nullptr) {
                lv_obj_add_flag(pupil_right_, LV_OBJ_FLAG_HIDDEN);
            }
        } else if (drowsy) {
            // Cute half-lids: keep pupils tiny and centered so nothing clips brown
            lv_obj_remove_flag(eye_left_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(eye_right_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(eye_left_, 36, 18);
            lv_obj_set_size(eye_right_, 36, 18);
            lv_obj_set_pos(eye_left_, 18, 40);
            lv_obj_set_pos(eye_right_, 70, 40);
            lv_obj_set_style_radius(eye_left_, 18, 0);
            lv_obj_set_style_radius(eye_right_, 18, 0);
            lv_obj_set_style_bg_color(eye_left_, lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_bg_color(eye_right_, lv_color_hex(0xFFFFFF), 0);
            if (pupil_left_ != nullptr) {
                lv_obj_remove_flag(pupil_left_, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_size(pupil_left_, 8, 8);
                lv_obj_center(pupil_left_);
            }
            if (pupil_right_ != nullptr) {
                lv_obj_remove_flag(pupil_right_, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_size(pupil_right_, 8, 8);
                lv_obj_center(pupil_right_);
            }
        } else {
            lv_obj_remove_flag(eye_left_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(eye_right_, LV_OBJ_FLAG_HIDDEN);
            int eye_size = listening ? 38 : 36;
            lv_obj_set_size(eye_left_, eye_size, eye_size);
            lv_obj_set_size(eye_right_, eye_size, eye_size);
            lv_obj_set_pos(eye_left_, 18, 30);
            lv_obj_set_pos(eye_right_, 70, 30);
            lv_obj_set_style_radius(eye_left_, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_radius(eye_right_, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_bg_color(eye_left_, lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_bg_color(eye_right_, lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_pad_all(eye_left_, 0, 0);
            lv_obj_set_style_pad_all(eye_right_, 0, 0);
            if (pupil_left_ != nullptr) {
                lv_obj_remove_flag(pupil_left_, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_size(pupil_left_, 14, 14);
            }
            if (pupil_right_ != nullptr) {
                lv_obj_remove_flag(pupil_right_, LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_size(pupil_right_, 14, 14);
            }
        }
    }

    // Always re-center pupils (padding-safe). Tiny look-aside only when pondering.
    if (pupil_left_ != nullptr && pupil_right_ != nullptr && mood != TurtleMood::Sleeping) {
        if (listening && !drowsy) {
            lv_obj_align(pupil_left_, LV_ALIGN_CENTER, 2, 0);
            lv_obj_align(pupil_right_, LV_ALIGN_CENTER, 2, 0);
        } else if (annoyed) {
            lv_obj_align(pupil_left_, LV_ALIGN_CENTER, -1, 1);
            lv_obj_align(pupil_right_, LV_ALIGN_CENTER, 1, 1);
        } else {
            lv_obj_center(pupil_left_);
            lv_obj_center(pupil_right_);
        }
    }

    // Angry brows
    if (brow_left_ != nullptr && brow_right_ != nullptr) {
        if (annoyed) {
            lv_obj_remove_flag(brow_left_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(brow_right_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(brow_left_, 16, 22);
            lv_obj_set_pos(brow_right_, 78, 22);
        } else {
            lv_obj_add_flag(brow_left_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(brow_right_, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // Mouth shape
    if (mouth_ != nullptr) {
        if (mood == TurtleMood::Sleeping || drowsy) {
            lv_obj_set_size(mouth_, 22, 6);
            lv_obj_set_pos(mouth_, 51, 84);
            lv_obj_set_style_radius(mouth_, 4, 0);
        } else if (annoyed) {
            lv_obj_set_size(mouth_, 30, 6);
            lv_obj_set_pos(mouth_, 47, 88);
            lv_obj_set_style_radius(mouth_, 3, 0);
        } else if (sad) {
            lv_obj_set_size(mouth_, 28, 6);
            lv_obj_set_pos(mouth_, 48, 90);
            lv_obj_set_style_radius(mouth_, 3, 0);
        } else if (speaking) {
            lv_obj_set_size(mouth_, 34, mouth_open_h_);
            lv_obj_set_pos(mouth_, 45, 74);
            lv_obj_set_style_radius(mouth_, 14, 0);
        } else if (listening) {
            lv_obj_set_size(mouth_, 14, 14);
            lv_obj_set_pos(mouth_, 55, 82);
            lv_obj_set_style_radius(mouth_, LV_RADIUS_CIRCLE, 0);
        } else {
            // Soft cute smile
            lv_obj_set_size(mouth_, 32, mouth_closed_h_);
            lv_obj_set_pos(mouth_, 46, 82);
            lv_obj_set_style_radius(mouth_, 12, 0);
        }
    }

    // Zzz when sleeping / standby / not activated — force visible + on top
    if (zzz_label_ != nullptr) {
        if (drowsy || mood == TurtleMood::Sleeping) {
            lv_obj_remove_flag(zzz_label_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(zzz_label_);
            lv_label_set_text(zzz_label_, "Zzz");
            lv_obj_set_style_text_color(zzz_label_, lv_color_hex(0xFFFFFF), 0);
        } else {
            lv_obj_add_flag(zzz_label_, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // Thought bubble while listening / pondering
    if (thought_bubble_ != nullptr) {
        if (listening) {
            lv_obj_remove_flag(thought_bubble_, LV_OBJ_FLAG_HIDDEN);
            if (thought_text_ != nullptr) {
                lv_label_set_text(thought_text_, "...?");
            }
        } else {
            lv_obj_add_flag(thought_bubble_, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // Backdrop dim when drowsy
    if (sky_ != nullptr) {
        lv_obj_set_style_bg_color(sky_,
            lv_color_hex((drowsy || mood == TurtleMood::Sleeping) ? 0x4A6070 : 0x87CEEB), 0);
    }
    if (water_ != nullptr) {
        lv_obj_set_style_bg_color(water_,
            lv_color_hex((drowsy || mood == TurtleMood::Sleeping) ? 0x1F4A66 : 0x3A8FBF), 0);
    }
}

void TurtleScene::Show() {
    if (visible_ || root_ == nullptr) {
        return;
    }
    visible_ = true;
    anim_tick_ = 0;
    touch_react_ticks_ = 0;
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_background(root_);
    ApplyMoodVisuals();
    if (anim_timer_ != nullptr) {
        lv_timer_resume(anim_timer_);
    }
    ESP_LOGI(TAG, "Franklin turtle scene active");
}

void TurtleScene::UpdateAnimation() {
    if (!visible_ || turtle_ == nullptr) {
        return;
    }

    TurtleMood mood = power_save_ ? TurtleMood::Sleeping : mood_;
    anim_tick_ = (anim_tick_ + 1) % 240;
    float phase = anim_tick_ * 3.14159f / 12.0f;
    bool drowsy = IsDrowsyMood(mood);
    bool listening = mood == TurtleMood::Listening || mood == TurtleMood::Thinking;
    bool speaking = mood == TurtleMood::Speaking;
    bool annoyed = mood == TurtleMood::Angry;

    // Gentle idle bob; stronger when speaking / annoyed
    int bob = 2;
    int sway = 0;
    if (speaking) {
        bob = 5;
    } else if (annoyed) {
        bob = 8;
        sway = static_cast<int>(std::sin(phase * 3.0f) * 6);
    } else if (listening) {
        bob = 3;
        sway = static_cast<int>(std::sin(phase * 0.4f) * 4);  // thoughtful lean
    } else if (drowsy) {
        bob = 2;
    }

    int y_off = static_cast<int>(std::sin(phase * 0.6f) * bob);
    lv_obj_align(turtle_, LV_ALIGN_CENTER, sway, 8 + y_off);

    // Speaking mouth chatter
    if (mouth_ != nullptr && speaking) {
        float mouth_wave = std::sin(phase * 2.2f);
        int mouth_h = mouth_closed_h_ + static_cast<int>((mouth_wave + 1.0f) * 0.5f *
                                                         (mouth_open_h_ - mouth_closed_h_));
        if (mouth_h < mouth_closed_h_) {
            mouth_h = mouth_closed_h_;
        }
        lv_obj_set_size(mouth_, 34, mouth_h);
        lv_obj_set_pos(mouth_, 45, 92 - mouth_h);
        lv_obj_set_style_radius(mouth_, mouth_h > 12 ? 14 : 6, 0);
    }

    // Annoyed: shake brows + frown twitch
    if (annoyed && brow_left_ != nullptr && brow_right_ != nullptr) {
        int brow_jiggle = static_cast<int>(std::sin(phase * 4.0f) * 2);
        lv_obj_set_pos(brow_left_, 16, 22 + brow_jiggle);
        lv_obj_set_pos(brow_right_, 78, 22 - brow_jiggle);
    }

    // Listening: arm "chin think" — raise left arm toward face
    if (arm_left_ != nullptr && arm_right_ != nullptr) {
        if (listening) {
            int think_bob = static_cast<int>(std::sin(phase * 0.8f) * 3);
            lv_obj_set_pos(arm_left_, 28, 88 + think_bob);   // up near chin
            lv_obj_set_pos(arm_right_, 158, 120);
        } else if (annoyed) {
            int flap = static_cast<int>(std::sin(phase * 3.5f) * 5);
            lv_obj_set_pos(arm_left_, 8, 120 + flap);
            lv_obj_set_pos(arm_right_, 158, 120 - flap);
        } else {
            int idle = static_cast<int>(std::sin(phase * 0.5f) * 2);
            lv_obj_set_pos(arm_left_, 8, 120 + idle);
            lv_obj_set_pos(arm_right_, 158, 120 - idle);
        }
    }

    // Zzz floats up from head on standby / sleep
    if (zzz_label_ != nullptr && (drowsy || mood == TurtleMood::Sleeping) && head_ != nullptr) {
        int z = static_cast<int>(std::sin(phase * 0.35f) * 6);
        // Keep below status/listening bar — sit just above the temple, not the top edge
        lv_obj_align_to(zzz_label_, head_, LV_ALIGN_OUT_TOP_RIGHT, 4 + z / 2, 14 - z);
        lv_obj_remove_flag(zzz_label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(zzz_label_);
    }

    // Thought bubble gently floats
    if (thought_bubble_ != nullptr && listening) {
        int t = static_cast<int>(std::sin(phase * 0.5f) * 5);
        lv_obj_align(thought_bubble_, LV_ALIGN_CENTER, 92, -58 + t);
        // Cycle ponder text
        if (thought_text_ != nullptr) {
            const char* texts[] = {"...?", "hmm", "...", "?!"};
            lv_label_set_text(thought_text_, texts[(anim_tick_ / 20) % 4]);
        }
    }

    // End annoyance reaction and restore prior mood
    if (touch_react_ticks_ > 0) {
        touch_react_ticks_--;
        if (touch_react_ticks_ == 0 && !power_save_) {
            SetMood(mood_before_touch_);
        }
    }
}

void TurtleScene::AnimTimerCb(lv_timer_t* timer) {
    auto* scene = static_cast<TurtleScene*>(lv_timer_get_user_data(timer));
    if (scene != nullptr) {
        scene->UpdateAnimation();
    }
}
