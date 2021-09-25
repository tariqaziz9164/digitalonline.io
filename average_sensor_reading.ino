const float reffvolts = 5;
const float   r1 = 100000;
const float   r2 = 10000;
const float   resistorFactor =(5/ 1024)*(r1+r2)/r2;
int           batteryVoltPin = 0;
int           batteryAmpPin     =  1;
const float solAfactor = (5/1024)/0.1;

                                            void setup() {
                                                            Serial.begin(9600);
         
                                                           }

                                            void loop() {  
                                                          float BatVolt =   averagefun( batteryVoltPin) * 0.0537108;      //0.01635750;
                                                                                        //    float solA  = averagefun(solarA) * 0.048828125;                   // 0.0537669;
                
                                                          Serial.print("BatteryVoltage  =   " ) ;
            
                                                          Serial.println(BatVolt);              //- 0.26 );
                                                                                       //   Serial.print("solar current  =  ");
                                                                                       //  Serial.println (solA);
                                                      
             

                                                         float q = 0.5 * 5.0;          //for current measuring calibration.
                            
                                                         float batAmp = averagefun(batteryAmpPin);
                            
                                                         float AmpValue = 5.0 / 1024 *  batAmp;
                            
                                                         float raw = AmpValue - q + 0.012;
              
                                                         float current = raw / 0.100;   //for 20 amp current sensor..85 for 10 amp sensor
             
                                                         Serial.print("battery Ampear  =    ");
                                                         Serial.println( current );
             
      
                                                         delay (1000);
     

                                                       }
 float averagefun(int pin)
                         {
                          
                          float sum = 0;
                          float temp;
                          for(int i = 0; i<9 ; i++)
                                             {
                
                                                 temp = analogRead(pin);             // * 0.01635750 );
                                                                                 //  float AmpValue = analogRead(batteryAmpPin);                         // * 0.026393581);
                                                
                                                sum += temp;
                                                delayMicroseconds(50);
                                              }
                 return(sum/10) ;

                          }
