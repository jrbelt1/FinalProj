#include "Clock.h"
#include <iostream>

//default constructor
Clock::Clock(){
    hr = 11;
    min = 59;
    sec = 55;
    ampm = "PM";
}

//constructor for clock object
Clock::Clock(int h, int m, int s, string ap){
    hr = h;
    min = m;
    sec = s;
    ampm = ap;
}

//update time member function
void Clock::set_time(int h, int m, int s, string ap){
    hr = h;
    min = m;
    sec = s;
    ampm = ap;
}

//member functions for returning time object variables
int Clock::hour(){
    return hr;
}

int Clock::minute(){
    return min;
}

int Clock::second(){
    return sec;
}

string Clock::am_pm(){
    return ampm;
};
