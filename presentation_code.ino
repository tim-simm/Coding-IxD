#include <AccelStepper.h> 
#include "HX711.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define NUM_MOTORS 3


// Define stepper motor pins.
const int dirPin0 = 14;
const int stepPin0 = 32;
const int slpPin0 = 13;

const int dirPin1 = 15;
const int stepPin1 = 33;

const int dirPin2 = 27;
const int stepPin2 = 12;

// Sleep Pin 1 is for both, driver 1 and driver 2 (because there are not enough pins on our arduino).
const int slpPin1 = 21;

// Button used for initializing the rings to most retracted position.
const int button = 26;

// List for managing motors.
AccelStepper motors[NUM_MOTORS] = {
  AccelStepper(1, stepPin0, dirPin0),
  AccelStepper(1, stepPin1, dirPin1),
  AccelStepper(1, stepPin2, dirPin2),
};

// We use 1/16 micro steps. For some reason we had to multiply each step by 2, that's why we have the multiplier 32.
const int stepmultiplier = 32;


// Pins to control the scale.
const int DT_PIN = 25;
const int SCK_PIN = 4;

// Create scale object.
HX711 scale;

// Needs to be measured for each scale. 
const float calibration_factor = -22950; 


// WiFi name and password.
const char* ssid = "see";
const char* password = "KHB-Computerstudio";


// Server to pull data from.
const char* server = "lamit03.userpage.fu-berlin.de/database.php";

// To store db entry.
String survey_data;



// Connect to Wifi. Pull the newest entry from database and safe as the old entry. Calibrate Scale. Set Rings to default position. 
void setup() {
  Serial.begin(9600);
  

  pinMode(button, INPUT);
  pinMode(slpPin0, OUTPUT);
  pinMode(slpPin1, OUTPUT);

  // Put motors in sleep mode while they are not moving, so that they consume less power and ultimately genereate less heat.
  digitalWrite(slpPin0, LOW);
  digitalWrite(slpPin0, LOW);

  // Connect to WiFi defined by ssid.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi ...");
  }
  Serial.println("Connected to WiFi.");


  // Catch old entry of db.
  bool dataFetched = false;
  while(WiFi.status() == WL_CONNECTED && !dataFetched){
    
    HTTPClient http;
  
    String url = "https://" + String(server);

    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        survey_data = http.getString();
        dataFetched = true;
      }
    } 
    else {
      Serial.println("Unexpected HTTP response code: " + String(httpCode) + "Sending default value [2,44,44].");
      survey_data = "[2,44,44]";
      dataFetched = true;
    }

    http.end();
  }

  // Redundancy check.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost.");
  }


  // Calibrate the scale.
  scale.begin(DT_PIN, SCK_PIN); 
  scale.set_scale(calibration_factor);
  
  // Reset the scale to 0. 
  scale.tare();                         

  
  // adjust Maxspeed to desired speed.
  // Extremly high acceleration, so that we instantly move at max speed.
  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].setMaxSpeed(90 * stepmultiplier);
    motors[i].setAcceleration(200 * stepmultiplier);
  }


  // Initialise the rings to the most retracted position.
  int RingCount = 0;
  Serial.println("Initialising... ");
  while (RingCount < NUM_MOTORS) { 
    
    if (digitalRead(button)){
      Serial.print("Moving ring: ");
      Serial.println(RingCount);

      // Wake motors up to move them.
      digitalWrite(slpPin0, HIGH);
      digitalWrite(slpPin1, HIGH);
      delay(100);

      motors[RingCount].moveTo(-100000);  // Move them counterclockwise.
      while (digitalRead(button)){        // A different approach would use the sleep pin of the a4988 to turn the motors off and enable moving the rings by hand.
        motors[RingCount].run();
      }

      digitalWrite(slpPin0, LOW);
      digitalWrite(slpPin1, LOW);
      delay(100);

      Serial.print("Current position of ring ");
      Serial.print(RingCount);
      Serial.println(" is zero (fully retracted).");
      RingCount++;    
    }
    delay(1000);
  }
  
  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].setCurrentPosition(0);
    motors[i].setMaxSpeed(100 * stepmultiplier);
    motors[i].setAcceleration(20000);
  }

  Serial.println("Initialisization of all rings finished.");
  Serial.println("");
  Serial.println("");
  Serial.println("");
}


