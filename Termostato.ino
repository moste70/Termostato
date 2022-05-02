
#include "DHT.h"
#include "Timer.h"
#include "U8glib.h"

#define DHT_CASTELLO_PIN 2     // Digital pin connected to the DHT sensor Castello
#define DHT_DINETTE_PIN 2     // Digital pin connected to the DHT sensor Dinette
#define DHT_MANSARDA_PIN 2     // Digital pin connected to the DHT sensor Mansarda

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dhtCastello(DHT_CASTELLO_PIN, DHTTYPE);
DHT dhtCastello(DHT_DINETTE_PIN, DHTTYPE);
DHT dhtCastello(DHT_MANSARDA_PIN, DHTTYPE);

//Gestione Display
volatile int lastEncoded = 0;
volatile long encoderValue = 0;  
long lastencoderValue = 0; 
int lastMSB = 0;
int lastLSB = 0;

U8GLIB_ST7920_128X64_1X u8g(44, 48, 46); // SPI Com: en=18,rw=16,di=17

//Sensori DHT
float dhtTempCastello;
float dhtTempDinette;
float dhtTempMansarda;

float dhtLimitTempCastello;
float dhtLimitTempDinette;
float dhtLimitTempMansarda;

int SensCastello;
int SensDinette;
int SensMansarda;

const int ValvolaCastello = 1;
const int ValvolaDinette = 2;
const int ValvolaMansarda = 3;
const int ReleRiscaldamento = 4;

boolean StatoValvolaCastello;
boolean StatoValvolaDinette;
boolean StatoValvolaMansarda;

const int MenuNone = 0; 
const int MenuCastello = 1; 
const int MenuDinette = 2; 
const int MenuMansarda = 3; 
int CurrentMenu;
int MenuState;


void setup() {
  // put your setup code here, to run once:

  //Setup sensore Castello
  StatoValvolaCastello = false;
  dhtCastello.begin();
  //Setup sensore Dinette
  StatoValvolaDinette = false;
  dhtDinette.begin();
  //Setup sensore Mansarda
  StatoValvolaMansarda = false;
  dhtMansarda.begin();

  //Setup Valvola Castello
  pinMode(ValvolaCastello, OUTPUT);
  SetValveState(ValvolaCastello,false);
  
  //Setup Valvola Dinette  
  pinMode(ValvolaDinette, OUTPUT);
  SetValveState(ValvolaDinette,false);
  
  //Setup Valvola Mansarda  
  pinMode(ValvolaMansarda, OUTPUT);
  SetValveState(ValvolaMansarda,false);
  
  //Setup Rele Riscaldamento  
  pinMode(ReleRiscaldamento, OUTPUT);
  digitalWrite(ReleRiscaldamento, LOW);
 
  //Inzializza Display
  if (u8g.getMode()==U8G_MODE_R3G3B2)        { u8g.setColorIndex(255); }
  else if (u8g.getMode()==U8G_MODE_GRAY2BIT) { u8g.setColorIndex(3); }
  else if (u8g.getMode()==U8G_MODE_BW)       { u8g.setColorIndex(1); }
  else if (u8g.getMode()==U8G_MODE_HICOLOR)  { u8g.setHiColorByRGB(255,255,255);}
  
  //Inizializza Encoder
  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  pinMode(pinEncButt,  INPUT);
 
  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);
  digitalWrite(pinEncButt,  HIGH);
 
  attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2), updateEncoder, CHANGE);
  
  //Inzializza Menù
  CurrentMenu = 0;
  MenuState = 0;
  DrawMenu();
  
}

