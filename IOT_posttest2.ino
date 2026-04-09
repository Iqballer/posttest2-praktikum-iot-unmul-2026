#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <DHT.h>

const char* ssid = "ball";
const char* password = "tplu1234";

// Telegram
#define BOT_TOKEN "8637281652:AAFmsUcdY_VbknK4TgyQuEfE25vNEKQxwnc"
#define GROUP_ID "-1003803563175"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

#define LED_A 14
#define LED_B 27
#define LED_C 26
#define LED_UTAMA 25

#define DHTPIN 32
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define MQ2_PIN 34
#define GAS_THRESHOLD 350

// User
#define USER_A "1145474235" // ID Iqbal
#define USER_B "6517860580" // ID Byhaqi
#define USER_C "8691049495" // ID Avin

bool gasAlertSent = false;
unsigned long lastTime = 0;
unsigned long botDelay = 1000;

// Wifi
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  client.setInsecure(); // WAJIB
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_UTAMA, OUTPUT);

  dht.begin();
  connectWiFi();
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {

    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String user_id = String(bot.messages[i].from_id);

    Serial.println("Pesan masuk: " + text);

    if (text.startsWith("/start")) {
      String welcome = "🏠 *SMART HOME BOT*\n\n";
      welcome += "Halo 👋\n\n";
      welcome += "📌 *COMMAND:*\n";
      welcome += "/ledA_on /ledA_off\n";
      welcome += "/ledB_on /ledB_off\n";
      welcome += "/ledC_on /ledC_off\n";
      welcome += "/ruang_on /ruang_off\n";
      welcome += "/cek_suhu\n\n";
      welcome += "⚠️ Gunakan sesuai akses!";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }

    // ===== LED A =====
    if (text.startsWith("/ledA_on")) {
      if (user_id == USER_A) {
        digitalWrite(LED_A, HIGH);
        bot.sendMessage(chat_id, "💡 LED A MENYALA", "");
      } else {
        bot.sendMessage(chat_id, "⚠️ AKSES DITOLAK! Bukan LED kamu", "");
      }
    }

    if (text.startsWith("/ledA_off")) {
      if (user_id == USER_A) {
        digitalWrite(LED_A, LOW);
        bot.sendMessage(chat_id, "🌑 LED A MATI", "");
      } else {
        bot.sendMessage(chat_id, "⚠️ AKSES DITOLAK! Bukan LED kamu", "");
      }
    }

    // ===== LED B =====
    if (text.startsWith("/ledB_on")) {
      if (user_id == USER_B) {
        digitalWrite(LED_B, HIGH);
        bot.sendMessage(chat_id, "💡 LED B MENYALA", "");
      } else {
        bot.sendMessage(chat_id, "⚠️ AKSES DITOLAK! Bukan LED kamu", "");
      }
    }

    if (text.startsWith("/ledB_off")) {
      if (user_id == USER_B) {
        digitalWrite(LED_B, LOW);
        bot.sendMessage(chat_id, "🌑 LED B MATI", "");
      } else {
        bot.sendMessage(chat_id, "⚠️ AKSES DITOLAK! Bukan LED kamu", "");
      }
    }

    // ===== LED C =====
    if (text.startsWith("/ledC_on")) {
      if (user_id == USER_C) {
        digitalWrite(LED_C, HIGH);
        bot.sendMessage(chat_id, "💡 LED C MENYALA", "");
      } else {
        bot.sendMessage(chat_id, "⚠️ AKSES DITOLAK! Bukan LED kamu", "");
      }
    }

    if (text.startsWith("/ledC_off")) {
      if (user_id == USER_C) {
        digitalWrite(LED_C, LOW);
        bot.sendMessage(chat_id, "🌑 LED C MATI", "");
      } else {
        bot.sendMessage(chat_id, "⚠️ AKSES DITOLAK! Bukan LED kamu", "");
      }
    }

    // ===== LED UTAMA =====
    if (text.startsWith("/ruang_on")) {
      digitalWrite(LED_UTAMA, HIGH);
      bot.sendMessage(chat_id, "🏠 Lampu ruang tamu MENYALA", "");
    }

    if (text.startsWith("/ruang_off")) {
      digitalWrite(LED_UTAMA, LOW);
      bot.sendMessage(chat_id, "🌙 Lampu ruang tamu MATI", "");
    }

    // ===== DHT =====
    if (text.startsWith("/cek_suhu")) {
      float t = dht.readTemperature();
      float h = dht.readHumidity();

      if (isnan(t) || isnan(h)) {
        bot.sendMessage(chat_id, "❌ Sensor error!", "");
        return;
      }

      String msg = "🌡 Suhu: " + String(t) + "°C\n";
      msg += "💧 Kelembapan: " + String(h) + "%";

      bot.sendMessage(chat_id, msg, "");
    }
  }
}

// ================= LOOP =================
void loop() {

  if (millis() - lastTime > botDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages) {
      Serial.println("Ada pesan masuk");
      handleNewMessages(numNewMessages);
    }

    lastTime = millis();
  }

  // ===== SENSOR GAS =====
  int gasValue = analogRead(MQ2_PIN);
  Serial.println("Gas: " + String(gasValue));

  if (gasValue > GAS_THRESHOLD && !gasAlertSent) {
    bot.sendMessage(GROUP_ID, "🚨 WARNING!!!\nGas terdeteksi!!!", "");
    gasAlertSent = true;
  }

  if (gasValue < GAS_THRESHOLD) {
    gasAlertSent = false;
  }

  delay(500);
}