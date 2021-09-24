
#define SOL_AMPS_CHAN A7
//#define sol_amps_scale 0.04887585627
float vpp = 0.0048828125;
float sense = 0.066;
int pwmreading = 0;

void setup() {  pinMode(SOL_AMPS_CHAN,INPUT);
               Serial.begin(9600);

}

void loop() { pwmreading = read_adc(SOL_AMPS_CHAN);
              //sol_amps = (read_adc(SOL_AMPS_CHAN) * sol_amps_scale) -24.9;
              //Serial.print("current    ");
              //Serial.print(sol_amps);
              Serial.print("    pwm reading is   ");
              Serial.print(pwmreading);
              float volts = pwmreading * vpp;
              volts -= 2.5;
              Serial.print("   voltage is  ");
              Serial.print (volts);
              float amps = volts/sense;
              
              Serial.print("    amps  ");
              Serial.print(amps);
              
          
              Serial.println();
              delay(1000);

}

float read_adc(int channel)
                        {
                         int sum = 0;
                         int temp;
                         int i;
                         for (i = 0; i < 10; i++)
                                                {
                                                 temp = analogRead(channel);
                                                 sum += temp;
                                                delayMicroseconds(50);
                                                 }                                              
                                                return (sum / 10);
                         }
