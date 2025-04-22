#pragma once
#include <fstream>
#include <vector>
#include "Vector2D.h"

struct FrameData {
    Vector2D player;
    std::vector<Vector2D> enemies;
    std::vector<Vector2D> pets;
    std::vector<Vector2D> bullets;
};

// Simple CSV‐style recorder
class Recorder {
public:
    Recorder(const std::string& filename);
    ~Recorder();

    void begin();                      // open file for writing
    void recordFrame(const FrameData& frame);
    void end();                        // close file

private:
    std::ofstream out;
    std::string fname;
};
