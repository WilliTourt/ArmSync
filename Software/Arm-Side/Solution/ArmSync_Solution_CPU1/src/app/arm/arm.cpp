#include "arm.h"
#include "app/drivers/Emm_V5.h"

Arm::Arm(Motor& j1, Motor& j2, Motor& j3,
         Motor& j4, Motor& j5, Motor& j6)
    : _j1(j1), _j2(j2), _j3(j3),
      _j4(j4), _j5(j5), _j6(j6) {}

bool Arm::init() {
    if (!Emm_V5_Init()) return false;

    return _j1.init() && _j2.init() && _j3.init()
        && _j4.init() && _j5.init() && _j6.init();
}

bool Arm::isReady() const {
    return Emm_V5_IsReady();
}

void Arm::setAngles(const float angles_deg[6]) {
    _j1.setAngle(angles_deg[0]);
    _j2.setAngle(angles_deg[1]);
    _j3.setAngle(angles_deg[2]);
    _j4.setAngle(angles_deg[3]);
    _j5.setAngle(angles_deg[4]);
    _j6.setAngle(angles_deg[5]);
}

void Arm::getFeedback(float angles_deg[6], float currents_ma[6]) {
    (void)angles_deg;
    (void)currents_ma;
}

void Arm::stop() {
    _j1.stop(); _j2.stop(); _j3.stop();
    _j4.stop(); _j5.stop(); _j6.stop();
}

void Arm::enable(bool en) {
    Emm_V5_En_Control(_j1.addr(), en, false);
    Emm_V5_En_Control(_j2.addr(), en, false);
    Emm_V5_En_Control(_j3.addr(), en, false);
    Emm_V5_En_Control(_j4.addr(), en, false);
    Emm_V5_En_Control(_j5.addr(), en, false);
    Emm_V5_En_Control(_j6.addr(), en, false);
}
