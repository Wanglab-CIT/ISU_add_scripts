#include <ros.h>
#include <std_msgs/Int32MultiArray.h>
#include <WiFi.h>
#define LED 2
#define PCB_LED 21
#define PCB_SW 4

const char* ssid = "0024A533F4A7";
const char* password = "9vhb7psay4wxk";
int value;
WiFiClient client;
//IPAddress server(192, 168, 0, 2); //ROS core IP adress
IPAddress server(192, 168, 11, 20); //ROS core IP adress

class WiFiHardware {
  public:
    WiFiHardware() {};
    void init() {
      client.connect(server, 11411);
    }
    int read() {
      return client.read();
    }
    void write(uint8_t* data, int length) {
      for (int i = 0; i < length; i++)
        client.write(data[i]);
    }
    unsigned long time() {
      return millis(); // easy; did this one for you
    }
};

ros::NodeHandle_<WiFiHardware> nh;
std_msgs::Int32MultiArray stgage;
ros::Publisher messagePub("stg_array", &stgage);


void setupWiFi() {
  WiFi.begin(ssid, password);
   digitalWrite(LED, LOW);
  Serial.printf("\nConnecting to "); Serial.printf("%s\n", ssid);
  uint8_t i = 0;
  //while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
   while (WiFi.status() != WL_CONNECTED && i++ < 100) delay(500);
  ///if (i == 21) {
  if (i == 101) {
    Serial.printf("Could not connect to "); Serial.printf("%s\n", ssid);
    while (1) delay(500);
  }
  Serial.printf("Ready to use\n");
}
void setup() {
  pinMode(PCB_SW, INPUT);
  pinMode(PCB_LED, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(21, LOW);
  digitalWrite(13, LOW);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  setupWiFi();
  stgage.data_length = 4;
  stgage.data = (int32_t *)malloc(sizeof(int32_t) * 4);
  stgage.data[0] = 0.01;
  stgage.data[1] = 0.01;
  stgage.data[2] = 0.01;
  stgage.data[3] = 0.01;
  nh.initNode();
  nh.advertise(messagePub);
  // digitalWrite(5, LOW);
}
void loop() {
/*  value = digitalRead( PCB_SW );
  if(value == HIGH){
    digitalWrite(PCB_LED, HIGH);
    }*/

  if (client.connected() != true) {
    ///digitalWrite(PCB_LED, HIGH);
    digitalWrite(LED, HIGH);
    digitalWrite(PCB_LED, HIGH);
    client.connect(server, 11411);
  }
  else{
    digitalWrite(LED, LOW);
    digitalWrite(PCB_LED, LOW);
    }
  stgage.data[0] = analogRead(A0);
  stgage.data[1] = analogRead(A3);
  stgage.data[2] = analogRead(A6);
  stgage.data[3] = analogRead(A7);
  
  digitalWrite(13, !digitalRead(13));
  Serial.printf("%d %d %d %d\n", analogRead(A0), analogRead(A3), analogRead(A6), analogRead(A7));
  messagePub.publish(&stgage);
  nh.spinOnce();
  //delay(20);

}
