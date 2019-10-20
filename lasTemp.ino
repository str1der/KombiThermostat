#include <DHT.h> //DHT sensor kütüphanesi
#include <SPI.h> // SPI haberleşme kütüphanesi 
#include <LiquidCrystal_I2C.h> //LCD ekran kütüphanesi 
#include <Wire.h> // Wire kütüphanesi
#include <RF24.h> //RF24 kütüphanesi kablosuz haberleşme için

//Hardware Configuration: SPI haberleşme binlerini arduino SPI pinlerine (11,12,13(MOSI, MISO, SCK))
//diğer pinleri CE, 7 ve CSN,8 pinlerine bağlıyoruz.
RF24 radio(7, 8);
byte address[][7] = {"1Dugum", "2Dugum"};
char onay[5] = "onay";
char data = '0';



#define DHTPIN 2 //DHT pin bağlantısı
#define DHTTYPE DHT11 //DHT sensör tipi 

#define SETPOT A0 // Potansiyometre ile alınan değer
int sicaklik;
int lastTemp;


DHT dht(DHTPIN, DHTTYPE);

//LCD bağlantı ayarları
#define LCDI2C 0x27
#define LCDCOL 16
#define LCDROW 2

LiquidCrystal_I2C lcd(LCDI2C, LCDCOL, LCDROW);

byte Logo[][8] = {
  {B00100, B11010, B01010, B11010, B01010, B10001, B10001, B01110}, //termostat sembolü
  {B00000, B00000, B00000, B00100, B01010, B10011, B10111, B01110}, //damla sembolü
  {B00000, B11111, B10001, B10111, B10001, B10111, B10001, B11111}, //Ev işareti
  {B11111, B11111, B10001, B01010, B00100, B00100, B00100, B00100}, //Anten
  {B00000, B00000, B00001, B00001, B00101, B00101, B10101, B10101}, //Sinyal
  {B10100, B01000, B10101, B00001, B00101, B00101, B10101, B10101}  //Sinyal yok
};


// ----------------------------------------------------------------------------------
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// ----------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  radio.begin();

  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);

  pinMode(DHTPIN, INPUT);
  dht.begin();

  pinMode(SETPOT, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, Logo[0]); //termostat sembolü
  lcd.createChar(1, Logo[1]); //damla sembolü
  lcd.createChar(2, Logo[2]); //Ev işareti
  lcd.createChar(3, Logo[3]); //Anten
  lcd.createChar(4, Logo[4]); //Sinyal
  lcd.createChar(5, Logo[5]); //Sinyal yok

  lastTemp = dht.readTemperature();
}


void loop() {

  //İstenilen sıcaklığın ayarlanması
  int setTemp = analogRead(SETPOT);
  setTemp = map(setTemp, 0, 1023, 0, 39);

  // Kombiyi kontrol edecek ayarlar.
  sicaklik = dht.readTemperature();

  if (lastTemp - setTemp <= 0) {
    data = '1';
    lastTemp = sicaklik;
  } else if (lastTemp - sicaklik == 2) {
    data = '1';
    lastTemp = sicaklik;
  } else {
    data = '0';
  }

  // Debugging
  //Serial.println(lastTemp);


  //Radio haberleşme

  radio.stopListening();
  radio.write(&data, sizeof(data) );
  char rep[5];
  radio.startListening();



  //Ekrana yazdır.
  lcd.setCursor(0, 0);
  lcd.write(2);
  lcd.print("Ayar: ");
  lcd.print(setTemp);
  lcd.print(" \337C ");
  lcd.setCursor(14, 0);
  lcd.write(byte(3));
  if (radio.available()) {
    radio.read(&rep, sizeof(rep));
    if (strcmp(rep, "onay") == 0) {
      lcd.setCursor(15, 0);
      lcd.write(4);
    }
  } else {
    lcd.setCursor(15, 0);
    lcd.write(5);
  }
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.print(dht.readTemperature());
  lcd.print("\337");
  lcd.print("C");
  lcd.print(" ");
  lcd.write(1);
  lcd.print(dht.readHumidity());
  lcd.print("%");
}
