#include <Wire.h>
FingerprintSensor sensor;
KeyPadInterface keyInterface;



// 0 : test
// 1 : fill
// 2 : code admin
// 3 : New user

int state = 0;
int nUsers = 0;

int myArray[100];

int arr = 0;

char code[5];  // Stocke le code entré (4 chiffres + '\0')
byte index = 0;  // Position actuelle dans l'entrée utilisateur
const char correctCode[] = "1234";  // Code correct à tester


void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  for (int i = 0; i < 100; i++) {
        myArray[i] = 1;
    }
  myArray[0] = -1;

  sensor.setupSensor();
  sensor.eraseAll();

  Wire.begin(); // Initialize I2C as master
}

// Wrapper function to send state
void sendState(byte state) {
  Wire.beginTransmission(8); // Address of the slave (receiver)
  Wire.write(state);         // Send state as a byte (0-3)
  Wire.endTransmission();    // End transmission
}


void loop() {

  char key = keyInterface.get();
  if(key == '*'){
    if(state == 0){
      state = 2;
    }
    else{
      state = 0;
    }
    //Serial.println(state);
  }
  if (state == 0){
    arr = sensor.fingerTest();
    //Serial.println(arr);
    if (myArray[arr] == 1){
      state = 1;
    }
    else if (myArray[arr] == 0){
      Serial.println("already fill");
    }
  }
  else if (state == 1){
    myArray[arr] = 0;
    digitalWrite(8, LOW);  // turn the LED on (HIGH is the voltage level)
    delay(800);                      // wait for a second
    digitalWrite(8, HIGH);
    state = 0;
  }
  else if (state == 2) {
    Serial.println("Entrer code admin");
        // Lecture du code sur le clavier
        if (key >= '0' && key <= '9') {  // Vérifie si c'est un chiffre
            Serial.print(key);
            code[index] = key;
            index++;

            // Vérifier si 4 chiffres sont entrés
            if (index == 4) {
                code[4] = '\0';  // Ajouter fin de chaîne

                if (strcmp(code, correctCode) == 0) {
                    Serial.println("\n Code correct !");
                    state = 3;  // Passe à l'étape suivante
                } else {
                    Serial.println("\n Code incorrect !");
                    state = 0;  // Retour au début
                }
                index = 0;  // Réinitialiser l'entrée
            }
        }
    } 
  else{
    if(sensor.getFingerprintEnroll(nUsers + 1)){\
      nUsers++;
      state = 0;
    }
    Serial.println(nUsers);

  }

  delay(50);
}