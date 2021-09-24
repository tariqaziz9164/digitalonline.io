


int currentpin = A2;
int sensetivety = 66;

int offset = 2500; //2.5 volts or 2500 milli
float adcVoltage= 0;
float currentValue = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
float  adcvalue = analogRead(currentpin);
float   adcVoltage = (adcvalue / 1024.0) * 500;
currentValue = ((adcVoltage - offset) / sensetivety);
Serial.println();

Serial.print ("the curent is   =     ");
Serial.print(currentValue);
delay(1000);

}
