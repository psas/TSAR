#ifndef __STATE__
#define __STATE__

#include <string>       // string

struct State {
    int current_state;
    std::string last_command;

    int fire_count; // 0 for start up, 0 > for count

    // TODO add any data needed
};

#endif // __SYSTEM_STATUS__