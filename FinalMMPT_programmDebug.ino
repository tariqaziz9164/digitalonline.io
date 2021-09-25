//#include <LiquidCrystal_I2C.h>
#include "TimerOne.h"
//#include <Wire.h>



#define Vsolar_sensor  A0
#define Isolar_sensor  A1
#define Vbatt_sensor   A2
#define Ibatt_sensor   A3
#define SD_pin          8  //IR2104 or IR2110 SD pin
#define IN_pin          9  //IR2104 or IR2110 pwm pin
#define load_pin        0
//#define COMMON_ANODE     // Uncommented if common anode
#define R    5
#define G    6
#define B    3

//#define MPPT_INCCOND  
#define Vsolar_factor 0.045085461                                     //(5/1024)*(R2 + R1)/R1; as R1 = 9.85K and R2 = 81.1K.
#define Vbatt_factor  0.014938902                                    //(5/1024)*(R2 + R1)/R1; as R1 = 9.75K  and R2 = 20.08K
#define Isolar_factor 0.048828125                                   //(5/1024)/0.1; sensitivity of ACS712 20A modele is 100mV/A
#define Ibatt_factor  0.048828125                                  //(5/1024)/0.1; sensitivity of ACS712 20A modele is 100mV/A

#define Ioffset1 24.9   // 2.5/0.15/1024/0.1,,sensitivety ACS712-20A 100mv/A and voltage at 0A is 2.5V(in this case in real it is 24.9
#define Ioffset2 24.8  //same as above but ACS712-20A 2nd sensor.


#define MOSFET_OFF digitalWrite(SD_pin,LOW)   // here an optocupler PC817 or any other,just turn off its input.
#define MOSFET_ON  digitalWrite(SD_pin,HIGH)
#define LOAD_ON    digitalWrite(load_pin,HIGH)
#define LOAD_OFF   digitalWrite(load_pin,LOW)
#define pwm_max    100 
#define pwm_top    1024
#define pwm_min    28  
#define pwm_stepsize 1
#define pwm_start    60     //optimal starting pwm 60% duty
#define sampale_no   10





//global variables
//float Tbatt = 25; uncom if temp sensor is use.
//float celcius;

float Vsolar ;
float PrevVsolar = 0.0 ;
float Isolar ;
float PrevIsolar = 0.0;
float Vbatt; 
float Ibatt;
float Psolar;
float Pleast = 2.0;
float PrevPsolar = 0.0;
unsigned int interrupt_counter = 0;
unsigned long boost_millis = 0;     // stor the boost, absorptoin ,topping time for the day.
float delta = pwm_stepsize;         //change pwm in bulk mode
float inc = pwm_stepsize;          // change pwm in boost and float mode
float error1 = 0.0;                 // error during maintain absorption voltage
float error2 = 0.0;                //error during maintain float voltage
float error3 = 0;               // error during avoiding exceeding max charging voltage
float pwm = 0;

//       Battery set Points

float Vfloat = 13.60; // by default
float Vfloat1; //to be set by user
char  strfloat[] = {"13.60"};  //string data types.strfloat is a arry of charector
float Vboost =14.50 ;  //by default
float Vboost1;
char strboost[] = {"14.50"};
float LVD = 11.00;    // by default              //LOW VOLTAGE DISCONECT.
char strLVD[] = {"11.00"};

unsigned long boost_settime= 060;  // by default
float boost_settime1;
char strboostTime[] = {"060"};
unsigned long boost_time;

char strBattType[] = {"12"};

float temp_compensation;
float tempComp1;
char strtempCopm[] = {"3.0"}; // 3milli vots per cell.

int Imax = 20;
int Tmax =40;  // max battery temperatur for god operation a fan wiill needed to cool down or mantain at fix teperatur.if battery temp increas from 40 then do so.

enum charger_mode {off,initialize,bulk,boost,float_mode}charger_mode;
//enum led_mode{red,green,blue,purple,black}led_mode;

