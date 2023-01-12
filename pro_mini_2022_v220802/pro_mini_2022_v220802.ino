/*ARDUINO BLUETOOTH RC CAR: 
annah kartika

tx
01N    = client active
01A    = alarm on
01O    = 30 minute timer started
01T    = 10 second timer started

rx
010    = timer start 10 second
011    = timer start 10 minute
012    = timer start 20 minute
013    = timer start 30 minute
801    = reset


FU1 = set to FU1 mode = 3.6 mA
FU2 = set to FU2 mode = 80 μA  / not available in V22.07
FU3 = set to FU3 mode = 16  mA

Remot Client Timer RCT
pro-mini ver V22.08
+ reduce instruction command
+ stand alone battery monitor

pro-mini ver V22.07
+ added internal battery monitor
+ add FU1 mode for low power

client notes
21 = ATmega168
22 = ATmega168
23 = ATmega168 
24 = ATmega168
25 = ATmega328 
26 = ATmega168
27 = ATmega168
28 = ATmega168
29 = ATmega168
30 = ATmega328

 */
 
#include "Timer.h"
#include <SoftwareSerial.h>

SoftwareSerial MySerial(3,4); // RX, TX

float battVolts;
Timer t;
int acara;
int ngedip;
int buzzer2    =  7 ;  // pin buzzer
int buzzer3    =  6 ;
//int led        =  9 ;    // led white
int led      =  13;    // led internal
int hc12       =  2 ;   // pin set HC12
String bt      = "" ;       //BT 
int alarm;
int imer;
int biip;
int nit;
int count      =  0 ;
                        // change magic number here -------------------------------------- START
                        // magic number change from 0xx to 9xx for client < 10
int btx0 =  "300";       // 10 detik Timer
int btx1 =  "301";       // 10 Minutes Timer
int btx2 =  "302";       // 20 Minutes Timer
int btx3 =  "303";       // 30 Minutes Timer
int btxr = "830";       // Reset

/*-----------------------------------------------------------------------------------------------------*/

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void numnum()
{
  MySerial.write("30O");                                    // change client number here ------------------------
}

void readserial()
{
    if (MySerial.available() > 0)
  {
    bt = MySerial.readString();
    
               if (bt == btx0)                                 
              { 
              //MySerial.write("06T");                       
              //digitalWrite(hc12, HIGH);  
              //delay(90);
              Serial.print("Timer 10 sec ");
              Serial.println(millis());
              alarmF();
              if (acara==0){acara = t.after(10000, print_message);}
              if (ngedip==0){ngedip = t.every(5*1000,kedip);}
              if (biip==0){biip = t.every(5*1000,alarmF);}
              imer = 1;
              cekbat(); 
              }   
        
            else if (bt == btx1)                          
              { numnum();                         
              Serial.print("Timer 10 min ");
              Serial.println(millis());
              alarmF();
              if (acara==0){acara = t.after(10*60000, print_message);}
              if (ngedip==0){ngedip = t.every(5*1000,kedip);}
              if (biip==0){biip = t.every(30*1000,alarmF);}
              imer = 1;
              } 
        
            else if (bt == btx2)                
              { numnum ();                      
              Serial.print("Timer 20 min ");
              Serial.println(millis());
              alarmF();
              if (acara==0){acara = t.after(20*60000, print_message);}
              if (ngedip==0){ngedip = t.every(5*1000,kedip);}
              if (biip==0){biip = t.every(30*1000,alarmF);}
              imer = 1;
              }       
              
            else if (bt == btx3)  
              { numnum();  
              Serial.print("Timer 30 min ");
              Serial.println(millis());
              alarmF();
              if (acara==0){acara = t.after(30*60000, print_message);}
              if (ngedip==0){ngedip = t.every(5*1000,kedip);}
              if (biip==0){biip = t.every(30*1000,alarmF);}
              imer = 1;
              }    
              
            else if (bt == btxr)  { resetFunc();  }
           // else if (bt == "FU1") { FU1(); }
           // else if (bt == "FU3") { FU3(); }
        
            else {
             //Serial.println("server send");
             // Serial.println(bt);
              digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
              bt = "";
              delay(5);                       // wait for a second
              digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
            }
              MySerial.flush();
            
     }
}

