#include <SPI.h>

/*

Circuit:

 SS:   pin 10
 MOSI: pin 11
 MISO: pin 12
 SCK:  pin 13
 
 */
 

// Arduino commands
#define ARDUINO_CMD_AVAILABLE 0x00
#define ARDUINO_CMD_READ      0x01
#define ARDUINO_CMD_WRITE     0x02
#define ARDUINO_CMD_TEST      0x03 

//#define SLAVESELECT 10//ss
const int CS = 10;


/*  Minimum wait time between SPI message: 160 us
 *  Minimum wait time between two bytes in a SPI message: 15 us
 */
const int wait_time_us_between_spi_msg   = 200;
const int wait_time_us_between_bytes_spi = 20;

void setup() {
  Serial.begin(9600);


  pinMode(CS, OUTPUT); // we use this for SS pin
  
  // start the SPI library:
  SPI.begin();  // wake up the SPI bus.
  SPI.setClockDivider(SPI_CLOCK_DIV32) ; // 16MHz/32 = 0.5 MHz
  

  SPI.setDataMode(SPI_MODE0);  // By Default SPI_MODE0, we make this fact explicit.
  SPI.setBitOrder(MSBFIRST);   // By Default MSBFIRST,  we make this fact explicit.

  delay(100);
}

int previous_cmd_status;
int shield_status;
int available_msb;
int available_lsb;
int available;

  
void loop() {

  available = checkifAvailableRXData() ;

  if (available){
    
    Serial.print("Message received, size:");
    Serial.print(available);
    

    byte data_rcv[available];
    readAvailableRXData(data_rcv , available);
    
    Serial.print(" data: ");
    PrintHex8(data_rcv , available); // Serial.println( data_rcv[i] , HEX); println as HEX does not include trailing zeroes! 0x00 gets printed as '0'
    
    Serial.println("");
     
  }

  delay(250);
  
}

void readAvailableRXData(byte buf[] , int buf_len) {

  for (int i=0; i<buf_len; i++) {

    digitalWrite(CS,LOW);          
       
      //  READ BYTE CMD
      previous_cmd_status = SPI.transfer(ARDUINO_CMD_READ);
        delayMicroseconds(wait_time_us_between_bytes_spi);
      shield_status = SPI.transfer(0x00);
        delayMicroseconds(wait_time_us_between_bytes_spi);
      // Store the received byte
      buf[i] = SPI.transfer(0x00);
        delayMicroseconds(wait_time_us_between_bytes_spi);
       
    digitalWrite(CS,HIGH);
      
    delayMicroseconds(wait_time_us_between_spi_msg);
     
  }

  
}  



int checkifAvailableRXData() {
  
  int tmp = 0;
  
  digitalWrite(CS, LOW);

    // data available cmd
    previous_cmd_status = SPI.transfer(ARDUINO_CMD_AVAILABLE);
      delayMicroseconds(wait_time_us_between_bytes_spi);
    shield_status = SPI.transfer(0x00);
      delayMicroseconds(wait_time_us_between_bytes_spi);  
    available_msb = SPI.transfer(0x00);
      delayMicroseconds(wait_time_us_between_bytes_spi); 
    available_lsb = SPI.transfer(0x00);
      delayMicroseconds(wait_time_us_between_bytes_spi);
  
  // take the SS pin high to de-select the chip:
  digitalWrite(CS,HIGH); 


  tmp = (available_msb<<8) + available_lsb;
  
  return tmp;
  
  
}




void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
  char tmp[length*2+1];
  byte first ;
  int j=0;
  for (uint8_t i=0; i<length; i++) 
  {
    first = (data[i] >> 4) | 48;
    if (first > 57) tmp[j] = first + (byte)39;
    else tmp[j] = first ;
    j++;

    first = (data[i] & 0x0F) | 48;
    if (first > 57) tmp[j] = first + (byte)39; 
    else tmp[j] = first;
    j++;
  }
  tmp[length*2] = 0;
  Serial.print(tmp);
}

