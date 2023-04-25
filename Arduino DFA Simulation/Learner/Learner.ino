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
#define MY_ADDRESS 2   // Learner arduino address
#define DEST_ADDRESS 1 // Where to send packets to
#define TIMEOUT 2000   // Timeout in milliseconds

#define SERIAL_BAUD 9600

void Blink(int milliseconds);
void TransmitMessage(uint8_t reply[]);

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

/*
 * The Arduino can only handle 30 * 30 character arrays, as one char = 1 byte,
 * and the dynamic memory is only 2048 bytes large. Thus, the trace array take
 * up 900 bytes. This leaves around 400 bytes for local variables once
 * everything has been compiled.
 */
// Test traces
const byte max_number_of_characters_in_one_trace = 30;
char array_of_traces[/* Max 30 traces */][max_number_of_characters_in_one_trace] = {"XYAZXBC","AXBCY","X","AXBCABC","AXBC","XAB","AXYZXYZ","AXYZ","AB","ABCAXBCABCAYZB","XABCYAZ","XABCYZX","XYZAXBCAYZB","A","AXYZX","XYZXYZX","XABCAYZB","AXYZXB","ABC","AX","ZX","ZBXXAABY","CYYBACYBCB","YYBCBAYYXZX","CAABZCXXZXXA","BYBXCBZYCYAB","Z","C","BZYABXXAXAZ","ZACXAYXCCY"};
const int array_size = sizeof(array_of_traces) / sizeof(array_of_traces[0]);
bool finished = false;

void setup() {
  // Setup for RFM69 chipset
  Serial.begin(SERIAL_BAUD);
  // Uncomment the following line, if used without serial connection
  while (!Serial) {
    delay(1);
  }

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);

  // Manual reset
  digitalWrite(RFM69_RST, LOW);
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69_manager.init()) {
    while (1) {
      Blink(1000);
    }
  }

  // Set base frequency
  if (!rf69.setFrequency(RF69_FREQ)) {
    while (1) {
      Blink(2000);
    }
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
  while (!finished) {
    delay(5000);
    // Loop over traces
    for (int i = 0; i < array_size; i++) {
      // Loop over char in trace
      for (int j = 0; j <= strlen(array_of_traces[i]); j++) {
        // If at end of a trace, tell blackbox its END
        if (j == strlen(array_of_traces[i])) {
          uint8_t message[] = "$";
          TransmitMessage(message);
          break;
        } else {
          // Send array_of_traces[i][j]
          uint8_t message[] = {(uint8_t)array_of_traces[i][j]};
          TransmitMessage(message);
        }
      }
      //wait for messagde from blackbox
      while(!rf69_manager.available()){delay(10);}

      // Print result from blackbox
      if (rf69_manager.available()) {
        // Wait for a message addressed to the Learner arduino
        uint8_t len = sizeof(buffer);
        uint8_t from;
        if (rf69_manager.recvfromAckTimeout(buffer, &len, TIMEOUT, &from)) {
          if ((char &)buffer == 'A') {
            Serial.print(strcat(array_of_traces[i], ":PASSED\n"));
          } 
          else if ((char &)buffer == 'F')  {
            Serial.print(strcat(array_of_traces[i], ":FAILED\n"));
          }
        }
      }
    }
    // When no more traces, print "STOP" to stop python monitor
    Serial.print("STOP\n");
    finished = true;
  }
  // Once all traces have been tested, indicate that we are finished by blinking
  Blink(100);
}

/*
 * Blink is used for error codes and to indicate that a run has finished,
 * as the serial monitor will solely be used for communication between
 * the learner and the blackbox device.
 * The error codes are as follows:
 * ½ second blinks: Failed to send message.
 * 1 second blinks: Failed RFM69HW initialization.
 * 2 second blinks: Failed while setting the frequency.
 */
void Blink(int milliseconds) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(milliseconds);
  digitalWrite(LED_BUILTIN, LOW);
  delay(milliseconds);
}

void TransmitMessage(uint8_t reply[]) {
  if (!rf69_manager.sendtoWait(reply, strlen((char *)reply), DEST_ADDRESS)) {
    // If the transmission fails, flash the error code
    for (int i = 0; i < 10; i++) {
      Blink(500);
    }
  }
}
