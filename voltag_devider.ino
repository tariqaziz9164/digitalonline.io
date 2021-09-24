const float reffvolts = 5;
const float r1 = 4700; //ohm
const float r2 = 2000; //ohm
const float resistorFactor =5/ 1023.0*(r2/(r1+r2));   //conceder a constant at current stage......Basic formula need algorithm depend on it.
const int batteryPin = 0;                             //battery voltage initiol value.

void setup() {
              Serial.begin(9600);
              pinMode (  batteryPin ,OUTPUT);
             }

void loop() {
              int value = analogRead(batteryPin  );

              
              Serial.println(value);
              float volts = value * 0.00145754 ;                                          
              float  voltag= value *0.0163;                                                   
              
             Serial.println(voltag );                   // caliberation for actual voltage
              delay (1000);

}


                                                        //An algorithm based system design.

int average (int valuer){
                            int sum =0 ;
                            int temp  ;
                               for(int i= 0;i< 4; i++){ 
                                                         temp = analogRead(valuer);
                                                         sum+=temp;
                                                         delayMicroseconds(50);
                                                        }
                                                                                    //  Serial.print(sum/4 *0.029296875 - 11.6); for dubugging
                                                   
                                                      return sum/4 * 0.029296875;      
                                            
                          }
