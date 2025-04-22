#include "Recorder.h"
#include <sstream>

Recorder::Recorder(const std::string& filename)
  : fname(filename)
{}

Recorder::~Recorder() {
    if (out.is_open()) out.close();
}

void Recorder::begin() {
    out.open(fname, std::ios::out);
    // write header (optional)
    out << "P.x,P.y";
    // we won't know how many enemies/pets ahead of time, so we'll just write counts at front of each line
    out << ",E.count";
    out << ",Pet.count";
    out << "\n";
}

void Recorder::recordFrame(const FrameData& f) {
    if (!out.is_open()) return;

    // player, enemy‑count, pet‑count (unchanged)…
    out << f.player.x << "," << f.player.y << ","
        << f.enemies.size() << "," << f.pets.size();

    // enemy positions…
    for (auto& e : f.enemies) out << "," << e.x << "," << e.y;
    // pet positions…
    for (auto& p : f.pets)    out << "," << p.x << "," << p.y;

    // now bullets
    out << "," << f.bullets.size();
    for (auto& b : f.bullets) out << "," << b.x << "," << b.y;

    out << "\n";
}

void Recorder::end() {
    if (out.is_open()) {
        out.close();
    }
}
