#include <Arduino.h>
#include <SPI.h> //WI-FI
#include <ESP_Mail_Client.h>
#include <Client.h>
#include <DHT.h>   //
#include <DHT_U.h> //sensor
#include <time.h>
#include <TimeLib.h>
char ssid[] = "internet";
char pass[] = "12345678";

#define DHTPIN 2      // pinMode()
#define DHTTYPE DHT11 // sensor type
#define MAIL_SENDER "MAIL_SENDER"
#define MAIL_RECIPIENT "MAIL_RECIPIENT"
#define MAIL_SENDER_PASSWORD "MAIL_SENDER_PASSWORD"
#define MAIL_PORT 465

int TimeWinter = 3600;

DHT_Unified dht(DHTPIN, DHTTYPE);

WiFiServer server(80);
sensors_event_t event;
String temp;
String humi;
SMTPSession smtp;
IMAP_Config config;
ESP_Mail_Session session;
SMTP_Message message;

int status_S = WL_IDLE_STATUS;

void setup()
{

  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  while (status_S != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status_S = WiFi.begin(ssid, pass);

    delay(5000); // wait 5 seconds for connection:
  }
  server.begin();
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Device connected to AP");
  }

  MailClient.networkReconnect(true);
  smtp.debug(1);

  session.server.host_name = "smtp.gmail.com";
  session.server.port = MAIL_PORT;
  session.login.email = MAIL_SENDER;
  session.login.password = MAIL_SENDER_PASSWORD;
  message.sender.name = "WeatherBot";
  message.sender.email = MAIL_SENDER; // od
  message.subject = "Weather message ";
  message.addRecipient("Bot", MAIL_RECIPIENT); // do
  smtp.connect(&session);
  dht.begin();
}

void clientMessageAndWifiReconected(int delayTime)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    while (true)
    {
      Serial.println("Device disconnected to AP");
      WiFi.disconnect();
      status_S = WiFi.begin(ssid, pass);
      smtp.closeSession();
      delay(delayTime);
      smtp.connect(&session);
      if (WiFi.status() == WL_CONNECTED)
        break;
    }
  }
}

void sendTimeDelay(int hourSend, int minuteSend)
{
  delay(59999);
  while (true)
  {
    if (hour(WiFi.getTime() + TimeWinter) == hourSend && minute(WiFi.getTime()) == minuteSend) // hour(WiFi.getTime() return houer + 3600 winter time
      break;
  
  }
}

void loop()
{
  sendTimeDelay(13, 06);
  clientMessageAndWifiReconected(5000);
  dht.temperature().getEvent(&event);
  temp = String(event.temperature, 0);
  dht.humidity().getEvent(&event);
  humi = String(event.relative_humidity, 0);
  message.subject = "Weather message ";
  message.text.content = ("Temperature: " + temp + " C°\nHumidity: " + humi + " %");

  if (!MailClient.sendMail(&smtp, &message))
  {
    Serial.println(smtp.errorReason());
  }
}
