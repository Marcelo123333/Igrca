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
    out << "P.x,P.y";
    out << ",E.count";
    out << ",Pet.count";
    out << "\n";
}

void Recorder::recordFrame(const FrameData& f) {
    if (!out.is_open()) return;


    out << f.player.x << "," << f.player.y << ","
        << f.enemies.size() << "," << f.pets.size();

    // POZICIJA ENEMIJA
    for (auto& e : f.enemies) out << "," << e.x << "," << e.y;
    // POZICIJA PETA
    for (auto& p : f.pets)    out << "," << p.x << "," << p.y;

    // METKI
    out << "," << f.bullets.size();
    for (auto& b : f.bullets) out << "," << b.x << "," << b.y;

    out << "\n";
}

void Recorder::end() {
    if (out.is_open()) {
        out.close();
    }
}
