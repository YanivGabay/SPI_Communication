// Master Code

const int SLAVE_1 = 10;
const int SLAVE_2_POTE = 9;

int clockPin = 13;
int chipSelectPin = SLAVE_1;
int mosiPin = 11;
int misoPin = 12;
int chipSelectPin2 = SLAVE_2_POTE;
int slaveNumber;
// Global variables
unsigned long dataToSend = 0;    // Variable to store the user-defined message
unsigned long dataReceived = 0;  // Variable to store received data
int numBits = 8;                 // Default number of bits (can be changed)
int baseDelay = 250;             // Base delay in milliseconds (parameterized)



// SPI class definition
class SPI {
private:
  int clockPin;
  int chipSelectPin;
  int chipSelectPin2;
  int mosiPin;
  int misoPin;
  int baseDelay;

public:
  // Constructor
  SPI(int clkPin, int csPin,int cs_2pin, int mosi, int miso, int delayTime) {
    clockPin = clkPin;
    chipSelectPin = csPin;
    chipSelectPin2 = cs_2pin;
    mosiPin = mosi;
    misoPin = miso;
    baseDelay = delayTime;
  }

  // Initialize pins
  void begin() {
    pinMode(clockPin, OUTPUT);
    pinMode(chipSelectPin, OUTPUT);
    pinMode(mosiPin, OUTPUT);
    pinMode(misoPin, INPUT);
    digitalWrite(chipSelectPin2,HIGH); // Deselect Slave by setting CS HIGH
    digitalWrite(chipSelectPin, HIGH);  // Deselect Slave by setting CS HIGH
    digitalWrite(clockPin, LOW);        // Initialize CLOCK to LOW
  }
  int getBaseDelay() {
    return baseDelay;
  }
  // Select the Slave
  void selectSlave(int slaveNumber) {
     if (slaveNumber == 1) {
      digitalWrite(chipSelectPin, LOW);  // Select Slave 1
      digitalWrite(chipSelectPin2, HIGH); // Deselect Slave 2
    } else if (slaveNumber == 2) {
      digitalWrite(chipSelectPin, HIGH); // Deselect Slave 1
      digitalWrite(chipSelectPin2, LOW);  // Select Slave 2
    }
  }

  // Deselect the Slave
void deselectAllSlaves() {
    digitalWrite(chipSelectPin, HIGH); // Deselect Slave 1
    digitalWrite(chipSelectPin2, HIGH); // Deselect Slave 2
  }

  // Send bits to the slave
  void sendBits(unsigned long data, int numBits) {
    for (int i = numBits - 1; i >= 0; i--) {
      digitalWrite(clockPin, LOW);  // Ensure clock is LOW
      digitalWrite(mosiPin, (data >> i) & 0x01);  // Set data on MOSI
      delay(baseDelay);                           // Adjusted delay
      digitalWrite(clockPin, HIGH);               // Clock goes HIGH
      delay(baseDelay);                           // Adjusted delay
      digitalWrite(clockPin, LOW);                // Clock goes LOW for next bit
    }
  }

  // Send and receive bits (full-duplex)
  unsigned long transfer(unsigned long dataToSend, int numBits) {
    unsigned long dataReceived = 0;
    for (int i = numBits - 1; i >= 0; i--) {
      digitalWrite(clockPin, LOW);  // Ensure clock is LOW
      digitalWrite(mosiPin, (dataToSend >> i) & 0x01);  // Set data on MOSI
      delay(baseDelay);                                   // Adjusted delay
      digitalWrite(clockPin, HIGH);                       // Clock goes HIGH
      delay(baseDelay);                                   // Adjusted delay
      int bitReceived = digitalRead(misoPin);             // Read MISO
      dataReceived |= (bitReceived << i);                 // Store the received bit
      digitalWrite(clockPin, LOW);                        // Clock goes LOW for next bit
    }
    return dataReceived;
  }
};

class DAC {
public:
  void init() {
    Serial.println("Initializing DAC...");
  }

  void read() {
    Serial.println("Hello read from DAC.");
  }

  void write() {
    Serial.println("Hello write from DAC.");
  }
};

// ADC class definition
class ADC {
public:
  void init() {
    Serial.println("Initializing ADC...");
  }

  void read() {
    Serial.println("Hello read from ADC.");
  }

  void write() {
    Serial.println("Hello write from ADC.");
  }
};


// Instantiate SPI object
SPI spi(clockPin, chipSelectPin,chipSelectPin2, mosiPin, misoPin, baseDelay);
DAC dac;
ADC adc;

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  //spi.begin();   
  dac.init();
  adc.init();      // Initialize SPI pins

  // Get base delay from user
  Serial.println("Enter the base delay in milliseconds (e.g., 250):");
  while (Serial.available() == 0) {
  }  // Wait for input
  baseDelay = Serial.parseInt();  // Read base delay from user
  if (baseDelay <= 150) {           // Validate baseDelay
    baseDelay = 250;              // Default to 250 ms if input is invalid
    Serial.println("Invalid input. Defaulting to 250 ms.");
  }

  // Re-initialize SPI with the new delay
  spi = SPI(clockPin, chipSelectPin,chipSelectPin2 ,mosiPin, misoPin, baseDelay);
  spi.begin();  // Re-initialize pins with possibly updated baseDelay
}

void loop() {
    Serial.println("Enter command (e.g., WR_ADC, READ_ADC, WR_DAC, READ_DAC):");
  while (Serial.available() == 0) {
    // Wait for input
  }
  String command = Serial.readStringUntil('\n');
  command.trim();  // Remove leading/trailing whitespace

   if (command == "WR_ADC") {
    adc.write();
  } else if (command == "READ_ADC") {
    adc.read();
  } else if (command == "WR_DAC") {
    dac.write();
  } else if (command == "READ_DAC") {
    dac.read();
  } else {
    Serial.println("Invalid command.");
  }
  delay(spi.getBaseDelay()*4);  // Wait before the next communication cycle
}