// Main loop that allows multiple users to use the object one after the other.
void loop(){
  
  if(WiFi.status() == WL_CONNECTED) {

    setRingsZero();

    readScales();
    
    readQuestionnaire();
    
    parseStringToArray();

    calculateRingMovement();

    moveRings();

  } else {
    Serial.println("WiFi connection lost.");
  }
}


// Move motors back to default (retracted) position.
void setRingsZero(){
  Serial.println("Moving Rings back into default position...");

  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].moveTo(0);
  }

  runMotors();
}


// Read scales input over 5 seconds, calculate the average and save it globally.
float averageWeight = 0;

void readScales(){

  scale.tare();

  bool scalesRead = false;                       
  
  while (!scalesRead) {
    unsigned long startTime = millis();
    unsigned long elapsedTime = 0; 
    int measurementsCount = 0; 
    float totalValue = 0.0; 

    // Only start measuring when the registered weight is above 20, indicating someone is standing on the scale.
    if (scale.get_units() >= 20) { // 
      Serial.println("Reading scales input ...");

        while (elapsedTime < 10000) {
          float inputValue = scale.get_units(); 
          totalValue += inputValue; 
          measurementsCount++; 
          delay(100); 
          elapsedTime = millis() - startTime;
        }

      averageWeight = totalValue / measurementsCount; 

      scalesRead = true;
    }
  }  
}


// Indicates when somoething went wrong with receiving the data from the database. Is set in readQuestionnaire and read in parseStringToArray.
bool unexpectedHttp = false;

// Pull newest entry from the database and compare it with the old value. If we have something new, save it and break out of loop.
void readQuestionnaire(){

  bool new_data = false;
  while(!new_data){
    
    HTTPClient http;
  
    String url = "https://" + String(server);

    http.begin(url);

    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        Serial.println("Request...");
        String payload = http.getString();
        
        if (survey_data != payload) {
          survey_data = payload;
          Serial.println(survey_data);
          new_data = true;
        }
      }
    } 
    else {
      Serial.println("Unexpected HTTP response code: " + String(httpCode) + "Sending default value [2,44,44].");
      unexpectedHttp = true;
      new_data = true;
    }

    http.end();

    delay(2000);
  }
}


// HTTP request returns a string of the form "[x,y,z]", so we have to extract the data.
int dataArray[3];

void parseStringToArray() {
  String workingString;
  if (unexpectedHttp) {
    workingString = "[2,44,44]";  
  } else {
    workingString = survey_data;
  }
  
  workingString.remove(0, 1);                       // Remove first "["-character.
  workingString.remove(workingString.length() - 1); // Remove last "]"-character.

  // Extract numbers and save in array
  int index = 0;
  while (workingString.length() > 0) {
    int commaPos = workingString.indexOf(',');
    
    if (commaPos == -1) {
      dataArray[index] = workingString.toInt();
      break;
    } else {
      String numberStr = workingString.substring(0, commaPos);
      dataArray[index] = numberStr.toInt();
      workingString.remove(0, commaPos + 1);
      index++;
    }
  }
}


// Uses averageWeight and body height (dataArray[2]) to calculate BMI. 
// Then uses BMI and personalityType (dataArray[0]) to determine Eigenform.
#define BMI_UNDER 0
#define BMI_MEDIUM 1
#define BMI_OVER 2

int Eigenform = -1;
void calculateRingMovement(){
  
  // Body weight / (body height ** 2)
  float bmi = averageWeight / (dataArray[2] * dataArray[2]);
  int personalityBMI;
  
  if (bmi <= 18.5) {
    personalityBMI = BMI_UNDER;
  }else if (bmi > 18.5 && bmi <= 25) {
    personalityBMI = BMI_MEDIUM;
  }else if (bmi > 25) {
    personalityBMI = BMI_OVER;
  }else {
    Serial.println("Something went wrong while mapping the BMI.");
    personalityBMI = BMI_MEDIUM;
  }

  int personalityType = dataArray[0];
  
  // use mapping to determine Eigenform
  if (personalityBMI == BMI_UNDER) {
    if (personalityType == 0) {
      Eigenform = 0;
    } else if (personalityType == 1) {
      Eigenform = 1;
    } else if (personalityType == 2) {
      Eigenform = 2;
    }
  } else if (personalityBMI == BMI_MEDIUM) {
    if (personalityType == 0) {
      Eigenform = 3;
    } else if (personalityType == 1) {
      Eigenform = 4;
    } else if (personalityType == 2) {
      Eigenform = 5;
    }
  } else if (personalityBMI == BMI_OVER) {
    if (personalityType == 0) {
      Eigenform = 6;
    } else if (personalityType == 1) {
      Eigenform = 7;
    } else if (personalityType == 2) {
      Eigenform = 8;
    }
  } else {
    Serial.print("Something went wrong while mapping the Eigenform. Eigenform value: ");
    Serial.println(Eigenform);
  }
}



