#include "mbed.h"
#include "TextLCD.h"
#include <string>
#include "Clock.h"          //header file for clock class that controls time
#include "Temperature.h"    //header file for temperature class that controls temp

//bugs: need to set up set mode button press and switch unit button press as interrupts so they aren't affected by the wait
//maybe turn time and temperature arrays into Clock and Temperature Class



//TextLCD lcd(RS, E, D4, D5, D6, D7);                          // RS, E, D4-D7, LCDType=LCD16x2, BL=NC, E2=NC, LCDTCtrl=HD44780
TextLCD lcd(PA_0, PA_1, PA_4, PB_0, PC_1, PC_0);

//declare operating mode functions
void normal();                              //increment time by 1 second and return correct am/pm
void set();                                 //call user to set time values, returning inputted am/pm
int input(int range_low, int range_up);     //actual function for user to inputting set time values, maintain correct range for each
int str_to_int(string num);                 //change user string input to usable integers

//initializing GPIO pins for keypad
DigitalOut ROW1(PA_6);
DigitalOut ROW2(PA_7);
DigitalOut ROW3(PB_6);
DigitalOut ROW4(PC_7);

DigitalIn COL1(PA_9, PullUp);
DigitalIn COL2(PA_8, PullUp);
DigitalIn COL3(PB_10, PullUp);
DigitalIn COL4(PB_4, PullUp);

//create interrupt pin connected to COL1 of keypad externally
InterruptIn star(PA_10);
void setmode();          //declaration of interrupt routine

//declaration of keypad functions for retrieving character input
int col_scan(void);
char keypad_scan(void);

//initializing temp sensor
AnalogIn sens(D13);
InterruptIn button(PC_13);  //create push button as interrupt object
void tempread();            //declaration of function that reads temperature
void switchunit();          //declaration of function that switches temp unit


Clock clk(11, 59, 55, "PM");    //create clk object with default settings
Temperature temp(22, 72, 'C');  //create temp object with default settings

int main(){      
    button.fall(&switchunit);   //button press starts interrupt service routine that switches temp unit
    star.fall(&setmode);        //'*' press on keypad starts interrupt service routine that 

   //loop updating display every second
    while(true){
       lcd.cls();
       //output to LCD display with format hh:mm:ss xx yy T
       lcd.printf("%0*d:%0*d:%0*d %s %i%c\n", 2, clk.hour(), 2, clk.minute(), 2, clk.second(), clk.am_pm(), temp.display_temp(), temp.unit_char());
       tempread();      //read temperature
       
       normal();        //call function to add 1 second, toggle am/pm when hr = 12
       wait_ms(975);    //1 second timer
   }
}

//defining function for normal operating mode; adds 1 second to time
void normal(){
    int hr = clk.hour();    //transfer current values in clock object to function variables
    int min = clk.minute();
    int sec = clk.second();
    string ampm = clk.am_pm();

    //add 1s
    sec = (sec + 1) % 60; //% wraps around if over 59

    //add one minute if seconds = 60
    if (sec == 0){
        min = (min + 1) % 60;

        //add one hour if minute = 60
        if (min == 0 && hr != 11){      //exclude when hr = 11; range is 1-12 and % wraps to 0
            hr = (hr + 1) % 12;
        }
        else if (min == 0 && hr == 11)  //manually set hr time to 12 and toggle ampm
        { 
            hr = 12;
            if (ampm == "AM"){
                ampm = "PM";
            }
            else {
                ampm = "AM";
            }
         }
    }

    clk.set_time(hr, min, sec, ampm);   //update clock object to incremented values
}

//interrupt routine that changes operation to set mode; runs when COL1 keypad buttons pressed
void setmode(){
    if (keypad_scan() == '*'){  //exclude COL buttons other than '*'
        wait_ms(300);           //keypad debounce
        set();                  //set clock function
     }
}