//LiquidCrysral_I2CLcd(0X3F,2,1,0,,4,5,6,7,3,POSITIVE);  //Set LCD I2C addres for 2004 display by syntax : hexaaddress,en,rw,rs,D4,D5,D6,D7,bl,blpol

// uncoment if keypad will use.
//keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);

//set up teperature sensor dsi8b2 Dallas teperature.
//OneWire oneWire(ONE_WIRE_BUS);


void setup() { 
               pinMode ( SD_pin,OUTPUT);
               pinMode (IN_pin,OUTPUT);
               pinMode ( R , OUTPUT ); pinMode ( G ,OUTPUT ); pinMode ( B , OUTPUT );
               pinMode (load_pin,OUTPUT);
               MOSFET_OFF;
               charger_mode = off;
            // lcd.begin(20,4);   //initialize LCD 2004 display (20 columns 4 rows)' uncoment if want to use.
           //  lcd.backlight();
            // for(int batcicon = 1;baticon < 7 ; baticon++)
                                                       //     { lcd.createChar(baticon,batt_icons[baticon]); }
                                                            
                                                        //      lcd.createChar(PV_ICON,pv_icon);
                                                        //      lcd.createChar(PWM_ICON,pwm_icon);

                                                        //      led_mode = black ;

                                                              Timer1.initialize(20);   // initialize timer1 at a 20 micro second period,frequency = 50kHz
                                                              Timer1.pwm(IN_pin , 0);   // set in pin of mosfit drive to 0.
                                                              pwm = pwm_start;   // start from 60%' pwm_start = 60.

                                                           //   keypad.addEventListener(keypadEvent);

                                                            //    lcd.setCursor(0,0);
                                                            //    lcd.print ("PV");
                                                            //    lcd.setCursor (3,0);
                                                            //    lcd.write(PV_ICON);
                                                            //    lcd.setCursor(7,0);    //uncoment if lcd will use.
                                                            //    lcd.print("PWM");
                                                            //    lcd.setCursor (11,0);
                                                            //   lcd.write(PWM_ICON);
                                                            //    lcd.setCursor(14,0);
                                                            //    lcd.print("BATT");
                                                                
                                                                  
                                                              
                                                              
                                                              
             }

void loop() {      read_sensors();
                  // keypadmode (); 
                   run_MPPTcharger();
                  // lcd_display();  
                //   led_display();
                led_output();
                   load_engage();
                 //  resetting();
             }  
                                           
                                                              
                                                             
             
int sample_sensor(int Inputpin) 
                                 {
                                   int sum = 0;
                                   int temp ;

                                   for(int k= 0 ;k< sampale_no;k++) { temp = analogRead(Inputpin);
                                                                     sum+=temp;
                                                                     delayMicroseconds(50);
                                                                    }
                                                             return(sum/sampale_no);
                                                            
                                 }


void read_sensors(void) {   Vsolar = sample_sensor(Vsolar_sensor)*Vsolar_factor;
                           Vbatt = sample_sensor(Vbatt_sensor) * Vbatt_factor;
                           Isolar =( sample_sensor(Isolar_sensor) * Isolar_factor) - Ioffset1;    // offset to minus some value for exact caliberatio.
                           Ibatt =( sample_sensor (Ibatt_sensor) * Ibatt_factor) -Ioffset2; // if not work so use.. Isolar sample_sensor = (sample_sensor(IbatSensor)*IbattFactor);
                           
                           Psolar = Vsolar * Isolar;
                           

                           
                                                  //uncomment if sensor to be use.
                         //  sensors.begin();   for temperatue sensor ds18b20 modle.
                         //   int b = (sensors.getDeviceCount()); if (b=10)   {sensors.requestTemperatures(); celcius=sensor.getTempCByIndex(0); Tbatt=celcius;}
                        //  else {Tbatt=25;}
                        
                       //     Vfloat = (Vfloat1 + (25 - Tbatt ) * temp_compensation);
                       //     Vboost = (Vboost1 + (25 - Tbatt ) * temp_compensation;

                         }