// 26 is the maximum extended angle they can move to.
// 0 is the most retracted angle they can move to.

#define CLOSE 0 * stepmultiplier
#define MEDIUM 13 * stepmultiplier
#define OPEN 26 * stepmultiplier

void moveRings(){
  
  switch (Eigenform) {
    case 0:
    motors[0].moveTo(CLOSE);
    motors[1].moveTo(MEDIUM);
    motors[2].moveTo(OPEN);
      break;
    case 1:
    motors[0].moveTo(MEDIUM);
    motors[1].moveTo(MEDIUM);
    motors[2].moveTo(CLOSE);
      break;
    case 2:
    motors[0].moveTo(CLOSE);
    motors[1].moveTo(CLOSE);
    motors[2].moveTo(OPEN);
      break;
    case 3:
    motors[0].moveTo(OPEN);
    motors[1].moveTo(MEDIUM);
    motors[2].moveTo(CLOSE);
      break;
    case 4:
    motors[0].moveTo(MEDIUM);
    motors[1].moveTo(MEDIUM);
    motors[2].moveTo(OPEN);
      break;
    case 5:
    motors[0].moveTo(CLOSE);
    motors[1].moveTo(CLOSE);
    motors[2].moveTo(MEDIUM);
      break;
    case 6:
    motors[0].moveTo(OPEN);
    motors[1].moveTo(CLOSE);
    motors[2].moveTo(MEDIUM);
      break;
    case 7:
    motors[0].moveTo(MEDIUM);
    motors[1].moveTo(MEDIUM);
    motors[2].moveTo(MEDIUM);
      break;
    case 8:
    motors[0].moveTo(CLOSE);
    motors[1].moveTo(CLOSE);
    motors[2].moveTo(CLOSE);
      break;
    default:
      // Handle unexpected values if necessary
      Serial.println("Unmapped value for Eigeform. Value: ");
      Serial.println(Eigenform);
      break;
  }

  runMotors();

  Serial.print("Eigenform: ");
  Serial.println(Eigenform);

  Serial.println("Moving rings based on input. Holding position.");
  while (scale.get_units() >= 10) {
    delay(100);
  }
  delay(1000);
}


// Create separate function for this, because it's pretty ugly.
void runMotors() {

  // Wake motors up.
  digitalWrite(slpPin0, LOW);
  digitalWrite(slpPin1, LOW);

  // Move motors.
  while (motors[0].distanceToGo() != 0 || motors[1].distanceToGo() != 0 || motors[2].distanceToGo() != 0) {
    for (int i = 0; i < NUM_MOTORS; i++) {
      motors[i].run();
    } 
  }

  // Put motors back to sleep.
  digitalWrite(slpPin0, HIGH);
  digitalWrite(slpPin1, HIGH);
}


// Call this to attract attention.
void eyeCandy(){
  Serial.println("Eyecandy ... ");

  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].moveTo(OPEN);
  }


  while (motors[0].distanceToGo() != 0) {
  motors[0].run();
  }

  delay(500);
  
  while (motors[1].distanceToGo() != 0) {
  motors[1].run();
  }
  
  delay(500);
  
  motors[0].moveTo(CLOSE);
  while (motors[0].distanceToGo() != 0) {
  motors[0].run();
  }  
  
  delay(500);

  while (motors[2].distanceToGo() != 0) {
  motors[2].run();
  }

  delay(500);

  motors[1].moveTo(CLOSE);
  while (motors[1].distanceToGo() != 0) {
  motors[1].run();
  }

  delay(500);

  motors[2].moveTo(CLOSE);
  while (motors[2].distanceToGo() != 0) {
  motors[2].run();
  }

}


// Testing function
void loop2() {
  eyeCandy();
}