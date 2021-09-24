#include <TimerOne.h>

#define SOL_AMPS_CHAN A1  //read solar amps

#define SOL_VOLTS_CHAN A0 

#define BAT_VOLTS_CHAN A2

#define AVG_NUM 10
#define SOL_AMPS_SCALE 0.048828125  // 5/(1024*0.100) 0.100 for 20 amps sensor

#define SOL_VOLTS_SCALE 0.0537109375 //r1 = 100k  r2 = 10k     5/1024 * r1+r2/r2

 #define BAT_VOLTS_SCALE 0.0537109375

#define PWM_PIN 9

#define PWM_ENABALE_PIN 8

#define LOAD_PIN 6
#define LED_RED 13
#define LED_GREEN 11
#define LED_YELLOW 12

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

#define LOW_SOL_WATTS 8.00    //8 watt
#define MIN_SOL_WATTS 5.00  //5 watt
#define MIN_BAT_VOLTS 11.00   //scaled by 100 this is 11.00v
#define MAX_BAT_VOLTS 14.30   //scaleld by 100 so this is 14.20v
#define BATT_FLOAT 13.60
#define HIGH_BAT_VOLTS 13.20  // scale to 13.20v 
#define OFF_NUM 10   //numbers of acurence of OFF
#define LVD 11.00
//#define OFF_NUM 9
//.................................global vareable..........................
int count = 0;
int pwm = 0;
float sol_amps;
float sol_volts;
float bat_volts;
float sol_watts;
float old_sol_watts = 0;
unsigned int seconds = 0;
unsigned int prev_seconds = 0;
unsigned int interrupt_counter = 0;
unsigned long time = 0;
int delta = PWM_INC;
//float LVD = 11.00;
int load_status = 0;
enum charger_mode{off,on,bulk,bfloat}charger_state;

//////////////////////////.....................setup...............//////////////



void setup() 
            { pinMode (LED_RED,OUTPUT);
              pinMode (LED_GREEN,OUTPUT);
              pinMode (LED_YELLOW,OUTPUT);
              pinMode(PWM_ENABALE_PIN,OUTPUT);
              Timer1.initialize(20);
              Timer1.pwm(PWM_PIN,0);
              Timer1.attachInterrupt(callback);
              Serial.begin(9600);
              pwm = PWM_START;
              charger_state = off;
              pinMode(LOAD_PIN,OUTPUT);
              digitalWrite(LOAD_PIN,LOW);

            }

            /////////////////////////loop
void loop() {
             read_data();
             run_charger();
             load();
             led_output();
            
             
             if((seconds - prev_seconds) > 0)
                                            {
                                              prev_seconds = seconds;
                                              read_data();
                                              run_charger();
                                            }
         
             
            
            // Serial.println("battery voltage = ");
            // Serial.print(bat_volts);
             
            // Serial.println("solar power =  ");
            // Serial.print(sol_watts);
            // Serial.println();
              // delay(500);                                     

            }
            ////////////////////load on off///
            void load(void)
                          {
                            if(bat_volts< LVD )
                                            {
                                              digitalWrite(LOAD_PIN,LOW);
                                              load_status = 0;
                                            }
                                        else if(bat_volts>LVD)
                                                         {
                                                          digitalWrite(LOAD_PIN,HIGH);
                                                         }
                                                         
                          }
