#include "Replayer.h"
#include <sstream>

Replayer::Replayer(const std::string& filename)
  : fname(filename)
{}

Replayer::~Replayer() {
    if (in.is_open()) in.close();
}

bool Replayer::begin() {
    in.open(fname, std::ios::in);
    if (!in.is_open()) return false;
    std::string header;
    std::getline(in, header);
    return true;
}

bool Replayer::next(FrameData& fd) {
    if (!in.is_open() || in.eof()) return false;
    std::string line;
    std::getline(in, line);
    if (line.empty()) return false;

    std::stringstream ss(line);
    std::string cell;

    // player
    std::getline(ss, cell, ','); fd.player.x = std::stof(cell);
    std::getline(ss, cell, ','); fd.player.y = std::stof(cell);
    // counts
    size_t ecount, pcount;
    std::getline(ss, cell, ','); ecount = std::stoul(cell);
    std::getline(ss, cell, ','); pcount = std::stoul(cell);

    fd.enemies.clear();
    for (size_t i = 0; i < ecount; ++i) {
        Vector2D v;
        std::getline(ss, cell, ','); v.x = std::stof(cell);
        std::getline(ss, cell, ','); v.y = std::stof(cell);
        fd.enemies.push_back(v);
    }

    fd.pets.clear();
    for (size_t i = 0; i < pcount; ++i) {
        Vector2D v;
        std::getline(ss, cell, ','); v.x = std::stof(cell);
        std::getline(ss, cell, ','); v.y = std::stof(cell);
        fd.pets.push_back(v);
    }
    return true;
}

void Replayer::end() {
    if (in.is_open()) in.close();
}
