/* Set TASK to the task you are working on.
 * Each task assumes that the previous tasks are already completed. */ 



#define SPI_SCK_PIN  13
#define SPI_MISO_PIN 12
#define SPI_MOSI_PIN 11
#define SPI_CS_PIN   10

#define START_DELAY 50 // Delay before loop()

/* Send data from FIRST_CHAR to LAST_CHAR */
#define FIRST_CHAR '0'
#define LAST_CHAR  '9'

/* Data to be sent */
char dataSend = FIRST_CHAR;

/* Clock internal variables */
int clkState = LOW;

/* Reset clock internal variables */
void resetClock(void)
{
  clkState = LOW;
}

/* Read the clock state.
 *
 * Return -1       NO transition
 *         0(LOW)  in case it was a transition from HIGH to LOW.
 *         1(HIGH) in case it was a transition from LOW to HIGH.
 */
int getClockTransition(void)
{
  int transition = -1;
  int currentClkVal = LOW;
  

  /* TODO3: Read the Clock pin value to be in sync with the Master */
  currentClkVal = digitalRead(SPI_SCK_PIN);

  
  if (clkState != currentClkVal)
  {
    clkState = !clkState;
    transition = clkState;
  }
  
  return transition;
}

/* Send a byte of data to the Master. 
 * Receive a byte of data from the Master.
 *
 * param  dataSend - data to be sent to the Master
 *
 * Return char     - data received from the Master
 */
char sendRecvByte(char dataSend)
{
  int bit;
  int clkTransition;
  char dataRecv;
  
  int dataSendBitIndex = 0;
  int dataRecvBitIndex = 0;


  resetClock();
  Serial.println("CLK signal listen");
  

  /* TODO4: Extract the bit from the dataSendBitIndex of the dataSend byte of data */
  bit = bitRead(dataSend, dataSendBitIndex);
  /* TODO4: Send the bit using the MISO pin */
  digitalWrite(SPI_MISO_PIN, bit);

  dataSendBitIndex++;
  
  while (1)
  {
    clkTransition = getClockTransition();
    if (clkTransition == HIGH)
    {
      Serial.println("CLK signal read HIGH");
      

      /* TODO4: Read the incoming bit of data on the MOSI pin */
      bit = digitalRead(SPI_MOSI_PIN);
      /* TODO4: Store the bit to the dataRecvBitIndex of the dataRecv byte of data */
      bitWrite(dataRecv, dataRecvBitIndex, bit);

      dataRecvBitIndex++;
    }
    else if (clkTransition == LOW)
    {
      Serial.println("CLK signal read LOW");
      

      /* TODO4: Extract the bit from the dataSendBitIndex of the dataSend byte of data */
      bit = bitRead(dataSend, dataSendBitIndex);
      /* TODO4: Send the bit using the MISO pin */
      digitalWrite(SPI_MISO_PIN, bit);

      dataSendBitIndex++;
    }
    
    if ((dataSendBitIndex == 8) && (dataRecvBitIndex == 8))
    {
      break;
    }
  }

  
  return dataRecv;
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello, I am SPI Slave");


  /* TODO1: Configure SPI pins: INPUT/OUTPUT */
  pinMode(SPI_SCK_PIN,  INPUT);
  pinMode(SPI_MISO_PIN, OUTPUT);
  pinMode(SPI_MOSI_PIN, INPUT);
  pinMode(SPI_CS_PIN,   INPUT);
  
  /* TODO1: Set the output SPI pins to the idle values: LOW/HIGH */
  digitalWrite(SPI_MISO_PIN, LOW);
  
  Serial.println("SPI Slave pins are initialized");

  
  delay(START_DELAY);
  Serial.println();
}

void loop(void)
{
  char dataRecv = '?';
  

  /* TODO2: Wait to be selected by the Master */
  while (digitalRead(SPI_CS_PIN) == HIGH)
  {
    ;
  }
  Serial.println("I was selected by the Master");


  /* Communicate over SPI */
  dataRecv = sendRecvByte(dataSend);


  /* TODO2: Wait to be unselected by the Master */
  while (digitalRead(SPI_CS_PIN) == LOW)
  {
    ;
  }
  Serial.println("I was unselected by the Master");

  

  /* Print data */
  Serial.print("Sent:     ");
  Serial.println(dataSend);
  Serial.print("Recieved: ");
  Serial.println(dataRecv);
  
  /* Choose new data to send */
  dataSend++;
  if (dataSend == (LAST_CHAR + 1))
  {
    dataSend = FIRST_CHAR;
  }


  Serial.println();

}
