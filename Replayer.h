#pragma once
#include <fstream>
#include <vector>
#include "Vector2D.h"
#include "Recorder.h"

class Replayer {
public:
    Replayer(const std::string& filename);
    ~Replayer();

    bool begin();                // open & parse file
    bool next(FrameData& out);   // fill out next frame, return false at EOF
    void end();                  // close file

private:
    std::ifstream in;
    std::string fname;
};