void loop() {
  
  // Controlla Sensore Castello
  // Read temperature as Celsius (Castello)
  dhtTempCastello = dhtCastello.readTemperature();
  // Se Sensore Castello è inferiore a temperatura limite
  // Apro Valvola Castello
 If (dhtTempCastello >= dhtTempLimitCastello +1)
       SetValveState(ValvolaCastello,false);
  else if (dhtTempCastello <= dhtTempLimitCastello - 1)
        SetValveState(ValvolaCastello,true);
 
  // Controlla Sensore Dinette
  // Read temperature as Celsius (Dinette)
  dhtTempDinette = dhtDinette.readTemperature();
  // Se Sensore Dinette è inferiore a temperatura limite
  // Apro Valvola Dinette
  If (dhtTempDinette >= dhtTempLimitDinette +1)
       SetValveState(ValvolaCastello,false);
  else if (dhtTempDinette <= dhtTempLimitDinette - 1)
        SetValveState(ValvolaDinette,true);
        
  // Controlla Sensore Mansarda
  // Read temperature as Celsius (Mansarda)
  dhtTempMansarda = dhtMansarda.readTemperature();
  // Se Sensore Mansarda è inferiore a temperatura limite
  // Apro Valvola Mansarda
  If (dhtTempMansarda >= dhtTempLimitMansarda +1)
       SetValveState(ValvolaMansarda,false);
  else if (dhtTempMansarda <= dhtTempLimitMansarda - 1)
        SetValveState(ValvolaMansarda,true);
        

  DrawMenu();


  //Se uno delle 3 temperature è inferiore al linite accendo riscaldamento

//Gestione menu

  
}

void ManageMenu(void)
{
    
 switch (encoderValue)
  {
    case 0:
      //Pagina 1 Principale 
      u8g.firstPage();  
      do { drawPage1(); } while( u8g.nextPage() );   
      delay(50);
    break;  
    case 4:
      //Pagina 2 Impostazioni temperatura Letto a castello 
      //Gestione display
      u8g.firstPage();  
      do { drawPage2(); } while( u8g.nextPage() );   
      delay(50);
    break;  
    case 8:
     //Pagina 3 Impostazioni temperatura Dinette
      //Gestione display
      u8g.firstPage();  
      do { drawPage3(); } while( u8g.nextPage() );   
      delay(50);
    break;  
    case 12:
     //Pagina 3 Impostazioni temperatura Mansarda
      //Gestione display
      u8g.firstPage();  
      do { drawPage4(); } while( u8g.nextPage() );   
      delay(50);
    break;  
  }  

}

void ManageTempCastello() 
{
  switch (flagGRD)
  {
    case 0:
      if (digitalRead(pinEncButt) == LOW) 
      {
        Serial.println("ManageTempCastello Press"); 
        digitalWrite(RL5, LOW);
        digitalWrite(RL6, HIGH);
        delay(4);
        flagGRD = 1;
      }
      break;
    case 1:
      if (digitalRead(pinEncButt) == HIGH) 
      {
        Serial.println("ManageTempCastello Released"); 

        flagGRD = 2;
      }
       break;
 
    case 2:
      if (digitalRead(pinEncButt) == LOW) 
      {
        Serial.println("GRD PRESS"); 
        digitalWrite(RL5, HIGH);
        digitalWrite(RL6, LOW);
        delay(4);
        Serial.println("GRD OPEN"); 
        flagGRD = 3;
      }
      break;
    case 3:
      if (digitalRead(pinEncButt) == HIGH) 
      {
        Serial.println("GRD RELEASE"); 
        digitalWrite(RL5, HIGH);
        digitalWrite(RL6, HIGH);
        flagGRD = 0;
      }
      break;
 }
}


void SetValveState(int Valvola, boolean State)
{
    if (Valvola == ValvolaCastello)
    {
      if (State == true)
        digitalWrite(ValvolaCastello, HIGH);
      else
        digitalWrite(ValvolaCastello, LOW);
      StatoValvolaCastello = State;
    }
    else if (Valvola == ValvolaDinette)
    {
      if (State == true)
        digitalWrite(ValvolaDinette, HIGH);
      else
        digitalWrite(ValvolaDinette, LOW);
      StatoValvolaDinette = State;
    }            
    else if(Valvola == ValvolaMansarda)
    {
      if (State == true)
        digitalWrite(ValvolaMansarda, HIGH);
      else
        digitalWrite(ValvolaMansarda, LOW);
      StatoValvolaDinette = State;
    }
}

void DrawMenu(void)
{
  
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1);
  int LSB = digitalRead(encoderPin2);
  
  int encoded = (MSB << 1) |LSB;
  int sum  = (lastEncoded << 2) | encoded;
  
  if(sum==0b1101 || sum==0b0100 || sum==0b0010 || sum==0b1011) encoderValue++;
  if(sum==0b1110 || sum==0b0111 || sum==0b0001 || sum==0b1000) encoderValue--;
  
  lastEncoded = encoded; //store this value for next time
  if(encoderValue == 20)
  {
     encoderValue = 0;
  }
}
