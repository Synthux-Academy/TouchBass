#include "switches.h"

using namespace synthux;
using namespace daisy;
using namespace seed;

void Switches::Init() {
    _switch_7_8.Init(D7, D6);
    _switch_9_10.Init(D9, D8);
};

int Switches::A() {
    return _switch_7_8.Read();
};

int Switches::B() {
    return _switch_9_10.Read();
};
