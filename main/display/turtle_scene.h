#ifndef TURTLE_SCENE_H
#define TURTLE_SCENE_H

#include <lvgl.h>
#include <cstdint>

enum class TurtleMood {
    Awake,
    Relaxed,
    Listening,
    Speaking,
    Sleeping,
    Happy,
    Sad,
    Thinking,
    Angry,
};

class TurtleScene {
public:
    TurtleScene(lv_obj_t* parent, int width, int height);
    ~TurtleScene();

    void Show();
    void SetMood(TurtleMood mood);
    void SetMoodFromEmotion(const char* emotion);
    void SetPowerSave(bool on);
    TurtleMood GetMood() const { return mood_; }

private:
    lv_obj_t* root_ = nullptr;
    lv_obj_t* surface_layer_ = nullptr;
    lv_obj_t* mid_layer_ = nullptr;
    lv_obj_t* deep_layer_ = nullptr;
    lv_obj_t* turtle_ = nullptr;
    lv_obj_t* head_ = nullptr;
    lv_obj_t* eye_left_ = nullptr;
    lv_obj_t* eye_right_ = nullptr;
    lv_obj_t* flipper_left_ = nullptr;
    lv_obj_t* flipper_right_ = nullptr;
    lv_obj_t* zzz_label_ = nullptr;
    lv_obj_t* thought_bubble_ = nullptr;
    lv_obj_t* bubbles_[6] = {};

    lv_timer_t* anim_timer_ = nullptr;
    TurtleMood mood_ = TurtleMood::Relaxed;
    bool power_save_ = false;
    bool visible_ = false;
    int anim_tick_ = 0;
    int turtle_x_ = 0;
    int turtle_y_ = 0;
    int turtle_dir_ = 1;
    int bubble_y_[6] = {};
    int bubble_x_[6] = {};
    int width_ = 0;
    int height_ = 0;

    void CreateOceanBackground();
    void CreateTurtle();
    void CreateBubbles();
    void CreateOverlays();
    void ApplyMoodVisuals();
    void UpdateAnimation();
    static void AnimTimerCb(lv_timer_t* timer);
    static TurtleMood EmotionToMood(const char* emotion);
};

#endif
