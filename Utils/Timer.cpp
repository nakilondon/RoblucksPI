//
// Created by james on 06/03/19.
//

#ifndef ROBLUCKSPI_TIMER_CPP
#define ROBLUCKSPI_TIMER_CPP

#include "Timer.h"



void Timer::setTimeout(std::function<void()>function, int delay) {
    this->clear = false;
    std::thread t([=]() {
        if(this->clear) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(this->clear) return;
        function();
    });
    t.detach();
}

void Timer::setInterval(std::function<void()>function, int interval) {
    this->clear = false;
    std::thread t([=]() {
        while(true) {
            if(this->clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(this->clear) return;
            function();
        }
    });
    t.detach();
}

void Timer::stop() {
    this->clear = true;
}

#endif //ROBLUCKSPI_TIMER_CPP
