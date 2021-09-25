float Vsolar;
float Isolar;
float Psolar;
float PsolarPrev;
float deltaPsolar = Psolar - PsolarPrev;
float deltaPwm = pwm - pwmOld;
float pwmOld ;
float pwm;
void loop()
           {
         if  ( deltaPsolar >= 0)
                               {
                                if(deltaPwm >= 0)
                                                 {
                                                  pwm = pwm + deltaPwm;
                                                 }
                                                else {
                                                     pwm = pwm - deltaPwm;
                                                     }
                               }

                    if(deltapwm >= 0)
                                     {
                                      pwm = pwm - deltaPwm
                                     }

                                 pwm = pwm + deltaPwm;    
           }
           PsolarPrev = Psolar;
           pwmOld = pwm;


