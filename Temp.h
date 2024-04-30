//creating temperature class to hold temp values in an object; allows use of interrupt routines to change values unlike arrays

#include <string>
using namespace std;

class Temperature
{
    private:
    int cels, fahr; //creating temp variables
    char unit;

    public:
    //default constructor
    Temperature();

    //constructor for temp object
    Temperature(int c, int f, char u);

    //update temperature member function
    void set_temp(int c, int f);

    //update temp unit member function
    void set_unit(char u);

    //member functions for returning temp object variables
    int display_temp();

    char unit_char();
};
