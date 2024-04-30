//creating clock class to hold time values in an object; allows use of interrupt routines to change values unlike arrays

#include <string>
using namespace std;

class Clock {
    private:
        int hr, min, sec;   //creating time variables
        string ampm;

    public:
    //default constructor
    Clock();

    //constructor for clock object
    Clock(int h, int m, int s, string ap);

    //update time member function
    void set_time(int h, int m, int s, string ap);

    //member functions for returning time object variables
    int hour();

    int minute();

    int second();

    string am_pm();
};
