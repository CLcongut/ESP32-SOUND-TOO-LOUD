#include <Arduino.h>
#include "myI2S.h"
#include <WiFi.h>
#include <Preferences.h>
#include "DFRobotDFPlayerMini.h"

// #define wifi_SSID "CCongut"
#define wifi_SSID "CLcongut"
#define wifi_PSWD "88888888"

#define sample_buffer_size 1000

myI2S mems;
int16_t *voicedata;

// IPAddress serverIP(192, 168, 31, 199);
IPAddress serverIP(192, 168, 59, 172);
uint16_t serverPort = 6060;
WiFiClient client;
static TaskHandle_t xTCPhandler = NULL;
volatile uint32_t CompVal;

uint32_t i, j;
volatile uint64_t avg_ten;
volatile uint64_t avg_eve;

Preferences prefs;

DFRobotDFPlayerMini splayer;
bool playov = false;
volatile uint32_t Volume;

void TCPTask(void *param)
{
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(wifi_SSID, wifi_PSWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
  }
  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());
  for (;;)
  {
    Serial.println("尝试访问服务器");
    if (client.connect(serverIP, serverPort))
    {
      Serial.println("访问成功");

      while (client.connected() || client.available())
      {
        vTaskDelay(1);
        if (client.available())
        {
          String line = client.readStringUntil('\n');
          Serial.print("读取到数据：");
          Serial.println(line);
          if (line.startsWith("/"))
          {
            if (line.indexOf("set comval") > 0)
            {
              String value = line.substring(line.lastIndexOf(" ") + 1);
              CompVal = value.toInt();
              Serial.printf("设定比较值为: %d\r\n", CompVal);
              client.printf("设定比较值为: %d\r\n", CompVal);
              prefs.begin("CompareValue");
              prefs.putUInt("CompVal", CompVal);
              prefs.end();
            }
            else if (line.indexOf("set volume") > 0)
            {
              String value = line.substring(line.lastIndexOf(" ") + 1);
              Volume = value.toInt();
              splayer.volume(Volume);
              Serial.printf("设定音量为: %d\r\n", Volume);
              client.printf("设定音量为: %d\r\n", Volume);
              prefs.begin("CompareValue");
              prefs.putUInt("Volume", Volume);
              prefs.end();
            }
          }
          // client.write(line.c_str());
        }
      }
      Serial.println("关闭当前连接");
      client.stop();
    }
    else
    {
      Serial.println("访问失败");
      client.stop();
    }
    vTaskDelay(1);
  }
}

void bussz_isr()
{
  playov = false;
  digitalWrite(2, LOW);
}

void setup()
{
  Serial.begin(115200);
  voicedata = (int16_t *)calloc(sample_buffer_size, sizeof(int16_t));

  mems.begin(0, 16);
  mems.SetSampleRate(10000);
  mems.SetChannelFormat(ONLY_LEFT);
  mems.SetDMABuffer(4, 64);
  mems.SetInputMode(33, 32, 35);

  pinMode(2, OUTPUT);

  Serial2.begin(9600);
  splayer.begin(Serial2);
  splayer.outputDevice(DFPLAYER_DEVICE_SD);

  prefs.begin("CompareValue");
  CompVal = prefs.getUInt("CompVal", 0);
  Serial.printf("当前比较值为: %d\r\n", CompVal);
  Volume = prefs.getUInt("Volume", 0);
  splayer.volume(Volume);
  Serial.printf("当前音量为: %d\r\n", Volume);
  prefs.end();

  pinMode(13, INPUT);
  attachInterrupt(digitalPinToInterrupt(13), bussz_isr, RISING);

  // esp_sleep_enable_timer_wakeup(2e6);
  // esp_deep_sleep_start();

  xTaskCreatePinnedToCore(
      TCPTask,
      "TCPTask",
      4096,
      NULL,
      5,
      &xTCPhandler,
      0);
}

void loop()
{
  mems.Read(voicedata, sample_buffer_size);

  for (i = 0; i < sample_buffer_size; i++)
  {
    avg_ten += abs(voicedata[i]);
  }
  avg_ten = avg_ten / 100;

  if (avg_ten > CompVal && !playov)
  {
    digitalWrite(2, HIGH);
    Serial.println("播放！");
    randomSeed(millis());
    splayer.play(random(1, 130));
    // splayer.play(1);
    playov = true;
  }
  avg_ten = 0;
}
