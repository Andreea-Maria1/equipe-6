#include <Adafruit_Fingerprint.h>
#include <time.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
SoftwareSerial mySerial(19, 18);  // pin #2 is IN from sensor, pin #3 is OUT to Arduino
#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
#define mySerial Serial1  // #0 is green wire, #1 is white
#endif

class FingerprintSensor {
  private:
    Adafruit_Fingerprint finger;

  public:
    FingerprintSensor() : finger(&mySerial) {}

    void setupSensor() {
      Serial.begin(9600);
      while (!Serial);  // For Yun/Leo/Micro/Zero/...
      delay(100);
      Serial.println("\n\nAdafruit finger detect test");

      // set the data rate for the sensor serial port
      finger.begin(57600);
      delay(5);
      if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
      } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) { delay(1); }
      }
    }

    int fingerTest() {
      uint8_t p = finger.getImage();  // Capture fingerprint image
      if (p != FINGERPRINT_OK) {
        Serial.print(".");
        return false;
      }

      p = finger.image2Tz();  // Convert image to template
      if (p != FINGERPRINT_OK) {
        Serial.println("Failed to convert image");
        return false;
      }

      p = finger.fingerSearch();  // Search for the finger in the database
      if (p == FINGERPRINT_OK) {
        Serial.println("Fingerprint found!");

        return finger.fingerID;
      } else {
        Serial.println("Fingerprint not found");
        return false;
      }
    }

    void eraseAll() {
      Serial.println("Erasing all fingerprints...");
      uint8_t p = finger.emptyDatabase();
      if (p == FINGERPRINT_OK) {
        Serial.println("All fingerprints erased successfully.");
      } else {
        Serial.println("Failed to erase fingerprints.");
      }

      Serial.println(finger.getTemplateCount());
    }

    uint8_t getFingerprintEnroll(int id) {

      int p = -1;
      Serial.print("Waiting for valid finger to enroll as #"); 
      Serial.println(id);
      while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
        }
      }

      // OK success!

      p = finger.image2Tz(1);
      switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image converted");
          break;
        case FINGERPRINT_IMAGEMESS:
          Serial.println("Image too messy");
          return p;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          return p;
        case FINGERPRINT_FEATUREFAIL:
          Serial.println("Could not find fingerprint features");
          return p;
        case FINGERPRINT_INVALIDIMAGE:
          Serial.println("Could not find fingerprint features");
          return p;
        default:
          Serial.println("Unknown error");
          return p;
      }

      Serial.println("Remove finger");
      delay(2000);
      p = 0;
      while (p != FINGERPRINT_NOFINGER) {
        p = finger.getImage();
      }
      Serial.print("ID "); Serial.println(id);
      p = -1;
      Serial.println("Place same finger again");
      while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
        }
      }

      // OK success!

      p = finger.image2Tz(2);
      switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image converted");
          break;
        case FINGERPRINT_IMAGEMESS:
          Serial.println("Image too messy");
          return p;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          return p;
        case FINGERPRINT_FEATUREFAIL:
          Serial.println("Could not find fingerprint features");
          return p;
        case FINGERPRINT_INVALIDIMAGE:
          Serial.println("Could not find fingerprint features");
          return p;
        default:
          Serial.println("Unknown error");
          return p;
      }

      // OK converted!
      Serial.print("Creating model for #");  Serial.println(id);

      p = finger.createModel();
      if (p == FINGERPRINT_OK) {
        Serial.println("Prints matched!");
      } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
      } else if (p == FINGERPRINT_ENROLLMISMATCH) {
        Serial.println("Fingerprints did not match");
        return p;
      } else {
        Serial.println("Unknown error");
        return p;
      }

      Serial.print("ID "); Serial.println(id);
      p = finger.storeModel(id);
      if (p == FINGERPRINT_OK) {
        Serial.println("Stored!");
      } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
      } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not store in that location");
        return p;
      } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
        return p;
      } else {
        Serial.println("Unknown error");
        return p;
      }

      return id;
    }
};
