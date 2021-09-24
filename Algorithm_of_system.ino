#define Svpin A0
#define BVPinn A1
#define SIPin A2

float Vnew = 0.0;
float Inew = 0.0;
float Pnew = Inew * Vnew;
float Pold = 0.0;
float Vold = 0.0;


   if (Pnew > Pold)
                    { 
                     
                      if (Vnew > Vold)
                                    {
                                      Pnew = Vnew * Inew;
                                      Pold = Pnew;
                                      Vold = Vnew;
                                    }
                            else if (Vnew > Vold)
                                       {
                                        pwm = pwm -1;
                                                        
                                        }
                      
                    }

