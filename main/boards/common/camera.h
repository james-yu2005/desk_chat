#ifndef CAMERA_H
#define CAMERA_H

#include <cstdint>
#include <string>
#include <vector>

class Camera {
public:
    virtual void SetExplainUrl(const std::string& url, const std::string& token) = 0;
    virtual bool Capture() = 0;
    virtual bool SetHMirror(bool enabled) = 0;
    virtual bool SetVFlip(bool enabled) = 0;
    virtual bool SetSwapBytes(bool enabled) { return false; }  // Optional, default no-op
    virtual std::string Explain(const std::string& question) = 0;

    // Downsample the current frame to a luma grid for desk-presence heuristics.
    virtual bool BuildPresenceGrid(std::vector<uint8_t>& grid, int cols, int rows) {
        (void)grid;
        (void)cols;
        (void)rows;
        return false;
    }
};

#endif // CAMERA_H
