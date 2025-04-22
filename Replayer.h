#pragma once
#include <fstream>
#include <vector>
#include "Vector2D.h"
#include "Recorder.h"

class Replayer {
public:
    Replayer(const std::string& filename);
    ~Replayer();

    bool begin();
    bool next(FrameData& out);
    void end();

private:
    std::ifstream in;
    std::string fname;
};
