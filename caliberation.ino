//This coding is use to caliberats the values and debug.

#include <TimerOne.h>

#define SOL_AMPS_CHAN A1  //read solar amps

#define SOL_VOLTS_CHAN A0 

#define BAT_VOLTS_CHAN A2

#define AVG_NUM 10

#define SOL_AMPS_SCALE 48

#define SOL_VOLTS_SCALE 53

#define BAT_VOLTS_SCALE 53

int sol_amps;
int sol_volts;
int bat_volts;
int sol_watts;
int old_sol_watts = 0;
void setup(){pinMode(SOL_AMPS_CHAN,INPUT);
             pinMode(SOL_VOLTS_CHAN,INPUT);
  Serial.begin(9600);
}
void loop(){
          read_data();
          delay(1000);
}

/////////////////////////////////....................sampling data.................../////////////
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
                      Serial.println(read_adc(SOL_AMPS_CHAN));
                      sol_volts = ((read_adc(SOL_VOLTS_CHAN) * SOL_VOLTS_SCALE) +5) /10; 
                      Serial.println(read_adc(SOL_VOLTS_CHAN));
                      bat_volts = ((read_adc(BAT_VOLTS_CHAN) * BAT_VOLTS_SCALE) +5) /10;
                      sol_watts = (int)((((long)sol_amps) * (long)sol_volts + 50) / 100); 
             Serial.print("solar voltage = ");
             Serial.print(sol_volts);
             Serial.print("    solar ampar = ");
             Serial.print(sol_amps);
             Serial.println(); 
                    }         