void set_pwmdutycycle(float pwm) {

                                    if ( pwm < 28)
                                                      { pwm = 28;}
                                      else if (pwm > 100 || pwm == 100) 
                                                      { pwm = 100; Timer1.pwm(IN_pin,(1024 - 1),20); } //maximum duty cycle 99.9 %.

                                      else if ( pwm < pwm_max && pwm > pwm_min) 
                                                      { Timer1.pwm( IN_pin,(1024) * (long)pwm/100,20); } // set pwm at 50kHz //1024*pwm value / 100 = duty cycle =  puls ON time.
                                                      

                                 }


void run_MPPTcharger()  { 

                          switch(charger_mode)
                                               { 
                                                case off:
                                                      //    MOSFET_OFF;
                                                          delay(1500);

                                                          if(((Vbatt > 12 )&&( Vsolar>Vbatt+0.5)||(Vbatt<=12)&&( Vsolar>=12.5))) {
                                                                                                                                   charger_mode= initialize;
                                                                                                                                 }
                                                                                                                                   break;
                                                                                                                                   
                                                          
                                                case initialize: off_conditions();
                                                    /* if(((Vbatt>12)&&(Vsolar< Vbatt+0.5)||(Vbatt<= 12)&&(Vsolar<12.5)||(Isolar<0.1)||(Ibatt>Imax)||(Vbatt>Vboost+0.15)||Psolar<Pleast))
                                                             // || Tbatt>Tmax )) if temperatur sensor is use then uncomment and include in if statment
                                                           {
                                                            charger_mode = off;
                                                             MOSFET_OFF;
                                                            break; 
                                                              
                                                           }*/

                                                             
                                                                {
                                                                 if(boost_time>boost_settime) { 
                                                                                               charger_mode = float_mode;
                                                                                              }
                                                                   else 
                                                                       {
                                                                         if (Vbatt>Vboost) {
                                                                         
                                                                                            charger_mode = boost; 
                                                                                            MOSFET_ON;
                                                                                            boost_millis=millis(); 
                                                                                            break;
                                                                                           }
                                                                                                else 
                                                                                                    { charger_mode = bulk;
                                                                                                      MOSFET_ON;
                                                                                                      break;
                                                                                                    }
                                                                        }
                                                                  }
                                                      break;

              case bulk:
                        if(((Vbatt>12)&&(Vsolar<Vbatt+0.5)|| (Vbatt<= 12) &&( Vsolar < 12.5) || (Isolar<0.1)||(Ibatt>Imax)||(Vbatt>Vboost+0.15)||Psolar<Pleast))//||Tbatt>Tmax))
                              {
                               charger_mode = off;
                               MOSFET_OFF;
                               break;
                               } 
                                  else {  if ( Vbatt > Vboost+0.01)
                                                                   { if (boost_time > boost_settime)
                                                                       {
                                                                        charger_mode = float_mode;
                                                                        MOSFET_ON;
                                                                        break;
                                                                       }

                                                                         else {
                                                                               charger_mode = boost;
                                                                               MOSFET_ON;
                                                                               boost_time= millis();
                                                                               break;
                                                                              }
                                                                   }   
                                                  else { //#ifdef MPPT_Pand0 mppt_Pand0(); #endif 
                                                       //  #ifdef MPPT_INCCOND mppt_INCCOND();  #endif

                                                        if(Ibatt> Imax-1) 
                                                           {
                                                             error3 = Ibatt-(Imax-1);
                                                             inc = 300*delta*error3;
                                                             pwm-=inc;
                                                             set_pwmdutycycle(pwm);
                                                           }
                                                            
                                            break;     }
                                                                                                                                          
                                      }
               case boost :
                            boost_time = boost_time + millis()-boost_millis;
                            boost_millis= millis();

                              if (((Vbatt > 12)&&(Vsolar<Vbatt+0.5) || (Vbatt<=12)&&(Vsolar< 12.5)||(Ibatt>Imax)||(Isolar< 0.1)||(Vbatt>Vboost+0.15)||Psolar<Pleast))//||Tbatt>Tmax))
                                                                           {
                                                                            charger_mode = off;
                                                                            MOSFET_OFF;
                                                                            break;
                                                                           }
                                      else   
                                           {
                                             if(Vbatt<Vboost - 0.25)
                                                                     {
                                                                      charger_mode= bulk;
                                                                      MOSFET_ON ;
                                                                      break;
                                                                      
                                                                     }
                                                 else {
                                                        if(boost_time > boost_settime)
                                                                                       {
                                                                                        charger_mode = float_mode;
                                                                                        MOSFET_ON ;
                                                                                        break;
                                                                                                           
                                                                                       }

                                                              else {
                                                                     if(Vbatt>Vboost+ 0.02)
                                                                                    {
                                                                                      error1= Vbatt-(Vboost+0.02);
                                                                                      inc = delta*500*error1;  //if battery voltage > 14.2 the pwm to be decrease by % error.the pwm is zero 
                                                                                                                 //at 14.6
                                                                                      pwm-=inc;
                                                                                      set_pwmdutycycle(pwm);
                                                                                      boost_time =  boost_time+millis()-boost_millis;                                                    
                                                                                    }


                                                                                    if(Ibatt>Imax-1)
                                                                                                     {
                                                                                                      error3 = Ibatt-(Imax-1);
                                                                                                      inc = 300*delta*error3;  //if battery current greater then 19 decrease pwm by error %;
                                                                                                      pwm-=inc;
                                                                                                      set_pwmdutycycle(pwm);
                                                                                                      
                                                                                                     }
                                                            break;  }
                                                      }
                                           }

                    case float_mode:
                                 if(((Vbatt > 12)&&(Vsolar<Vbatt+0.5) || (Vbatt<=12)&&(Vsolar< 12.5)||(Isolar< 0.1)||(Ibatt>Imax)||(Vbatt>Vboost+0.15)|| Psolar<Pleast))//||Tbatt>Tmax))
                                                                                                              {
                                                                                                                charger_mode = off;
                                                                                                                MOSFET_OFF;
                                                                                                                break;
                                                                                                              }

                                      else  {
                                             if(Vbatt<(Vfloat-0.1))
                                                                {
                                                                  charger_mode = bulk;
                                                                  MOSFET_ON;
                                                                  break;
                                                                }
                                             

                                             else 
                                                 { 
                                                  if(Vbatt>Vfloat+0.02)
                                                                     { error2 = Vbatt-(Vfloat+0.02);
                                                                      inc = delta*500*error2;
                                                                      pwm -= inc;
                                                                      set_pwmdutycycle(pwm);


                                                                              if(Ibatt>Imax-1)
                                                                                  {
                                                                                    error3 = Ibatt-(Imax-1);
                                                                                    inc = 300*delta*error3;  // if battery current > 19 decrease pwm by error3 %.
                                                                                    pwm-=inc;
                                                                                    set_pwmdutycycle(pwm);
                                                                                    
                                                                                  }
                                                                   break;}
                                                 }
                                              }

                              default: 
                                       charger_mode = off;
                                       MOSFET_OFF;
                                       break;

                                          }
                                                                      

                                           
                                  }
                                                 

  void mppt_INCCOND()
                     {
                      float delta_P = Psolar - PrevPsolar;
                      float delta_V = Vsolar - PrevVsolar;
                      float delta_I = Isolar - PrevIsolar;
                      float gradient = delta_P/delta_V;

                                  if(delta_V!=0)
                                     {
                                       if(gradient>0)
                                            {
                                              pwm+=delta;
                                              PrevPsolar = Psolar;
                                              PrevVsolar = Vsolar;
                                              PrevIsolar = Isolar;
                                              set_pwmdutycycle(pwm);
                                            } 
                                              else if(gradient<0)
                                                     {
                                                      pwm-=delta;
                                                      PrevPsolar = Psolar;
                                                      PrevVsolar = Vsolar;
                                                      PrevIsolar = Isolar;
                                                      set_pwmdutycycle(pwm);
                                                     }
                                     }

                                            else{
                                                  if(delta_I>0)
                                                     {
                                                      pwm+=delta;
                                                      PrevPsolar = Psolar;
                                                      PrevVsolar = Vsolar;
                                                      PrevIsolar = Isolar;
                                                      set_pwmdutycycle(pwm);
                                                     }
                                                      else if(delta_I<0)
                                                             {
                                                              pwm-=delta;
                                                              PrevPsolar = Psolar;
                                                              PrevVsolar = Vsolar;
                                                              PrevIsolar = Isolar;
                                                              set_pwmdutycycle(pwm);
                                                             }
                                                      }
                     }
                     
                                              /*    void led_display(void)
                                                                       { 
                                                                        if (Vbatt>Vboost+0.2)        { led_mode = purple;}
                                                                            else if (Vbatt>12.5 && Vbatt<Vfloat)  { led_mode=green;}
                                                                                   else if (Vbatt<11.75 )         {led_mode = red;}
                                                                                           if (Vbatt<12.5 || Vsolar<0.25) { led_mode = blue;}
                                                                                          
                                                                            
                                                                       
                                     
                                                                            switch(led_mode)
                                                                                    {
                                                                                      case red:
                                                                                               setColor(255,0,0); break;

                                                                                      case green:
                                                                                               setColor(0,255,0); break;
 
                                                                                      case blue:
                                                                                               setColor(0,0,255); break;

                                                                                    //  case yellow:
                                                                                             //  setColor(255,255,0); break;

                                                                                      case purple:
                                                                                                setColor(80,0,80); break;

                                                                                     // case orange:
                                                                                               // setColor(255,128,0); break;

                                                                                    //  case black:
                                                                                               // setColor(0,0,0); break;
                                                                                    }
                                                                       }

                      void setColor(int r,int g,int b)
                                                      {
                                                       // #ifdef  COMMON_ANODE r = 255-r; g = 255-g; b = 255-b; #endif
                                                        analogWrite(R,r);
                                                        analogWrite(G,g);
                                                        analogWrite(B,b);
                                                      }    */

                     void load_engage()
                                       {
                                        if(Psolar<Pleast && Vbatt>LVD)
                                                                      {LOAD_ON;}

                                                                      else
                                                                          {
                                                                            if(Psolar>Pleast && Vbatt>Vfloat) { LOAD_ON;}
                                                                            else {LOAD_OFF;}
                                                                          }
                                         LOAD_OFF;
                                       }
                                                                        
 void led_output(void)
                {
                 if ( Vbatt > 14.4 )
                                  {
                                    leds_off_all();
                                    digitalWrite(B,HIGH);                                                                       
                                  }
                                  else if(Vbatt>12 && Vbatt < 14.4)
                                                                  { 
                                                                    leds_off_all();
                                                                    digitalWrite (G,HIGH);
                                                                  }
                                                             else if(Vbatt<12)
                                                                             {
                                                                              leds_off_all();
                                                                              digitalWrite(R,HIGH);
                                                                             }
                  }     

void leds_off_all()

                   {
                    digitalWrite(R,LOW);
                    digitalWrite(G,LOW);
                    digitalWrite(B,LOW);
                   }
void off_conditions(void)
                        {
                            if(((Vbatt>12)&&(Vsolar< Vbatt+0.5)||(Vbatt<= 12)&&(Vsolar<12.5)||(Isolar<0.1)||(Ibatt>Imax)||(Vbatt>Vboost+0.15)||Psolar<Pleast))
                                                             // || Tbatt>Tmax )) if temperatur sensor is use then uncomment and include in if statment
                                                           {
                                                            charger_mode = off
                                                            MOSFET_OFF;
                                                            break; 
                                                               
                                                           }
   
                        }

