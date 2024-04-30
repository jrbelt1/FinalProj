#include "Temperature.h"

//default constructor
Temperature::Temperature(){
    cels = 22;
    fahr = 72;
    unit = ' ';
}

//constructor for temp object
Temperature::Temperature(int c, int f, char u){
    cels = c;
    fahr = f;
    unit = u;
}

//update temperature member function
void Temperature::set_temp(int c, int f){
    cels = c;
    fahr = f;
}

//update temp unit member function
void Temperature::set_unit(char u){
    unit = u;
}

//member functions for returning temp object variables
int Temperature::display_temp(){
    if (unit == 'C'){
        return cels;
    }
    else {
        return fahr;
    }
}

char Temperature::unit_char(){
    return unit;
};