bool print_message(void *) {
  Serial.print("Timer Stop at ");
  Serial.println(millis());
  Serial.println("alarm !");
  MySerial.write("30A");                               // change client number here ------------------------
  alarm = 1;
  t.stop(ngedip);
  t.stop(biip);
  return true; // repeat? true
   
}

void setup()
{
  pinMode(led,OUTPUT);
  pinMode(hc12,OUTPUT);
  pinMode(buzzer2,OUTPUT); 
  pinMode(buzzer3,OUTPUT); 
  digitalWrite(hc12, HIGH);  
  MySerial.begin(9600);
  Serial.begin(9600);                     
  digitalWrite(buzzer2, HIGH);
  digitalWrite(buzzer3, HIGH);    
  digitalWrite(led, HIGH);// 
  delay(6);
  digitalWrite(led, LOW);
  delay(50);             
  digitalWrite(led, HIGH);// 
  delay(6);
  digitalWrite(led, LOW);   
  digitalWrite(buzzer2, LOW);
  digitalWrite(buzzer3, LOW);
  MySerial.write       ("30N");                         // change client number here ------------------------
  delay(50); 
  digitalWrite(buzzer2, HIGH);
  digitalWrite(buzzer3, HIGH);
  digitalWrite(led, HIGH);
  delay(6);   
  digitalWrite(led, LOW);
  Serial.println("RCT V22.08 Client #300 Start Up");    // change client number here ------------------------ FINISH
  Serial.println("  iSerius Technology 2022");
  delay(30);                    
  digitalWrite(buzzer2, LOW); 
  digitalWrite(buzzer3, LOW); 

   FU3();
 }

int getBandgap(void)    // Returns actual value of Vcc (x 100)
    {
    #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
           // For mega boards
      const long InternalReferenceVoltage = 1115L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
        // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
      ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (0<<MUX5) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  
    #else
         // For 168/328 boards
     const long InternalReferenceVoltage = 1090L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
        // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
       
    #endif
     delay(50);  // Let mux settle a little to get a more stable A/D conversion
        // Start a conversion  
     ADCSRA |= _BV( ADSC );
        // Wait for it to complete
     while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
        // Scale the value
     int results = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L; // calculates for straight line value 
     return results;
    }
    
void cekbat()
{
     battVolts= getBandgap();  //Determins what actual Vcc is, (X 100), based on known bandgap voltage
     //battVolts /=100;
     //Serial.print("Battery Voltage =  ");
     Serial.print(battVolts /100);
     Serial.println("V");
     //Serial.print(" ..... ");
     //Serial.print(map(battVolts, 320, 420, 0, 100));
     //Serial.println("%");    
      
  //MySerial.println("");
  MySerial.print(battVolts /100);
  MySerial.print("V_");
  MySerial.print(map(battVolts, 320, 420, 0, 100));
  MySerial.print("%");    
}

void FU1()
{ digitalWrite(hc12, LOW);         // Enter command mode
  delay(60); 
  MySerial.println("AT+FU1");     // mode FU1
  delay(60);                 
  digitalWrite(hc12, HIGH);        // disini baru sleep beneran
  delay(200); 
  Serial.println("FU1 mode"); 
}

void FU3()
{ digitalWrite(hc12, LOW);         // Enter command mode
  delay(60); 
  MySerial.println("AT+FU3");     // mode FU3
  delay(60);                 
  digitalWrite(hc12, HIGH);        // disini baru sleep beneran
  delay(200); 
  //Serial.println("FU3 mode"); 
}


void loop()
{
  readserial();
  if (imer == 1){t.update();}
  if (alarm==1){alarmF();}
}
void kedip()

{ 
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(6);    
  digitalWrite(led, LOW); 
 
}  
void alarmF()
 {
  digitalWrite(buzzer2, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(buzzer3, HIGH);
  digitalWrite(led, HIGH); 
  delay(20);    
  digitalWrite(led, LOW);    // wait for a second
  delay(50);
  digitalWrite(buzzer2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(buzzer3, LOW);
  delay(70);   
}
/*
void alarmz()
 {
  if (nit==0){nit = t.every(70,nyalak);}
  if (nit==0){nit = t.every(140,matik);}
}
void nyalak()
{
  digitalWrite(buzzer2, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(buzzer3, HIGH);
   digitalWrite(led, HIGH); 
}
void matik()
{
  digitalWrite(buzzer2, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(buzzer3, LOW); 
  digitalWrite(led, LOW); 
}
*/
