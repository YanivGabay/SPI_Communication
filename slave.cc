int clockPin = 13;
int chipSelectPin = 10;
int mosiPin = 11;
int misoPin = 12;

unsigned long dataReceived = 0;  // Variable to store received data
unsigned long dataToSend = 0b01010101;  // Default data to send back
int numBits = 8;  // Default number of bits

void setup() {
  pinMode(clockPin, INPUT);
  pinMode(chipSelectPin, INPUT);
  pinMode(mosiPin, INPUT);
  pinMode(misoPin, OUTPUT);
  digitalWrite(misoPin, LOW);  // Initialize MISO to LOW
  Serial.begin(9600);          // Initialize serial communication
}

void loop() {
  while (digitalRead(chipSelectPin) == HIGH) {}  // Wait for Slave selection

  // First, receive numBits from the master (assumed to be sent as 8 bits)
  int receivedNumBits = 0;
  for (int i = 7; i >= 0; i--) {
    while (digitalRead(clockPin) == LOW) {}  // Wait for CLOCK to go HIGH
    int bitReceived = digitalRead(mosiPin);  // Read MOSI
    receivedNumBits |= (bitReceived << i);  // Store the received bit
    while (digitalRead(clockPin) == HIGH) {}  // Wait for CLOCK to go LOW
  }
  numBits = receivedNumBits;  // Set numBits to the received value
  // Validate numBits
  if (numBits < 1 || numBits > 32) {
    numBits = 8;  // Default to 8 if received value is invalid
    Serial.println("Invalid numBits received. Defaulting to 8 bits.");
  }
  Serial.print("Number of bits to receive: ");
  Serial.println(numBits);

  // Now, receive and send the chosen number of bits
  dataReceived = 0;  // Clear the received data for actual data reception
  for (int i = numBits - 1; i >= 0; i--) {
    while (digitalRead(clockPin) == HIGH) {}  // Wait for CLOCK to go LOW
    digitalWrite(misoPin, (dataToSend >> i) & 0x01);  // Set data on MISO
    // No delay needed here, clock is slow
    while (digitalRead(clockPin) == LOW) {}  // Wait for CLOCK to go HIGH
    int bitReceived = digitalRead(mosiPin);  // Read MOSI
    dataReceived |= (bitReceived << i);  // Store the received bit
    while (digitalRead(clockPin) == HIGH) {}  // Wait for CLOCK to go LOW
  }

  while (digitalRead(chipSelectPin) == LOW) {}  // Wait for Slave deselection

  // Print the received data
  Serial.print("Data received: ");
  Serial.println(dataReceived, BIN);
  delay(100);
}
