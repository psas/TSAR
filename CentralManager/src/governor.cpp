#include "governor.h"


int main() {
    if(getuid() != 0) {
        std::cout << "Run as root" << std::endl;
        return 0;
    }

    if(nice(NICE_VALUE) != NICE_VALUE) {
        std::cout << "Setting nice to NICE_VALUE failed." << std::endl;
        // TODO: return or continue ?
    }

    governor gov;
    gov.start_system();
    return 1;
}

// constructor
governor::
governor() : serv(), link(serv), hdw_ctrl(link), seq(link, hdw_ctrl), 
            seq_thread_running(false), hdw_thread_running(false), 
            link_thread_running(false), server_thread_running(false) {}


// deconstructor
governor::
~governor() {
    // Stop all threads if they are still running
    if(hdw_thread.joinable())
        hdw_thread.join();
    if(seq_thread.joinable())
        seq_thread.join();
    if(link_thread.joinable())
        link_thread.join();
    if(server_thread.joinable())
        server_thread.join();
}


/* Calls all run function below to (re)start threads as needed, 
 * until sequencer is done sequencing.
 */
void governor::
start_system() {
    std::cout << "System is starting up." << std::endl;
    
    do {
        gov_mutex.lock();

        // (re)start threads
        if(hdw_thread_running == false)
            hdw_thread = std::thread(&governor::run_hdw, this);
        if(seq_thread_running == false)
            seq_thread = std::thread(&governor::run_seq, this);;
        if(link_thread_running == false)
            link_thread = std::thread(&governor::run_link, this);
        if(server_thread_running == false)
            server_thread = std::thread(&governor::run_server, this);

        // TODO: deal with logging cerr
        
        gov_mutex.unlock();
        
        std::this_thread::sleep_for(std::chrono::microseconds(GOV_DRIVER_DELAY));
    } while(seq.is_running()); // ask the sequencer if it needs the system running

    std::cout << "System is shuting down." << std::endl;

    // tell main classes to stop their driver 
    hdw_ctrl.stop_driver();
    link.stop_driver();
    serv.stop_driver();
    return;
}


// handles the hardware thread
void governor::run_hdw() {
    hdw_thread_running = true;
    std::cout << "Hardware thread has started." << std::endl;
    pthread_setname_np(pthread_self(), "Hardware");
    hdw_ctrl->driver_loop();
    std::cout << "Hardware thread has stoped." << std::endl;
    hdw_thread_running = false;
}


// handles the sequencer thread
void governor::run_seq() {
    seq_thread_running = true;
    std::cout << "Sequencer thread has started." << std::endl;
    pthread_setname_np(pthread_self(), "Sequencer");
    seq->driver_loop();
    std::cout << "Sequencer thread has stoped." << std::endl;
    seq_thread_running = false;
}


// handles the link thread
void governor::run_link() {
    link_thread_running = true;
    std::cout << "Link thread has started." << std::endl;
    pthread_setname_np(pthread_self(), "Link");
    link->driver_loop();
    std::cout << "Link thread has stoped." << std::endl;
    link_thread_running = false;
}


// handles the server thread
void governor::run_server() {
    server_thread_running = true;
    std::cout << "Server thread has started." << std::endl;
    pthread_setname_np(pthread_self(), "Server");
    serv->driver_loop();
    std::cout << "Server thread has stoped." << std::endl;
    server_thread_running = false;
}
