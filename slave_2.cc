// Slave Arduino 2 Code (New Slave with Sensor)

int clockPin = 13;
int chipSelectPin = 8; // CS connected to pin 9 on the master
int mosiPin = 11;
int misoPin = 12;
int sensorPin = A0; // Analog pin connected to the sensor (e.g., potentiometer)

unsigned long dataReceived = 0;    // Variable to store received data
unsigned long dataToSend = 0;      // Variable to store sensor data
int numBits = 16;                  // Number of bits to send (adjust as needed)

// SPI_Slave class definition (same as before)
class SPI_Slave {
private:
  int clockPin;
  int chipSelectPin;
  int mosiPin;
  int misoPin;

public:
  // Constructor
  SPI_Slave(int clkPin, int csPin, int mosi, int miso) {
    clockPin = clkPin;
    chipSelectPin = csPin;
    mosiPin = mosi;
    misoPin = miso;
  }

  // Initialize pins
  void begin() {
    pinMode(clockPin, INPUT);
    pinMode(chipSelectPin, INPUT);
    pinMode(mosiPin, INPUT);
    pinMode(misoPin, OUTPUT);
    digitalWrite(misoPin, LOW);  // Initialize MISO to LOW
  }

  // Wait for Slave selection
  void waitForSelect() {
    while (digitalRead(chipSelectPin) == HIGH) {
    }  // Wait for Slave selection
  }

  // Wait for Slave deselection
  void waitForDeselect() {
    while (digitalRead(chipSelectPin) == LOW) {
    }  // Wait for Slave deselection
  }

  // Send and receive bits (full-duplex)
  void transfer(unsigned long dataToSend, unsigned long &dataReceived, int numBits) {
    dataReceived = 0;
    for (int i = numBits - 1; i >= 0; i--) {
      while (digitalRead(clockPin) == HIGH) {
      }  // Wait for CLOCK to go LOW
      digitalWrite(misoPin, (dataToSend >> i) & 0x01);  // Set data on MISO
      while (digitalRead(clockPin) == LOW) {
      }  // Wait for CLOCK to go HIGH
      int bitReceived = digitalRead(mosiPin);  // Read MOSI
      dataReceived |= (bitReceived << i);      // Store the received bit
      while (digitalRead(clockPin) == HIGH) {
      }  // Wait for CLOCK to go LOW
    }
  }
};

// Instantiate SPI_Slave object
SPI_Slave spi(clockPin, chipSelectPin, mosiPin, misoPin);

void setup() {
  spi.begin();           // Initialize SPI pins
  Serial.begin(9600);    // Initialize serial communication
}

void loop() {
  // Read sensor data
  int sensorValue = analogRead(sensorPin); // Read the analog value (0 - 1023)
  dataToSend = sensorValue;                // Prepare data to send

  spi.waitForSelect();  // Wait for Slave selection

  // Send the sensor data to the master
  spi.transfer(dataToSend, dataReceived, numBits);

  spi.waitForDeselect();  // Wait for Slave deselection

  // Print the sent data
  Serial.print("Sensor value sent: ");
  Serial.println(dataToSend);

  delay(100);  // Optional delay before the next cycle
}
