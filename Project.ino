#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#include <Stepper.h>

// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Define dust sensor pins
const int dustSensorPin1 = A0;
const int dustSensorPin2 = A1;

const int stepesperrevolution = 200;
// Define stepper motor pins
const int motor1Pins[] = {22, 23, 24, 25}; 
const int motor2Pins[] = {8, 9, 10, 11}; 

// Define dust threshold
#define DUST_THRESHOLD 500

// Define Bluetooth module pins
const int BT_RX = 10;
const int BT_TX = 11;
SoftwareSerial BTSerial(BT_RX, BT_TX);

// Define DHT sensor type and pin
#define DHTTYPE DHT11   // DHT 11 sensor
const int DHTPIN = 7;  // Digital pin connected to the DHT sensor

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize stepper motors
Stepper motor1(STEPS_PER_REVOLUTION, motor1Pins[0], motor1Pins[1], motor1Pins[2], motor1Pins[3]);
Stepper motor2(STEPS_PER_REVOLUTION, motor2Pins[0], motor2Pins[1], motor2Pins[2], motor2Pins[3]);

// Flag to control sensor readings and manual control
bool sensorEnabled = true; // Start with sensor readings and manual control enabled

void setup() {
  // Setup dust sensor pins
  pinMode(dustSensorPin1, INPUT);
  pinMode(dustSensorPin2, INPUT);

  // Setup LCD
  lcd.begin(16, 2); // Change to 2x16 LCD

  // Setup Bluetooth module
  BTSerial.begin(9600);

  // Initialize DHT sensor
  dht.begin();

  // Display welcome message
  lcd.print("Dust Sensor System");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (sensorEnabled) {
    // Read dust sensor values
    int dustSensorValue1 = analogRead(dustSensorPin1);
    int dustSensorValue2 = analogRead(dustSensorPin2);

    // Control gates (now stepper motor) based on dust sensor readings
    if (dustSensorValue1 < DUST_THRESHOLD) {
      // Close gate 1 (rotate stepper motor clockwise)
      motor1.step(STEPS_PER_REVOLUTION / 2); 
    } else {
      // Open gate 1 (rotate stepper motor counter-clockwise)
      motor1.step(-STEPS_PER_REVOLUTION / 2); 
    }

    if (dustSensorValue2 < DUST_THRESHOLD) {
      // Close gate 2 (rotate stepper motor clockwise)
      motor2.step(STEPS_PER_REVOLUTION / 2); 
    } else {
      // Open gate 2 (rotate stepper motor counter-clockwise)
      motor2.step(-STEPS_PER_REVOLUTION / 2); 
    }

    // Read humidity from DHT sensor
    float humidity = dht.readHumidity();

    // Display values and gate states on LCD
    lcd.setCursor(0, 0);
    lcd.print("Sensor1: ");
    lcd.print(dustSensorValue1);
    lcd.setCursor(0, 1);
    lcd.print("Motor1: ");
    lcd.print((dustSensorValue1 < DUST_THRESHOLD) ? "Close" : "Open");

    lcd.setCursor(0, 0);
    lcd.print("Sensor2: ");
    lcd.print(dustSensorValue2);
    lcd.setCursor(0, 1);
    lcd.print("Motor2: ");
    lcd.print((dustSensorValue2 < DUST_THRESHOLD) ? "Close" : "Open");

    // Display humidity on LCD
    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
  }

  // Check for Bluetooth commands
  if (BTSerial.available() > 0) {
    char command = BTSerial.read();

    // Handle commands
    switch (command) {
      // ... (rest of the commands remain the same)
      case 'm': // Stepper motor control
        // Read next command for direction (clockwise or counter-clockwise)
        command = BTSerial.read();
        if (command == 'c') {
          // Clockwise
          command = BTSerial.read();
          int steps = command - '0';
          motor1.step(steps);
        } else if (command == 'a') {
          // Counter-clockwise
          command = BTSerial.read();
          int steps = command - '0';
          motor1.step(-steps);
        } else {
          break;
        }
        break;
    }
  }

  // Delay for a second before reading values again
  delay(1000);
}