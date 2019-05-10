#ifndef __CC__
#define __CC__

#include <thread>               // sleep_for
#include <chrono>               // time
#include <memory>               // shared ptr

#include "central_manager.h"
#include "system_status.h"

#define CC_DELAY 500            // milliseconds


class CommandControl {
    public:
        CommandControl();
        ~CommandControl();
        int start_system();

    private:
        int interface();

        std::shared_ptr<CentralManager> CM;
        std::shared_ptr<SystemStatus> status;
};

#endif  // __CC__
