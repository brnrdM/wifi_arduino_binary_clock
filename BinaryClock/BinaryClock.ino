//Bernard Morelos Jr. 
//Binary Clock Using DS3231
//
//This program uses a DS3231 connected through I2C communication. It extracts data from the clock chip,
//interprets it into an integer array, and then uses a function to convert the decimal values into a binary string.
//The program enters a 6 cycle loop which controls which column the arduino is projecting to.
//It checks the state of the loop, and turns off the other columns so that only one column is activated.
//In a 4 cycle loop inside of the 6 cycle loop, the binary string controls which rows of the LEDs will turn on.
//1 would be interpreted as ON, and 0 would be interpreted as OFF.
//The arduino then cycles to the next column. The fast cycling gives the illusion of a constant light, when in fact,
//the matrix is being turned on and off at a 166 Hz

#include <DS3231.h>
#define DEBUG false

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
Time  t;

//Initialize Variables
int     hms [] = {0, 0, 0, 0};
int hmsUnit [] = {0, 0, 0, 0};
int hmsAlrm [] = {0, 0, 0, 0};
int binRow[3];
int potPin[1];
int bufder = 250; // Light flash delay is set to 250 ms

// Function Clear LED parms: (LOWER BOUND, UPPER BOUND, IGNORED VALUE, HIGH/LOW)
void clearLED(int j, int k, int IGNORE, int HghOrLow) {
    for (int i=j; i<=k; i++){
      if (i != IGNORE){
        digitalWrite(i, HghOrLow);
      }
    }
    return;
}

// String function, converts decimal input to binary string
String convBin(int DECINPUT){
    String myStr2;
    // Read amount of bits
    int zeros = String(DECINPUT,BIN).length();
      
    //This will add MSB zero to string as need
    for (int i=0; i < 4 - zeros; i++) 
      { 
        myStr2 = myStr2 + "0";
      }
      
    myStr2 = myStr2 + String(DECINPUT,BIN);
    //Serial.print("\t");
    //Serial.println(DECINPUT);
    
    return myStr2;
}

void printToMatrix(int mtrix[]){
  //Output Time to the 4x6 Matrix LEDs
  for (int o = 6; o > 0; o--) // This for loop controls the COLUMNS
    { 
      // Convert the decimal bits to Binary
      String myStr;
      myStr = convBin(mtrix[o]);
      
      // SET UP LEDS, Turn off COLUMNS not being represented
      switch(o) {
        case 6:   clearLED(6, 11,  11, LOW);   break;
        case 5:   clearLED(6, 11,  10, LOW);   break;
        case 4:   clearLED(6, 11,   9, LOW);   break;
        case 3:   clearLED(6, 11,   8, LOW);   break;
        case 2:   clearLED(6, 11,   7, LOW);   break;
        case 1:   clearLED(6, 11,   6, LOW);   break;
        default:                               break;
      }

      /*Print in Serial: Hh:Mm:Ss
      Serial.print(mtrix[1]); Serial.print(mtrix[2]); Serial.print(":"); 
      Serial.print(mtrix[3]); Serial.print(mtrix[4]); Serial.print(":");
      Serial.print(mtrix[5]); Serial.println(mtrix[6]); 
      */
      for (int i=0; i <= 3; i++){ // controls the ROWS, reads BIN string
        binRow[i] = myStr[i]-48;
        //Serial.println(binRow[i]);
        if (binRow[i] == 1){
          digitalWrite(i+2, LOW);
        }
        else if (binRow[i] == 0) {
          digitalWrite(i+2, HIGH);
        }
      }
  
      delay(1);
      clearLED(2, 11, 999, HIGH);       
  }
  return;
}

//**************************** S E T U P ********************************
void setup()
{
  // Setup Serial connection
  Serial.begin(115200);
  pinMode(12, INPUT);
  
  // Setup LEDs 2-5 ROWS (GND) 6-9 (VCC)
  for (int i=2; i<=11; i++){
      pinMode(i, OUTPUT);
    }
    
  clearLED(2, 11, 999, HIGH);
  delay(400);  
  
  
  //Serial.println("Successfully Started");
  
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(11, 59, 33);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014  
}

//**************************** M A I N  L O O P ********************************
void loop()
{
  if (digitalRead(12) == LOW) {
    //Get the time from DS3231 through I2C
    t = rtc.getTime(); 
    hms[2] = t.sec;
    hms[1] = t.min;
    hms[0] = t.hour;
    //Distribute integer values into individual bits, store into an array
    hmsUnit[6] = hms[2]%10; 
    hmsUnit[5] = (int)(hms[2]/10);
    hmsUnit[4] = hms[1]%10;
    hmsUnit[3] = (int)(hms[1]/10); 
    hmsUnit[2] = hms[0]%10;
    hmsUnit[1] = (int)(hms[0]/10);

    printToMatrix(hmsUnit);
  } else if (digitalRead(12) == HIGH) {
    int hour_val = analogRead(A0);
    float voltage = hour_val * (23.0 / 1023.0);
    hms[0] = (int)voltage;
    
    int min_val = analogRead(A1);
    float voltage2 = min_val * (11.0 / 1023.0);
    hms[1] = (int)voltage2;
    hms[1] *= 5;
    
    hmsAlrm[6] = 0; 
    hmsAlrm[5] = 0;
    hmsAlrm[4] = hms[1]%10;
    hmsAlrm[3] = (int)(hms[1]/10); 
    hmsAlrm[2] = hms[0]%10;
    hmsAlrm[1] = (int)(hms[0]/10);
    
    printToMatrix(hmsAlrm);
  }

  //COMPARE TIME WITH ALARM1
  if (hmsAlrm[4] == hmsUnit[4] && hmsAlrm[3] == hmsUnit[3] &&
      hmsAlrm[2] == hmsUnit[2] && hmsAlrm[1] == hmsUnit[1] &&
      digitalRead(12) == LOW) {
        clearLED(6, 11,  999, HIGH);
        Serial.println("+GET,ALM");
        //TRIGGER ALARM
        for (int i = 1; i <= 8; i++) {
          clearLED(2, 5,  999, LOW);
          delay(bufder);
          clearLED(2, 5,  999, HIGH);
          delay(bufder);
        }
        
        //CLEARS ALARM VARIABLES
        for (int i = 1; i <= 4; i++){
            hmsAlrm[i] = 9;
        }
      }
}

