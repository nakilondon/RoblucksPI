//
// Created by james on 06/03/19.
//

#ifndef ROBLUCKSPI_TIMER_H
#define ROBLUCKSPI_TIMER_H

#include <iostream>
#include <thread>
#include <chrono>

class Timer {
    bool clear = false;

public:
    void setTimeout(std::function<void()>function, int delay);
    void setInterval(std::function<void()>function, int interval);
    void stop();

};

#endif //ROBLUCKSPI_TIMER_H
