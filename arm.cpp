//
// Example how to play a tune using MAVSDK.
//

#include <cstdint>
#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
//#include <mavsdk/plugins/offboard/offboard.h>
#include <iostream>
#include <thread>
#include <chrono>



using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

std::shared_ptr<System> get_system(Mavsdk& mavsdk)
{
    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has an
    // autopilot, we decide to use it.
    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.subscribe_on_new_system(nullptr);
            prom.set_value(system);
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found.\n";
        return {};
    }

    // Get discovered system now.
    return fut.get();
}

int main(int argc, char** argv)
{

    std::chrono::steady_clock clock; 

    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = get_system(mavsdk);
    if (!system) {
        return 1;
    }

    //auto offboard = Offboard(system);
    

    /////////////////////////////////////////////////////////////////////////
    //                              Action                                 //           
    /////////////////////////////////////////////////////////////////////////
    auto action = Action(system);
    Action::Result arm_result;
    Action::Result disarm_result;
    Action::Result takeoff_result;
    Action::Result land_result;
    Action::Result orbit_result;

    //orbit Parameters
    double x_orbit = 0;
    double y_orbit = 0;
    float  z_orbit = 1.5;
    float radius = 1;
    float velocity = 1;
    Action::OrbitYawBehavior yaw_behavior = Action::OrbitYawBehavior::HoldFrontTangentToCircle;



    char cmd = 'n';
    while(cmd!='q'){
        switch(cmd){
            case 'n': //do nothing
                break;
            case 'a': //arming 
                std::cout << "Arming..." << '\n';
                arm_result=arm_result = action.arm();
                if (arm_result != Action::Result::Success) {  
                    std::cout << "Arming failed:" << arm_result <<  '\n';
                    return 1; //Exit if arming fails
                }
                break;
            case 'd':
                std::cout << "Disarming..." << '\n';
                disarm_result = action.disarm();
                if (disarm_result != Action::Result::Success) {  
                    std::cout << "Disarming failed:" << disarm_result <<  '\n';
                    return 1; //Exit if arming fails
                }
                break;
            case 't':
                std::cout << "Taking off..." << '\n';
                takeoff_result = action.takeoff();
                if (takeoff_result != Action::Result::Success) {  
                    std::cout << "Takeoff failed:" << takeoff_result <<  '\n';
                    return 1; //Exit if arming fails
                }
                break;
            case 'l':
                std::cout << "Landing..." << '\n';
                land_result = action.land();
                if (land_result != Action::Result::Success) {  
                    std::cout << "Landing failed:" << land_result <<  '\n';
                    return 1; //Exit if arming fails
                }
                break;
            case 'p':
                //x_orbit = Telemetry::Position::latitude_deg{double(NAN)};
                //y_orbit = Telemetry::Position::longitude_deg{double(NAN)};
                //z_orbit = Telemetry::Position::absolute_altitude_m{ float(NAN)};
                break;
            case 'o':
                std::cout << "Orbiting..." << '\n';
                orbit_result = action.do_orbit(radius,velocity,yaw_behavior,x_orbit,y_orbit,z_orbit);
                if (land_result != Action::Result::Success) {  
                    std::cout << "Landing failed:" << land_result <<  '\n';
                    return 1; //Exit if arming fails
                }
                break;
            default:
                std::cout<<"dont know this command: type q to quit"<<'\n';
        }
        std::cin>>cmd;
    }
    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout<<"quitting..."<<'\n';
    return 0;
}
