//#include <LiquidCrystal.h>

//#include <LiquidCrystal.h>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2, Modo_Voltaje= 6;
const int Modo_Voltaje=6;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Definir constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED
#define ALTO_PANTALLA 64 // alto pantalla OLED
#define analogpin A1
#define VA A2
#define VB A3 
#define chargepin 13
#define dischargepin 10
#define resistorValue 10000.0F

int VI = 5;
unsigned long startTime;
unsigned long elapsedTime;
float microFarads;
float nanoFarads;

float R1_Volt=1000000;
float R2_Volt=100000;
float R=1000;
float corriente=0.0;
int Voltaje_Active;
int contador = 0;

const int pulsePin = 8; // Input signal connected to Pin 8 of Arduino


int pulseHigh; // Integer variable to capture High time of the incoming pulse
int pulseLow; // Integer variable to capture Low time of the incoming pulse
float pulseTotal; // Float variable to capture Total time of the incoming pulse
float frequency; // Calculated Frequency


void setup() {
  // put your setup code here, to run once:
  //lcd.begin(16, 2);
  //lcd.print("Bienvenido");
  Serial.begin(9600);
  display.setCursor(0,2);
  display.print("Bienvenido");

   display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
   //display.clearDisplay();
   //display.display();
  pinMode (Modo_Voltaje, INPUT);
  pinMode(chargepin, OUTPUT);
  digitalWrite(chargepin, LOW);
  //lcd.init();
  //lcd.backlight();
  pinMode(pulsePin,INPUT);
  display.display();
  delay(1000);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  float Voltaje=(analogRead(0)*5.0)/1023.0;
  float Divisor_Volt=(Voltaje/(R2_Volt/(R1_Volt+R2_Volt))-0.3);
  Voltaje_Active = digitalRead(Modo_Voltaje);
  
  if (Voltaje_Active == 1 ){
      contador++;
      if(contador==6){
        contador = 1;
      }
  }

  if (contador == 1){

      display.setTextSize(1);
      display.clearDisplay(); 
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0,1); 
      display.print("Voltaje");
      //lcd.begin(16, 2);
      //lcd.print("Voltaje");
      if (Divisor_Volt<=0.10){
        display.setCursor(1, 10);
        display.print("0.0");
        Serial.print(0.0);
        Serial.print(" Voltios ");
        Serial.println();
      }
      else{    
        display.setCursor(1,10);
        display.print(Divisor_Volt);
        display.print("V");
        Serial.print(Divisor_Volt);
        Serial.print(" Voltios ");
        Serial.println();
      }
  }
  if (contador == 2){
      display.setTextSize(1);
      display.clearDisplay(); 
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0,1); 
      display.print("Resistencia");
      float avgVI=0;
      float avgVA=0;
      float avgVB=0;
      getAvgVoltajes(avgVI,avgVA,avgVB);
      Serial.print("VI: ");
      Serial.print(avgVI);
      Serial.print(" | VA: ");
      Serial.print(avgVA);
      Serial.print(" | VB: ");
      Serial.print(avgVB);
      Serial.println(); 
    
      float resistencia=getResistenciaValue(avgVI,avgVA,avgVB);
      display.setCursor(1,10);
      display.print("R: ");
      display.print(resistencia);
      display.print(" Ohm");
  }  

  if(contador==3){
    display.clearDisplay();
    digitalWrite(chargepin, HIGH);
    startTime = micros();
    while(analogRead(analogpin)<648){
    }

    elapsedTime= micros() - startTime;
    microFarads=((float)elapsedTime / resistorValue);

    if(microFarads > 1){

      display.setTextSize(1);
      //display.clearDisplay(); 
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0,1);
      display.print("Capacitancia: ");
      display.setCursor(1,10);
      display.print(microFarads);
      //display.setCursor(14,1);
      display.print("uF");
      delay(500);
      }

    else{
      nanoFarads = microFarads * 1000.0;
      display.setTextSize(1);
      //display.clearDisplay(); 
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0,1);
      display.print("Capacitancia: ");
      display.setCursor(1,10);
      display.print(nanoFarads);
      //display.setCursor(14,1);
      display.print("nF");
      delay(500);
    }

    digitalWrite(chargepin, LOW);
    pinMode(dischargepin, OUTPUT);
    digitalWrite(dischargepin, LOW);
    while(analogRead(analogpin)>0){
      }

    pinMode(dischargepin, INPUT);

    //display.setCursor(1,10);
    
  }

  if(contador == 4){
    display.clearDisplay();
    display.setCursor(8,1);
    display.print("Frequencia");
    display.setCursor(0,1);
    display.print(" ");

    pulseHigh = pulseIn(pulsePin,HIGH);
    pulseLow = pulseIn(pulsePin,LOW);

    pulseTotal = pulseHigh + pulseLow; // Time period of the pulse in microseconds
    frequency=1000000/pulseTotal; // Frequency in Hertz (Hz)
    
    display.setCursor(1,10);
    display.print(frequency);
    //display.setCursor(14,1);
    display.print("Hz");
    delay(500);
    }

  if(contador==5){
    for(int i=1; i<=20; i++){
      corriente=(float)VA*(5/1023.0)/R+corriente;
      }
    display.clearDisplay();
    display.setCursor(8,1);
    display.print("Corriente");
    display.setCursor(1,10);
    display.print(corriente*1000/20,3);
    //display.setCursor(14,1);
    display.print("mA");
    Serial.print(corriente*1000);
    Serial.print("mA");
    Serial.println();
    delay(30);
    corriente =0;
  
    }

  display.display();
  delay(1000);

}

float getVoltaje(int analogPin){
  int divideBits = analogRead(analogPin);
  float divideVoltaje = (divideBits * 5.0) / 1023;
  return divideVoltaje;
  }

float getResistenciaValue(float vi, float va, float vb){
  float resistencia = R*(vi - 2.0*(va-vb))/(vi+2.0*(va-vb));
  return resistencia;
  }

float getAvgVoltajes(float &avgVI, float &avgVA, float &avgVB){
  float VISample=0;
  float VASample=0;
  float VBSample=0;
  float referenceVoltaje=0;
  for(int i=0; i<15; i++){
    VISample += getVoltaje(VI);
    VASample += getVoltaje(VA);
    VBSample += getVoltaje(VB);
    }
  avgVI = VISample/15.0;
  avgVA = VASample/15.0;
  avgVB = VBSample/15.0;
}