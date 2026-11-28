#include "practice_fixes.hpp"

#include <Geode/modify/CheckpointObject.hpp>

class $modify(CheckpointObject) {
  #ifdef GEODE_IS_WINDOWS
  bool init() {
    bool ret = CheckpointObject::init();
    CheckpointObject* cp = this;
  #else
  static CheckpointObject* create() {
    CheckpointObject* ret = CheckpointObject::create();
    CheckpointObject* cp = ret;
  #endif

    if (!cp) return ret;

    auto& g = Global::get();
    PlayLayer* pl = PlayLayer::get();

    PlayerData p1Data = PlayerPracticeFixes::saveData(pl->m_player1);
    PlayerData p2Data = PlayerPracticeFixes::saveData(pl->m_player2);

    Global::get().checkpoints[cp] = {
      Global::getCurrentFrame(),
      p1Data,
      p2Data,
      #ifdef GEODE_IS_WINDOWS
      *(uintptr_t*)((char*)geode::base::get() + seedAddr),
      #else
      0,
      #endif
      Global::get().previousFrame
    };

    return ret;

  }
};
