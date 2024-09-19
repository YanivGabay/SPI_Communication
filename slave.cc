// Slave Code

int clockPin = 13;
int chipSelectPin = 10;
int mosiPin = 11;
int misoPin = 12;

unsigned long dataReceived = 0;      // Variable to store received data
unsigned long dataToSend = 0b01010101;  // Default data to send back
int numBits = 8;                     // Default number of bits

// SPI_Slave class definition
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

  // Receive bits from the master
  unsigned long receiveBits(int numBits) {
    unsigned long receivedData = 0;
    for (int i = numBits - 1; i >= 0; i--) {
      while (digitalRead(clockPin) == LOW) {
      }  // Wait for CLOCK to go HIGH
      int bitReceived = digitalRead(mosiPin);   // Read MOSI
      receivedData |= (bitReceived << i);       // Store the received bit
      while (digitalRead(clockPin) == HIGH) {
      }  // Wait for CLOCK to go LOW
    }
    return receivedData;
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
  spi.waitForSelect();  // Wait for Slave selection

  // First, receive numBits from the master (assumed to be sent as 8 bits)
  int receivedNumBits = spi.receiveBits(8);
  numBits = receivedNumBits;  // Set numBits to the received value

  // Validate numBits
  if (numBits < 1 || numBits > 32) {
    numBits = 8;  // Default to 8 if received value is invalid
    Serial.println("Invalid numBits received. Defaulting to 8 bits.");
  }
  Serial.print("Number of bits to receive: ");
  Serial.println(numBits);

  // Now, receive and send the chosen number of bits
  spi.transfer(dataToSend, dataReceived, numBits);

  spi.waitForDeselect();  // Wait for Slave deselection

  // Print the received data
  Serial.print("Data received: ");
  Serial.println(dataReceived, BIN);
  delay(100);  // Optional delay before the next cycle
}
