#include <Keypad.h>

class KeyPadInterface{
  public:
    KeyPadInterface() : customKeypad(Keypad(makeKeymap(hexaKeys), rowPins, colPins, 4, 4)){}

    char get(){
      return customKeypad.getKey();
    }
  
  private:
    Keypad customKeypad;
    //const byte ROWS = 4; 
    //const byte COLS = 4; 

    
    byte rowPins[4] = {23, 25, 27, 29}; 
    byte colPins[4] = {31, 33, 35, 37}; 

    char hexaKeys[4][4] = {
      {'1', '2', '3', 'A'},
      {'4', '5', '6', 'B'},
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'}
    };
};