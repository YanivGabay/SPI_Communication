// Master Code with SPI and Potentiometer Classes

const int SLAVE_1 = 10;         // Not used in this example
const int SLAVE_2_POTE = 9;     // CS pin for the Potentiometer device

int clockPin = 13;
int chipSelectPin = SLAVE_2_POTE; // CS pin for the Potentiometer
int mosiPin = 11;
int misoPin = 12;

// Global variables
int baseDelay = 250;             // Base delay in microseconds (parameterized)

// SPI class definition
class SPI {
private:
  int clockPin;
  int chipSelectPin;
  int mosiPin;
  int misoPin;
  int baseDelay;
  int numBits;

public:
  // Constructor
  SPI(int clkPin, int csPin, int mosi, int miso, int delayTime, int bits)
      : clockPin(clkPin), chipSelectPin(csPin),
        mosiPin(mosi), misoPin(miso), baseDelay(delayTime), numBits(bits) {}

  // Initialize pins
  void begin() {
    pinMode(clockPin, OUTPUT);
    pinMode(chipSelectPin, OUTPUT);
    pinMode(mosiPin, OUTPUT);
    pinMode(misoPin, INPUT);
    digitalWrite(chipSelectPin, HIGH);  // Deselect Slave by setting CS HIGH
    digitalWrite(clockPin, LOW);        // Initialize CLOCK to LOW
  }

  int getBaseDelay() {
    return baseDelay;
  }

  void setBaseDelay(int delayTime) {
    baseDelay = delayTime;
  }

  void setNumBits(int bits) {
    numBits = bits;
  }

  // Select the Slave
  void selectSlave() {
    digitalWrite(chipSelectPin, LOW);    // Select Slave
  }

  // Deselect the Slave
  void deselectSlave() {
    digitalWrite(chipSelectPin, HIGH);   // Deselect Slave
  }

  // Send bits to the slave
  void sendBits(unsigned long data) {
    for (int i = numBits - 1; i >= 0; i--) {
      digitalWrite(clockPin, LOW);                              // Ensure clock is LOW
      digitalWrite(mosiPin, (data >> i) & 0x01);                // Set data on MOSI
      delayMicroseconds(baseDelay);                             // Adjusted delay
      digitalWrite(clockPin, HIGH);                             // Clock goes HIGH
      delayMicroseconds(baseDelay);                             // Adjusted delay
      digitalWrite(clockPin, LOW);                              // Clock goes LOW for next bit
    }
  }
};

// Potentiometer class definition
class Potentiometer {
private:
  SPI& spi; // Reference to the SPI object

public:
  Potentiometer(SPI& spiObject) : spi(spiObject) {}

  void init() {
    Serial.println("Initializing Potentiometer...");
    spi.setNumBits(16); // Always use 16 bits for Potentiometer
  }

  void sendCommand(unsigned long message) {
    // Send the command via SPI
    spi.selectSlave(); // Select the Potentiometer device
    spi.sendBits(message);
    spi.deselectSlave();

    Serial.println("Command sent to Potentiometer.");
  }
};

Potentiometer* pot; // Declare pointer to Potentiometer object

void setup() {
  Serial.begin(9600);  // Initialize serial communication

  // Instantiate SPI object
  SPI* spi = new SPI(clockPin, chipSelectPin, mosiPin, misoPin, baseDelay, 16); // numBits set to 16
  spi->begin();

  // Instantiate Potentiometer object
  pot = new Potentiometer(*spi);
  pot->init();          // Initialize Potentiometer

  // Get base delay from user
  Serial.println("Enter the base delay in microseconds (e.g., 250):");
  while (Serial.available() == 0) {
    // Wait for input
  }
  baseDelay = Serial.parseInt();  // Read base delay from user
  if (baseDelay <= 0) {           // Validate baseDelay
    baseDelay = 250;              // Default to 250 us if input is invalid
    Serial.println("Invalid input. Defaulting to 250 microseconds.");
  }

  // Update the base delay in the SPI object
  spi->setBaseDelay(baseDelay);
}

void loop() {
  // Display the menu
  Serial.println("Choose command:");
  Serial.println("1: Write data");
  Serial.println("2: Shutdown");
  int cmdChoice = 0;
  while (cmdChoice != 1 && cmdChoice != 2) {
    Serial.println("Enter 1 or 2:");
    while (Serial.available() == 0) {
      // Wait for input
    }
    cmdChoice = Serial.parseInt();
  }

  // Choose potentiometer
  Serial.println("Choose potentiometer:");
  Serial.println("1: Potentiometer 0");
  Serial.println("2: Potentiometer 1");
  Serial.println("3: Both potentiometers");
  int potChoice = 0;
  while (potChoice < 1 || potChoice > 3) {
    Serial.println("Enter 1, 2, or 3:");
    while (Serial.available() == 0) {
      // Wait for input
    }
    potChoice = Serial.parseInt();
  }

  // If Write data, get 8-bit data
  unsigned int dataBits = 0;
  if (cmdChoice == 1) { // Write data
    Serial.println("Enter 8-bit data (as binary):");
    String dataInput;
    while (true) {
      while (Serial.available() == 0) {
        // Wait for input
      }
      dataInput = Serial.readStringUntil('\n');
      dataInput.trim();
      if (dataInput.length() != 8) {
        Serial.println("Invalid input length. Expected 8 bits.");
      } else {
        bool validData = true;
        for (int i = 0; i < 8; i++) {
          if (dataInput.charAt(i) != '0' && dataInput.charAt(i) != '1') {
            validData = false;
            break;
          }
        }
        if (validData) {
          dataBits = strtoul(dataInput.c_str(), NULL, 2);
          break;
        } else {
          Serial.println("Invalid input. Please enter 8 bits of binary data.");
        }
      }
    }
  } else {
    // Shutdown command, data bits can be zeros
    dataBits = 0;
  }

  // Construct the 16-bit message
  unsigned long message = 0;

  // Bits 1-2: Don't care (we can set them to zero)
  message <<= 2; // Shift left 2 bits

  // Bits 3-4: Command bits
  if (cmdChoice == 1) { // Write data
    message = (message << 2) | 0b01;
  } else if (cmdChoice == 2) { // Shutdown
    message = (message << 2) | 0b10;
  }

  // Bits 5-6: Don't care
  message <<= 2; // Shift left 2 bits

  // Bits 7-8: Potentiometer selection bits
  if (potChoice == 1) { // Potentiometer 0
    message = (message << 2) | 0b01;
  } else if (potChoice == 2) { // Potentiometer 1
    message = (message << 2) | 0b10;
  } else if (potChoice == 3) { // Both potentiometers
    message = (message << 2) | 0b11;
  }

  // Bits 9-16: Data bits
  message = (message << 8) | (dataBits & 0xFF);

  // Print the constructed message
  Serial.print("Constructed 16-bit message: ");
  Serial.println(message, BIN);

  // Send the command via Potentiometer class
  pot->sendCommand(message);

  delay(500);  // Wait before the next communication cycle
}
