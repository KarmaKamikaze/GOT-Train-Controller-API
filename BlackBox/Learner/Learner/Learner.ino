#include <FiniteStateMachine.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <SPI.h>

// Radio stuff
#define RF69_FREQ 868  // Frequency
#define RFM69_INT 2    // DIO0 Pin
#define RFM69_CS 10    // Select Signal Pin
#define RFM69_RST 3    // RST Pin
#define LED 9          // Test LED
#define MY_ADDRESS 1   // Blackbox arduino address
#define DEST_ADDRESS 2 // Where to send packets to
#define TIMEOUT 2000   // Timeout in milliseconds

#define SERIAL_BAUD 9600

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

const char* array_of_traces[3] = {"100100110", "011010", "101010"};
const int array_size = sizeof(array_of_traces)/sizeof(array_of_traces[0]);
void TransmitMessagde(uint8_t reply[]);

void setup() {
 // Setup for RFM69 chipset
  Serial.begin(SERIAL_BAUD);
  // Uncomment the following line, if used without serial connection
  while (!Serial) {
    delay(1);
  } // Wait until serial console is open

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  Serial.println("RFM69HW Arduino DFA Blackbox System!");
  Serial.println();

  // Manual reset
  digitalWrite(RFM69_RST, LOW);
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69_manager.init()) {
    Serial.println("RFM69HW radio init failed.");
    while (1)
      ;
  }
  Serial.println("RFM69HW radio init OK!");

  // Set base frequency
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed.");
  } else {
    Serial.println("Listening at 868 MHz.");
  }
  // RFM69HW *requires* that the Tx power flag is set!
  rf69.setTxPower(20); // power range from 14-20

  // The encryption key has to be the same across all devices
  // The hex values were randomly generated by random.org
  uint8_t encryption_key[16] = {0x2B, 0x0F, 0x6D, 0x16, 0x54, 0x38, 0x3E, 0x63,
                                0x37, 0x5F, 0x1B, 0x09, 0x19, 0x5F, 0x2A, 0x6A};
  rf69.setEncryptionKey(encryption_key);
}

// Dont put this on the stack:
uint8_t buffer[RH_RF69_MAX_MESSAGE_LEN];

void loop() {
    
  /*loop over traces*/
  for (int i=0; i < array_size; i++){
    //loop over char in trace
    for(int j = 0; j <= strlen(array_of_traces[i]); j++){
      //if at end of a trace tell blackbox its END
      if(j == strlen(array_of_traces[i])){
        TransmitMessagde("END");
        break;
      }
      else{
        //Send array_of_traces[i][j]
        TransmitMessagde(array_of_traces[i][j]);
      }
    }
    /*when one trace have been sent await result*/
    while(!rf69_manager.available()){
    delay(10);
    }
    /*Print result from blackbox*/
    if (rf69_manager.available()) {
      // Wait for a message addressed to the Blackbox arduino
      uint8_t len = sizeof(buffer);
      uint8_t from;
      if (rf69_manager.recvfromAckTimeout(buffer, &len, TIMEOUT, &from)) {

        if ((char *)buffer == "Trace Accepted!") {
          Serial.print(strcat(array_of_traces[i],":SUCCESS"));
        }
        else if((char *)buffer == "Trace Failed!"){
          Serial.print(strcat(array_of_traces[i],":FAILED"));
        }
      }
    }
  }
  /*When no more traces, print "STOP" to stop python*/
  Serial.print("STOP");
  }
  


void TransmitMessagde(uint8_t reply[]) {
  Serial.println((char *)reply);

  if (!rf69_manager.sendtoWait(reply, sizeof(reply), DEST_ADDRESS))
    Serial.println("sendtoWait failed.");
}