//defining function for set operating mode
void set(){
    int hr, min, ap, sec = 0; //create new variables to override current clock object, seconds defaults to 0
    string ampm = "";

    //setting time terms:
    //input(range_low, range_up)  records + displays input and maintains correct ranges
    //setting hour
    lcd.cls();
    lcd.printf("HOUR (# TO ENTER): ");
    hr = input(1, 12); 
    wait_ms(300); //keypad debounce

    //setting min
    lcd.cls();
    lcd.printf("MIN (# TO ENTER): ");
    min = input(0, 59);
    wait_ms(300); //keypad debounce

    //setting AM/PM
    lcd.cls();
    lcd.printf("AM (0) OR PM (1)");
    ap = input(0, 1);
    wait_ms(300); //keypad debounce

    if (ap == 0){
           ampm = "AM";
       }
    else {
           ampm = "PM";
       }

    clk.set_time(hr, min, sec, ampm); //update clock object to inputted values
}

//defining function for receiving keypad input
int input(int range_low, int range_up){
    bool valid = false; //keep input in range
    string var;         //create string to hold user input
    int var_int;        //create int to hold integer value of var string

 //loop until valid input
    while (valid == false){
        var = ""; //reset to empty input string with each loop
        //loop for displaying + recording keypad presses
        while (keypad_scan() != '#') //# used as the enter key
        {
         if (keypad_scan() != '_'){
                     if (keypad_scan() != '_' && keypad_scan() != '#'){ //print and record input when button other than # pressed
                        lcd.printf("%c", keypad_scan());                //print keypad input to LCD
                        fflush(stdout);

                        var += keypad_scan();                           //record input
                        wait_ms(300);                                   //debounce                          
                    }
                }
        }

        //turning var string into integer
        var_int = str_to_int(var);
        if (range_low <= var_int && var_int <= range_up){ //monitor if input is in accepted range
            valid = true;                                 //end loop
        }
        else {
            lcd.cls();
            lcd.printf("ERROR, TRY AGAIN");
            wait_ms(300);
        }
    }
    return var_int; //return integer input to set function
} 

//manual function to convert string to int
int str_to_int(string num){
    char buffer[10]; //character array to temporarily hold string value
    string num_a[60]; //string array to hold total range accepted

    //initializing string array to hold 0-59 range of numbers
    for (int i = 0; i < 60; i++){
        sprintf(buffer, "%d", i); //for each i, buffer = i
        num_a[i] = buffer;        //for each i, the ith element of the string array = i
    }

    //loop that compares each string in array to input string, returning the integer value of the string when finding a match
    for (int i = 0; i < 60; i++){
        if (num.compare(num_a[i]) == 0){ //compare input string to each element of 0-59 string array
            return i;                    //return integer value to input function
        }
    }
    return 61; //return number outside of each accepted range to input function
}


//function for reading temperature from sensor and storing it in temp array
void tempread(){
    float volt = sens.read();                   //read voltage from sensor **either equals this or needs to be multipleid by ref voltage**
    float temp_c = volt / 0.01;                 //obtain celsius temp from voltage
    float temp_f = (temp_c * (9.0/5.0)) + 32;   //convert cels temp to fahr

    temp.set_temp(temp_c, temp_f);              //update temp object
}

//function for toggling unit when button pressed
void switchunit(){
    char unit = temp.unit_char();

    if (unit == 'F'){   //if current unit is cels, change to fahr
        unit = 'C';
    }
    else {
        unit = 'F';     //if current unit is fahr, change to cels
    }

    temp.set_unit(unit);//update temp object
}


int col_scan(void)
{
    if (COL1 == 0){
        return 0;
    }
    else if (COL2 == 0){
        return 1;
    }
    else if (COL3 == 0){
        return 2;
    }
    else if (COL4 == 0){
        return 3;
    }
    else {
        return 255;
    }
}

char keypad_scan(void)
{
    int row = 0;
    int col;
    char key_map [4][4] = {
        {'1', '2', '3', 'A'}, //1st row
        {'4', '5', '6', 'B'}, //2nd row
        {'7', '8', '9', 'C'}, //3rd row
        {'*', '0', '#', 'D'}, //4th row
    };

    for (int i = 1; i <= 4; i++)
    {
       
        ROW1 = 1;
        ROW2 = 1;
        ROW3 = 1;
        ROW4 = 1;

        switch (i)
        {
            case 1:
                ROW1 = 0;
                break;
            case 2:
                ROW2 = 0;
                break;
            case 3:
                ROW3 = 0;
                break;
            case 4:
                ROW4 = 0;
                break;
        }

        row = i - 1;
        col = col_scan();

        if (col != 255){
            return key_map[row][col];
        }
    }

    return '_';
    };

