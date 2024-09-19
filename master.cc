// Master Code

int clockPin = 13;
int chipSelectPin = 10;
int mosiPin = 11;
int misoPin = 12;

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
  int mosiPin;
  int misoPin;
  int baseDelay;

public:
  // Constructor
  SPI(int clkPin, int csPin, int mosi, int miso, int delayTime) {
    clockPin = clkPin;
    chipSelectPin = csPin;
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
    digitalWrite(chipSelectPin, HIGH);  // Deselect Slave by setting CS HIGH
    digitalWrite(clockPin, LOW);        // Initialize CLOCK to LOW
  }
  int getBaseDelay() {
    return baseDelay;
  }
  // Select the Slave
  void selectSlave() {
    digitalWrite(chipSelectPin, LOW);  // Select the Slave
  }

  // Deselect the Slave
  void deselectSlave() {
    digitalWrite(chipSelectPin, HIGH);  // Deselect the Slave
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

// Instantiate SPI object
SPI spi(clockPin, chipSelectPin, mosiPin, misoPin, baseDelay);

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  spi.begin();         // Initialize SPI pins

  // Get number of bits from user
  Serial.println("Enter the number of bits to send (1-32):");
  while (Serial.available() == 0) {
  }  // Wait for input
  numBits = Serial.parseInt();  // Read number of bits from user
  if (numBits < 1 || numBits > 32) {
    numBits = 8;  // Default to 8 if input is invalid
    Serial.println("Invalid input. Defaulting to 8 bits.");
  }

  // Get binary message from user
  Serial.println("Enter the binary message to send:");
  while (Serial.available() == 0) {
  }                                            // Wait for input
  String input = Serial.readStringUntil('\n');  // Read binary message as a string
  input.trim();                                 // Remove any leading/trailing spaces

  // Convert the binary string to a number
  dataToSend = strtoul(input.c_str(), NULL, 2);

  // Validate the length of the message fits the bit length
  if (input.length() > numBits) {
    Serial.println("Message too long. Truncating to the specified number of bits.");
    dataToSend &= ((1UL << numBits) - 1);  // Truncate message to fit the bit length
  }

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
  spi = SPI(clockPin, chipSelectPin, mosiPin, misoPin, baseDelay);
  spi.begin();  // Re-initialize pins with possibly updated baseDelay
}

void loop() {
  spi.selectSlave();  // Select the Slave

  // First, send numBits to the slave as 8 bits
  spi.sendBits(numBits, 8);

  // Now, send and receive the chosen number of bits
  dataReceived = spi.transfer(dataToSend, numBits);

  spi.deselectSlave();  // Deselect the Slave

  // Print the sent and received data
  Serial.print("Data sent: ");
  Serial.println(dataToSend, BIN);
  Serial.print("Data received: ");
  Serial.println(dataReceived, BIN);
  delay(spi.getBaseDelay()*4);  // Wait before the next communication cycle
}
