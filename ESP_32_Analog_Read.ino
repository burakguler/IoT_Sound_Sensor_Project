#include <WiFi.h>
#include <PubSubClient.h>
 
const int LED = 18;
const int Sensor = 13;

int seviye; //sesin şiddeti
const int esik = 1100; //eşik değeri
int count=0; //sayac 
int bekleme=0; // sayacı tekrar sıfırlamak için gereken bekleme süresi

const char* ssid="burak";
const char* pass="12345678";

void setupWifi()
{
  delay(100);
  Serial.println(ssid);
  Serial.print("\nBaglaniyor");
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED)
  {
   delay(100);
   Serial.print("-"); 
  }
  Serial.println(ssid);
  Serial.print("\nBaglandi");
}

void setup()
{
pinMode (LED, OUTPUT);
Serial.begin(9600);
delay(2000);
//setupWifi();
}
void loop() {
seviye = analogRead(Sensor);
if (seviye > esik) // 1) eger bebek ağlama sesi verilen eşiği aşarsa
{
  if(count>3){ // 3) sayac 3den büyükse
    Serial.println("Bebek Agliyor"); // 4) bildirim için output ver yani bebek ağlamaya başladı.
    digitalWrite (LED, HIGH); // 5) ledi yak
    Serial.println(seviye);
    delay(2000);
    count=0;
    }
delay (200);
digitalWrite (LED, LOW); // ses eşik değerini geçmiyorsa ledi sönük bırak
count++;bekleme=0; // 2) sayacı arttır ve bekleme süresini sıfırla
}
else
{
  Serial.println(seviye);
//Serial.println(bekleme); // burda bekleme süresini kontrol ediyorum
if(bekleme>820){ //bekleme süresi, ses seviyesi eşik değeri geçmezse sıfırlanmaz 820yi geçince count sayacını sıfırlar
  count=0;bekleme=0;// böyleyece kısa kesikli seslerin verdiği yanlış bilgiden kurtullur sürekli sesi yorumlamış oluruz
}
digitalWrite(LED, LOW);
}
bekleme++;

}
