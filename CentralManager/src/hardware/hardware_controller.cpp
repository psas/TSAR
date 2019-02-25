#include "hardware_controller.h"


// constructor, need pointer to link
hardware_controller::
hardware_controller(std::shared_ptr<link_logger> & input) : _ll(input) {
    wiringPiSetup();

    // setup i2c sensors
    _i2c_fds.MPL3115A2_1 = i2c_library::MPL3115A2_setup(MPL3115A2_1_ADD);
    _i2c_fds.MPL3115A2_2 = i2c_library::MPL3115A2_setup(MPL3115A2_2_ADD);

    // setup gpio pins
    pinMode(LIGHT_GPIO, OUTPUT);
    digitalWrite(LIGHT_GPIO, LOW);  // init value (LOW/HIGH)
    _light_status = false;    // init state

    // setup uart Actuator Controller connection
    _uart_fd = serialOpen(UART_PATH, BUAD_RATE);
    _AC_connected = true;    // init state
}

hardware_controller::
~hardware_controller() {
    serialClose(_uart_fd); // close uart connection
}


void hardware_controller::
driver_loop() {
    std::chrono::system_clock::time_point current_time;
    hardware_data_frame data_frame;

    _driver_running = true;
    while(_driver_running) {
        _mutex.lock();

        update_i2c_data();
        
        // read uart message
        uart_library::read(_AC_data, _uart_fd);

        // send uart message
        current_time = std::chrono::system_clock::now();
        if(current_time >= _next_heartbeat_time) {
            uart_library::send_default(_uart_fd); // TODO define this as non default commands
            _next_heartbeat_time = current_time + std::chrono::milliseconds(HB_TIME_MS);
        }

        if(_ll != NULL) {
            make_frame(data_frame);
            _ll->send(data_frame);
        }

        _mutex.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(HDW_DRIVER_DELAY));
    }
    return;
}


// give a copy of data frame
void hardware_controller::
get_frame(hardware_data_frame & input) {
    _mutex.lock();
    make_frame(input);
    _mutex.unlock();
    return;
}


// makes a hardware data frame
void hardware_controller::
make_frame(hardware_data_frame & input) {
    get_time_us(input.time);
    input.i2c_data = _i2c_data;
    input.light_status = _light_status;
    input.AC_data = _AC_data;
    return;
}


// build new sensor frame from live sensors 
void hardware_controller::
update_i2c_data() {
    // TODO deal with i2c sensor deconnecting
    _i2c_data.pres_1 = i2c_library::MPL3115A2_pres(_i2c_fds.MPL3115A2_1);
    _i2c_data.temp_1 = i2c_library::MPL3115A2_temp(_i2c_fds.MPL3115A2_1);
    _i2c_data.pres_2 = i2c_library::MPL3115A2_pres(_i2c_fds.MPL3115A2_2);
    _i2c_data.temp_2 = i2c_library::MPL3115A2_temp(_i2c_fds.MPL3115A2_2);
    return;
}


// make gpio pin voltage high
int hardware_controller::
light_on() {
    _mutex.lock();

    digitalWrite(LIGHT_GPIO, HIGH);
    _light_status = true;

    _mutex.unlock();
    return 1;
}


// make gpio pin voltage low
int hardware_controller::
light_off() {
    _mutex.lock();

    digitalWrite(LIGHT_GPIO, LOW);
    _light_status = false;

    _mutex.unlock();
    return 1;
}

