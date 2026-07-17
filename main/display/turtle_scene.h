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
    void OnTouchReaction();
    TurtleMood GetMood() const { return mood_; }

private:
    lv_obj_t* root_ = nullptr;
    lv_obj_t* sky_ = nullptr;
    lv_obj_t* water_ = nullptr;
    lv_obj_t* turtle_ = nullptr;
    lv_obj_t* shell_ = nullptr;
    lv_obj_t* head_ = nullptr;
    lv_obj_t* cheek_left_ = nullptr;
    lv_obj_t* cheek_right_ = nullptr;
    lv_obj_t* eye_left_ = nullptr;
    lv_obj_t* eye_right_ = nullptr;
    lv_obj_t* pupil_left_ = nullptr;
    lv_obj_t* pupil_right_ = nullptr;
    lv_obj_t* brow_left_ = nullptr;
    lv_obj_t* brow_right_ = nullptr;
    lv_obj_t* mouth_ = nullptr;
    lv_obj_t* neckerchief_ = nullptr;
    lv_obj_t* arm_left_ = nullptr;
    lv_obj_t* arm_right_ = nullptr;
    lv_obj_t* zzz_label_ = nullptr;
    lv_obj_t* thought_bubble_ = nullptr;
    lv_obj_t* thought_text_ = nullptr;

    lv_timer_t* anim_timer_ = nullptr;
    TurtleMood mood_ = TurtleMood::Relaxed;
    TurtleMood mood_before_touch_ = TurtleMood::Relaxed;
    bool power_save_ = false;
    bool visible_ = false;
    int anim_tick_ = 0;
    int width_ = 0;
    int height_ = 0;
    int touch_react_ticks_ = 0;
    int mouth_closed_h_ = 6;
    int mouth_open_h_ = 22;

    void CreateBackground();
    void CreateTurtle();
    void CreateOverlays();
    void ApplyMoodVisuals();
    void UpdateAnimation();
    bool IsDrowsyMood(TurtleMood mood) const;
    static void AnimTimerCb(lv_timer_t* timer);
    static TurtleMood EmotionToMood(const char* emotion);
};

#endif
