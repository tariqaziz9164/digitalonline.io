#include <TimerOne.h>

#define SOL_AMPS_CHAN A1  //read solar amps

#define SOL_VOLTS_CHAN A0 

#define BAT_VOLTS_CHAN A2

#define AVG_NUM 10

#define SOL_AMPS_SCALE 12

#define SOL_VOLTS_SCALE 27

#define BAT_VOLTS_SCALE 27

#define PWM_PIN 9

#define PWM_ENABALE_PIN 8

#define load_pin 7
#define R 13
#define G 11
#define Y 12

#define PWM_FULL  1023

#define PWM_MAX 100

#define PWM_MIN 60

#define PWM_START 90

#define PWM_INC 1

#define TRUE 1

#define FALSE 0

#define ON TRUE

#define OFF FALSE

#define TURN_ON digitalWrite (8,HIGH) // use pin 8 if driver is use for switching
#define TURN_OFF digitalWrite (8,LOW)

#define ONE_SECOND 50000     // i think scale devid hunderd  so 500,will be corect in run time

#define LOW_SOL_WATTS 800    //8 watt
#define MIN_SOL_WATTS 500  //5 watt
#define MIN_BAT_VOLTS 1100   //scaled by 100 this is 11.00v
#define MAX_BAT_VOLTS 1420   //scaleld by 100 so this is 14.20v

#define HIGH_BAT_VOLTS 1320  // scale to 13.20v 
#define OFF_NUM 10   //numbers of acurence of OFF
//.................................global vareable..........................
int count = 0;
int pwm = 0;
int sol_amps;
int sol_volts;
int bat_volts;
int sol_watts;
int old_sol_watts = 0;
unsigned int seconds = 0;
unsigned int prev_seconds = 0;
unsigned int interrupt_counter = 0;
int delta = PWM_INC;
float LVD = 11.00;
enum charger_mode{off,on,bulk,bfloat}charger_state;

void setup() 
            {
              pinMode(PWM_ENABALE_PIN,OUTPUT);
              Timer1.initialize(20);
              Timer1.pwm(PWM_PIN,0);
            //  Timer1.attachInterrupt(callback);
              Serial.begin(9600);
              pwm = PWM_START;
              charger_state = on;

            }

            /////////////////////////loop
void loop() {
             read_data();
                             /*    load();
                                   led_output();
                                   run_charger();
                                    if((seconds - prev_seconds) > 0)
                                          {
                                              prev_seconds = seconds;
                                              read_data();
                                              run_charger();
                                           }  */
             
             
            Serial.print(" solar voltage =   ");
             Serial.print(sol_volts);
             Serial.print("  solar ampar =   ");
             Serial.print(sol_amps);
             Serial.println();
             
            // Serial.println("battery voltage = ");
            // Serial.print(bat_volts);
             
            // Serial.println("solar power =  ");
            // Serial.print(sol_watts);
            // Serial.println();
               delay(500);                                     

            }

            int read_adc(int channel)
                        {
                          int sum = 0;
                          int temp;
                          int i;
                          for(i=0;i<AVG_NUM;i++)  //10 samples
                                               {
                                                temp= analogRead(channel);
                                                sum+=temp;
                                                delayMicroseconds(50);
                                               }return(sum/AVG_NUM);
                        }

void read_data(void)
                    {
                      sol_amps = ((read_adc(SOL_AMPS_CHAN) * SOL_AMPS_SCALE)    +5)/10;  
                      float amps = read_adc(SOL_AMPS_CHAN);
                      Serial.println(amps);
                      sol_volts = ((read_adc(SOL_VOLTS_CHAN) * SOL_VOLTS_SCALE) +5) /10; //Serial.println(sol_volts);
                      bat_volts = ((read_adc(BAT_VOLTS_CHAN) * BAT_VOLTS_SCALE) +5) /10;
                      sol_watts = (int)((((long)sol_amps) * (long)sol_volts + 50) / 100);  
                    }   
                        
