#include <WiFi.h>
#include <PubSubClient.h>

const int LED = 18;  //Esp-32 Module's 18. pin
const int Sensor = 12; //Esp-32 Module's 12. pin
int seviye=0;
int count=0;  //first count for 
int bekleme=0; 
const char* ssid = "burak";
const char* password = "12345678";
const char* mqtt_server = "192.168.137.56"; //yours mqtt server ip
  
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') 
  {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } 
  else 
  {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("deneme", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() 
{
  seviye = digitalRead(Sensor); // data getting with digitalread 
  Serial.println(seviye);
  delay(200);
  if (seviye == 1)//eger bebek ağlama sesi verilen eşiği aşarsa
  {
    if(count>3)
    { // 3) sayac 3den büyükse
        Serial.print(1); 
        Serial.println("Bebek Ağlıyor!");// 4) bildirim için output 1 ver yani bebek ağlamaya başladı.
        digitalWrite (LED, HIGH); // 5) ledi yak
      snprintf (msg, 50, "%ld", 3); //bebek aglarken influxdbye 3 değeri publish et
        Serial.print("Publish message: ");
        Serial.println(msg);
        client.publish("deneme", msg);
      long now = millis();
        lastMsg = now;       
        delay(1000);
        count=0;
     }
    delay (200);
    digitalWrite (LED, LOW); // ses eşik değerini geçmiyorsa ledi sönük bırak
    count++;bekleme=0; // 2) sayacı arttır ve bekleme süresini sıfırla
  }
  else
  {
    Serial.println(bekleme); // burda bekleme süresini kontrol ediyorum
    if(bekleme>820)
    { //bekleme süresi, ses seviyesi eşik değeri geçmezse sıfırlanmaz 820yi geçince count sayacını sıfırlar
      count=0;bekleme=0;// böyleyece kısa kesikli seslerin verdiği yanlış bilgiden kurtullur sürekli sesi yorumlamış oluruz
    }
    digitalWrite(LED, LOW);
  }
  
  if (!client.connected()) {
      reconnect();
  }
    client.loop();
  
    long now = millis();
   if (now - lastMsg > 2000) 
   {
      lastMsg = now;
      snprintf (msg, 50, "%ld", 1); // eğer ses verisi gelmiyorsa veri tabanına 
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("deneme", msg); //deneme topicine msg gönderiyoruz
   }
  
}
