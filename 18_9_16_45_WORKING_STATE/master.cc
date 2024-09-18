int clockPin = 13;
int chipSelectPin = 10;
int mosiPin = 11;
int misoPin = 12;

unsigned long dataToSend = 0;  // Variable to store the user-defined message
unsigned long dataReceived = 0;  // Variable to store received data
int numBits = 8;  // Default number of bits (can be changed)

void setup() {
  pinMode(clockPin, OUTPUT);
  pinMode(chipSelectPin, OUTPUT);
  pinMode(mosiPin, OUTPUT);
  pinMode(misoPin, INPUT);
  digitalWrite(chipSelectPin, HIGH);  // Deselect Slave by setting CS HIGH
  digitalWrite(clockPin, LOW);        // Initialize CLOCK to LOW
  Serial.begin(9600);                 // Initialize serial communication

  // Get number of bits from user
  Serial.println("Enter the number of bits to send (1-32):");
  while (Serial.available() == 0) {}  // Wait for input
  numBits = Serial.parseInt();        // Read number of bits from user
  if (numBits < 1 || numBits > 32) {  // Limit to a max of 32 bits
    numBits = 8;  // Default to 8 if input is invalid
    Serial.println("Invalid input. Defaulting to 8 bits.");
  }

  // Get binary message from user
  Serial.println("Enter the binary message to send:");
  while (Serial.available() == 0) {}  // Wait for input
  String input = Serial.readStringUntil('\n');  // Read binary message as a string
  input.trim();  // Remove any leading/trailing spaces

  // Convert the binary string to a number
  dataToSend = strtoul(input.c_str(), NULL, 2);

  // Validate the length of the message fits the bit length
  if (input.length() > numBits) {
    Serial.println("Message too long. Truncating to the specified number of bits.");
    dataToSend &= ((1UL << numBits) - 1);  // Truncate message to fit the bit length
  }
}

void loop() {
  digitalWrite(chipSelectPin, LOW);  // Select the Slave

  // First, send numBits to the slave
  for (int i = 7; i >= 0; i--) {
    digitalWrite(clockPin, LOW);  // Ensure clock is LOW
    digitalWrite(mosiPin, (numBits >> i) & 0x01);  // Set data on MOSI
    delay(250);  // 0.25 seconds (250 milliseconds)
    digitalWrite(clockPin, HIGH);  // Clock goes HIGH
    delay(250);  // 0.25 seconds
    digitalWrite(clockPin, LOW);  // Clock goes LOW for next bit
  }

  // Now, send and receive the chosen number of bits
  dataReceived = 0;  // Clear received data before actual data transmission
  for (int i = numBits - 1; i >= 0; i--) {
    digitalWrite(clockPin, LOW);  // Ensure clock is LOW
    digitalWrite(mosiPin, (dataToSend >> i) & 0x01);  // Set data on MOSI
    delay(250);  // 0.25 seconds
    digitalWrite(clockPin, HIGH);  // Clock goes HIGH
    delay(250);  // 0.25 seconds
    int bitReceived = digitalRead(misoPin);  // Read MISO
    dataReceived |= (bitReceived << i);  // Store the received bit
    digitalWrite(clockPin, LOW);  // Clock goes LOW for next bit
  }

  digitalWrite(chipSelectPin, HIGH);  // Deselect the Slave

  // Print the sent and received data
  Serial.print("Data sent: ");
  Serial.println(dataToSend, BIN);
  Serial.print("Data received: ");
  Serial.println(dataReceived, BIN);
  delay(1000);  // Wait before the next communication cycle
}