////////////////////////calbcack routine that occur every 20 micro senconds////////////////
void callback()
              {
                if (interrupt_counter++ > ONE_SECOND)
                                                    { interrupt_counter = 0;
                                                      seconds++;
                                                    }             
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
///////////////////////////////////////.......pwm seting routine......////////////////////
void set_pwm_duty(void)
                      {
                        if(pwm > PWM_MAX)
                                        { pwm = PWM_MAX;}
                          else if (pwm < PWM_MIN)
                                                {pwm = PWM_MIN;}
                                      if(pwm<PWM_MAX)
                                                    {
                                                      Timer1.pwm(PWM_PIN,(PWM_FULL * (long)pwm/100),20);                               
                                                    }
                                               else if (pwm == PWM_MAX)
                                                                      {
                                                                        Timer1.pwm(PWM_PIN,(PWM_FULL - 1),1000);//keep sweitching at 99.9 and slow down 1000 micro seconds
                                                                             
                                                                      }
                      }
 /////////////////////////////////..........routine to store real values.......///////////////////
 
 void read_data(void){
                      sol_amps = (read_adc(SOL_AMPS_CHAN) * SOL_AMPS_SCALE )- 24.8;  
                      
                      sol_volts = read_adc(SOL_VOLTS_CHAN) * SOL_VOLTS_SCALE -0.6; 
                     // Serial.println(read_adc(SOL_VOLTS_CHAN));
                      bat_volts = read_adc(BAT_VOLTS_CHAN) * BAT_VOLTS_SCALE -0.6;
                      sol_watts = sol_amps * sol_volts ; 
                    //  Serial.print("solar voltage = ");
                    //  Serial.print(sol_volts);
                    //  Serial.print("    solar ampar = ");
                     // Serial.print(sol_amps);
                     // Serial.println(); 
                    }         
//////////////////////////////////////////.this rountien is charger state machin  1 on:   solar watts > minimum watts in this state pwm is 99.9% for maximum power
/////////////////////////////////////////                                         2 bulk: happens if solar watts greater then minimum needs watts.here the mppt algorethm is a apply.
////////////////////////////////////////////                                      3 Float: happens if solar voltage reach its maximum level here keep the bat volts at maximum by addjust pwm.
//////////////////////////////////////////////////                                4 off  : happense when panel is not providing enoughf power,the mosfet is turn off
void run_charger(void) 
                     {
                      static int off_count = OFF_NUM;
                      switch(charger_state)
                                          {
                                            case on:
                                                   if(sol_watts<MIN_SOL_WATTS)  //5w
                                                                             {
                                                                              charger_state = off;     /////////////////chaeger off///////
                                                                              off_count = OFF_NUM;
                                                                              TURN_OFF;
                                                                             }
                                                                              else if(bat_volts>MAX_BAT_VOLTS) //13.5v
                                                                                     {
                                                                                      charger_state = bfloat;     ///////////charger float
                                                                                                             
                                                                                     }
                                                             else if(sol_watts < LOW_SOL_WATTS ) //&& sol_watts>MIN_SOL_WATT
                                                                                              {
                                                                                                pwm = PWM_MAX; //100.should be .99.9 for operation //boost
                                                                                                set_pwm_duty();                        
                                                                                              }

                                                                     else 
                                                                        {
                                                                          pwm = ((bat_volts * 10)/(sol_volts/10)) + 5;  //if bat= 12,solv= 17 so pwm=70.5
                                                                          charger_state = bulk;                   //////////////////charger bulk
                                                                                               
                                                                         }break;
                                           case bulk:
                                                    if(sol_watts<MIN_SOL_WATTS)
                                                                              {
                                                                                charger_state = off;           //////////off
                                                                                off_count = OFF_NUM;
                                                                                TURN_OFF;                 
                                                                              }
                                                       else if(bat_volts>MAX_BAT_VOLTS)
                                                                                      {
                                                                                        charger_state = bfloat;    /////float
                                                                                                             
                                                                                      }
                                                              else if(sol_watts < LOW_SOL_WATTS)
                                                                                              {
                                                                                                charger_state = on; ////on
                                                                                                
                                                                                                TURN_ON;
                                                                                                
                                                                                               }
                                                                    else {  //begin mppt
                                                                          if (old_sol_watts >= sol_watts)
                                                                                                        {
                                                                                                          delta = -delta;                       
                                                                                                        }
                                                                                                        pwm += delta;
                                                                                                        old_sol_watts = sol_watts;
                                                                                                        set_pwm_duty();
                                                                         }
                                                                     break;
                                         case bfloat:
                                                       if(sol_watts < MIN_SOL_WATTS)
                                                                                   {
                                                                                    charger_state = off;   ////offf
                                                                                    off_count = OFF_NUM;
                                                                                    set_pwm_duty();
                                                                                    TURN_OFF;
                                                                                    
                                                                                    
                                                                                   }
                                                                                   else if(bat_volts > MAX_BAT_VOLTS)
                                                                                                                    {
                                                                                                                      pwm -=1;
                                                                                                                      set_pwm_duty();
                                                                                                                      
                                                                                                                    }
                                                                                                                     else if(bat_volts< MAX_BAT_VOLTS)
                                                                                                                                                     {
                                                                                                                                                      pwm += 1;
                                                                                                                                                      set_pwm_duty();
                                                                                                                                                      if(pwm>=100)
                                                                                                                                                                 {
                                                                                                                                                                  charger_state = bulk;
                                                                                                                                                                  
                                                                                                                                                                 }
                                                                                                                                                     }
                                                                                                                                                     break;
                                       case off:
                                                if(off_count > 0)
                                                                {
                                                                  off_count --;
                                                                }
                                                 else if((bat_volts > HIGH_BAT_VOLTS) && (bat_volts < MAX_BAT_VOLTS) && (sol_volts > bat_volts))
                                                                                   {
                                                                                    charger_state = bfloat;   ////float
                                                                                    set_pwm_duty();
                                                                                    TURN_ON;
                                                                                                   
                                                                                   }
                                                       else if((bat_volts > MIN_BAT_VOLTS) && (bat_volts < MAX_BAT_VOLTS) && (sol_volts > bat_volts))
                                                                                                                                                    {
                                                                                                                                                      pwm = PWM_START;
                                                                                                                                                      set_pwm_duty();
                                                                                                                                                      charger_state = on; /////on
                                                                                                                                                      TURN_ON;
                                                                                                                                                                                 
                                                                                                                                                    }break;
                                       default:
                                                TURN_OFF;
                                                break;                                                                                                             
                                          }
                     }



            /////////////////////////....LED...
            void led_output(void)
                {
                 if ( bat_volts >= 14.2 )
                                  {
                                    leds_off_all();
                                    digitalWrite(LED_YELLOW,HIGH);                                                                       
                                  }
                                  else if(bat_volts>=12 && bat_volts < 14.2)
                                                                  { 
                                                                    leds_off_all();
                                                                    digitalWrite (LED_GREEN,HIGH);
                                                                  }
                                                             else if(bat_volts<12)
                                                                             {
                                                                              leds_off_all();
                                                                              digitalWrite(LED_RED,HIGH);
                                                                             }
                                                                             
                }

void leds_off_all()

                   {
                    digitalWrite(LED_RED,LOW);
                    digitalWrite(LED_GREEN,LOW);
                    digitalWrite(LED_YELLOW,LOW);
                   }
