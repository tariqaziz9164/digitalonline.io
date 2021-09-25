int analogpin = A0;

float vout = 0.0;
float vin = 0.0;
float R1 = 1000000.0;
float R2 = 10000.0;
int pwmread = 0;

void setup() {
              Serial.begin(9600);
              
             

              }

void loop() {
              pwmread =  analogRead(analogpin);
              Serial.print("pwm values in circuit = ");
              Serial.print(pwmread);
               
              vout = pwmread * 4.9 /1024.0;
              Serial.print("    out put voltage  = ");
              Serial.print(vout);
              
              vin = vout / 0.0909090909  ;                          //(R2/(R1+R2));  // R1 = 100000 R2 = 10000
              Serial.print("     input voltage is  = ");
              Serial.print(vin);
              
              if (vin < 0.09) 
                           {
                            vin = 0.0;
                           }
              Serial.println();
             
          /*    Serial.println(); 
              Serial.print("out put vout          = ");
              Serial.print(vout);
              Serial.println();
              Serial.print("the in put vin        = ");
              Serial.print(vin);*/
           
             
                   delay(1000);

             }
