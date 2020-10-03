#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Servo.h> 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "hai";
const char* password = "apahayoo";

unsigned long interval = 5000;
unsigned long millisawal = 0;
int x =0;
int y =0;

HTTPClient http;
WiFiClient client;
String url2;
int S0 = D3;
int S1 = D4;
int S2 = D5;
int nilaimakan,kekeruhan;
Servo myservo;
float Ph = 0;
int tds = 0;
int aliran = D8;
const int SensorPin = A0; 
int suhu=0;
int nilai_aliran;
int kondisi_ph=0;
int kondisi_ppm=0;
int kondisi_suhu=0;
// Data signal yang digunakan yaitu pin D2
#define ONE_WIRE_BUS D2
//prosedur untuk syntax onewire dan dallastemperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
//  // connect to wifi.
  WiFi.begin(ssid, password);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
    
  myservo.attach(12);
  Serial.begin(115200);
  sensors.begin();
  pinMode(aliran, INPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(SensorPin, INPUT);
}

void bacaPH()
{   
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  
   int nilaiPengukuranPh = analogRead(SensorPin);
   double TeganganPh = 5 / 1024.0 * nilaiPengukuranPh;
   //Ph = 7.00 + ((teganganPh7 - TeganganPh) / PhStep);
   Ph = 7.00 + ((3.1 - TeganganPh) / 0.6);
   Serial.print("PH : ");
   Serial.print(Ph);
   Serial.print(" ");
   delay(1000);

   http.begin("http://aquaponik.iotcenter.id/ph_bawah.php?");
   http.GET();
   String ph_bawah = http.getString();
   client.stop();
   http.end();
   delay(500);

   http.begin("http://aquaponik.iotcenter.id/ph_atas.php?");
   http.GET();
   String ph_atas = http.getString();
   client.stop();
   http.end();
   delay(500);
   
   String ph2 = String(Ph);
   
   if (ph2 < ph_bawah || ph_atas < ph2)
   {
   kondisi_ph = 1;}
    else
   {kondisi_ph = 0;}
}

void bacatds()
{   
   delay(500);
   int nilaiPengukuranTDS = analogRead(SensorPin);
   double TeganganTDS = 5 / 1024.0 * nilaiPengukuranTDS;

  tds = (211.2254 *TeganganTDS) - 144.1466;

   Serial.print("TDS: ");Serial.print(tds);Serial.print(" ");

   http.begin("http://aquaponik.iotcenter.id/ppm_bawah.php?");
   http.GET();
   String ppm_bawah = http.getString();
   client.stop();
   http.end();
   delay(500);

   http.begin("http://aquaponik.iotcenter.id/ppm_atas.php?");
   http.GET();
   String ppm_atas = http.getString();
   client.stop();
   http.end();
   delay(500);
   
   String tds2 = String(tds);
   if (tds2<ppm_bawah || ppm_atas<tds2)
   {kondisi_ppm = 1;}
   else
   {kondisi_ppm = 0;}
}

void bacasuhu()
{
  sensors.requestTemperatures(); //Command atau syntax untuk mendapatkan data pembacaan
  Serial.print("suhu : ");
  suhu = sensors.getTempCByIndex(00);
  Serial.print(suhu); //syntax untuk menampilkan suhu yang terbaca
  Serial.print(" ");
   http.begin("http://aquaponik.iotcenter.id/suhu_bawah.php?");
   http.GET();
   String suhu_bawah = http.getString();
   client.stop();
   http.end();
   delay(500);

   http.begin("http://aquaponik.iotcenter.id/suhu_atas.php?");
   http.GET();
   String suhu_atas = http.getString();
   client.stop();
   http.end();
   delay(500);
   String suhu2 = String(suhu);
   
   if (suhu2 < suhu_bawah || suhu_atas < suhu2)
   {kondisi_suhu = 1;}
    else
    {kondisi_suhu = 0;}
}
void aliranair(){
   nilai_aliran = digitalRead(aliran);
   Serial.print ("aliran :");
   Serial.println (nilai_aliran);
}

void bacamakan()
{ 
  unsigned long millisnow = millis();
  if (millisnow - millisawal >= interval )
  {
  x++;
  Serial.println(x);
  delay(200);
  millisawal = millis();
  }
   http.begin("http://aquaponik.iotcenter.id/Makan.php?");
   http.GET();
   String nilaimakan = http.getString();
   client.stop();
   http.end();
   delay(500);
  if (nilaimakan == "1" && x>= 24 )
  {
    myservo.write(0);
    delay(2000);
    myservo.write(90);
    delay(200);
    x=0;
    }
  if (nilaimakan == "2" && x>= 12 )
  {
    myservo.write(0);
    delay(2000);
    myservo.write(90);
    delay(200);
    x=0;
    }
  if (nilaimakan == "3" && x>= 8 )
  {
    myservo.write(0);
    delay(2000);
    myservo.write(90);
    delay(200);
    x=0;
    }
    if (nilaimakan == "4" && x>= 6 )
    {
    myservo.write(0);
    delay(2000);
    myservo.write(90);
    delay(200);
    x=0;
    }
  
  }

 void kirimkeserver()
{ 
  String url = "http://aquaponik.iotcenter.id/Create.php?";
  http.begin(url+"ph_air="+Ph+"&ppm_air="+tds+"&suhu_air="+suhu+"&aliran_air="+nilai_aliran+"&kondisi_ph="+kondisi_ph+"&kondisi_ppm="+kondisi_ppm+"&kondisi_suhu="+kondisi_suhu);
   http.GET();
   client.stop();
   http.end();
   delay(5000); 
 }
void loop()
{
  bacaPH();
  delay(100);
  bacatds();
  delay(100);
  bacasuhu();
  delay(100);
  bacamakan();
  aliranair();
kirimkeserver();
}
