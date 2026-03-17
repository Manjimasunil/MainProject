#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <DNSServer.h>
#include <EEPROM.h>

/* ================= EEPROM ================= */
#define EEPROM_SIZE       160
#define WIFI_FLAG_ADDR    0
#define SSID_ADDR         10
#define PASS_ADDR         42
#define SERVER_IP_ADDR    80

char savedSSID[32];
char savedPASS[32];
char savedServerIP[16];

/* ================= SERVER ================= */
String serverIP;

/* ================= OBJECTS ================= */
ESP8266WebServer server(80);
DNSServer dnsServer;
SoftwareSerial fingerSerial(D2, D1);
Adafruit_Fingerprint finger(&fingerSerial);

/* ================= STYLE CSS ================= */
String styleCSS() {
  return R"rawliteral(
body {
  margin: 0;
  font-family: 'Segoe UI', Arial, sans-serif;
  background: linear-gradient(135deg, #1e3c72, #2a5298);
  color: #fff;
}
.container { max-width: 420px; margin: 40px auto; padding: 20px; }
.card {
  background: #ffffff;
  color: #333;
  border-radius: 14px;
  padding: 22px;
  box-shadow: 0 10px 25px rgba(0,0,0,0.25);
}
h2 { text-align: center; margin-bottom: 20px; }
.btn {
  display: block;
  width: 100%;
  padding: 14px;
  margin: 12px 0;
  font-size: 17px;
  border-radius: 10px;
  text-decoration: none;
  color: white;
  text-align: center;
  background: linear-gradient(135deg, #00c6ff, #0072ff);
}
.btn.att { background: linear-gradient(135deg, #00b09b, #96c93d); }
.btn.del { background: linear-gradient(135deg, #ff416c, #ff4b2b); }
input, button {
  width: 100%;
  padding: 12px;
  margin-top: 10px;
  border-radius: 8px;
  border: 1px solid #ccc;
  font-size: 16px;
}
button {
  background: #ff416c;
  color: #fff;
  border: none;
  cursor: pointer;
}
.status {
  margin-top: 20px;
  padding: 18px;
  border-radius: 12px;
  text-align: center;
  font-size: 18px;
  font-weight: 500;
}
.status.info { background:#e3f2fd; color:#0d47a1; }
.status.success { background:#e8f5e9; color:#1b5e20; }
.status.error { background:#ffebee; color:#b71c1c; }
.status.warn { background:#fff3e0; color:#e65100; }
.back {
  display: inline-block;
  margin-top: 15px;
  padding: 10px 18px;
  background: #0072ff;
  color: #fff;
  border-radius: 8px;
  text-decoration: none;
}
)rawliteral";
}

/* ================= EEPROM LOAD/SAVE ================= */
bool loadConfig() {
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(WIFI_FLAG_ADDR) != 1) return false;

  for (int i = 0; i < 32; i++) {
    savedSSID[i] = EEPROM.read(SSID_ADDR + i);
    savedPASS[i] = EEPROM.read(PASS_ADDR + i);
  }
  for (int i = 0; i < 16; i++) {
    savedServerIP[i] = EEPROM.read(SERVER_IP_ADDR + i);
  }
  return true;
}

void saveConfig(String ssid, String pass, String sip) {
  EEPROM.write(WIFI_FLAG_ADDR, 1);

  for (int i = 0; i < 32; i++) {
    EEPROM.write(SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
    EEPROM.write(PASS_ADDR + i, i < pass.length() ? pass[i] : 0);
  }
  for (int i = 0; i < 16; i++) {
    EEPROM.write(SERVER_IP_ADDR + i, i < sip.length() ? sip[i] : 0);
  }
  EEPROM.commit();
}

/* ================= WIFI CONFIG PAGE ================= */
String wifiConfigPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Setup</title>
<style>
body{font-family:Arial;background:#1e3c72;color:#fff}
.card{max-width:360px;margin:60px auto;background:#fff;color:#333;padding:20px;border-radius:12px}
input{width:100%;padding:10px;margin:8px 0}
button{width:100%;padding:12px;background:#0072ff;color:#fff;border:0;border-radius:8px}
</style>
</head>
<body>
<div class="card">
<h2>Device Setup</h2>
<form action="/save">
<input name="ssid" placeholder="WiFi SSID" required>
<input name="pass" placeholder="WiFi Password" type="password">
<input name="server" placeholder="Server IP (eg: 192.168.1.3)" required>
<button>Save & Restart</button>
</form>
</div>
</body>
</html>
)rawliteral";
}

/* ================= STATUS PAGE ================= */
void sendStatusPage(String title, String msg, String type) {
  server.send(200, "text/html",
    "<!DOCTYPE html><html><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<link rel='stylesheet' href='/style.css'></head><body>"
    "<div class='container'><div class='card'>"
    "<h2>" + title + "</h2>"
    "<div class='status " + type + "'>" + msg + "</div>"
    "<a class='back' href='/'>Back</a>"
    "</div></div></body></html>"
  );
}

/* ================= DASHBOARD ================= */
String dashboardPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="/style.css">
</head>
<body>
<div class="container">
<div class="card">
<h2>Fingerprint System</h2>
<a class="btn" href="/enroll">Enroll Student</a>
<a class="btn att" href="/attendance">Attendance</a>
<form action="/delete">
<input type="number" name="id" min="1" max="127" placeholder="Delete Finger ID" required>
<button>Delete Finger</button>
</form>
<a class="btn del" href="/delete_all"
onclick="return confirm('Delete ALL fingerprints?')">
Delete All Fingers
</a>
</div>
</div>
</body>
</html>
)rawliteral";
}

/* ================= HELPERS ================= */
uint8_t getNextID() {
  for (uint8_t i = 1; i < 127; i++) {
    if (finger.loadModel(i) != FINGERPRINT_OK) return i;
  }
  return 127;
}

bool scanFinger(uint8_t timeoutSec = 4) {
  unsigned long start = millis();
  while (millis() - start < timeoutSec * 1000UL) {
    if (finger.getImage() == FINGERPRINT_OK &&
        finger.image2Tz() == FINGERPRINT_OK) return true;
    delay(80);
  }
  return false;
}

/* ================= ENROLL ================= */
void handleEnroll() {
  unsigned long t0 = millis();
  while (finger.getImage() != FINGERPRINT_OK) {
    if (millis() - t0 > 5000) {
      sendStatusPage("Timeout", "No finger detected", "warn");
      return;
    }
    delay(80);
  }

  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    sendStatusPage("Error", "Failed to read finger", "error");
    return;
  }

  if (finger.fingerFastSearch() == FINGERPRINT_OK) {
    sendStatusPage("Already Registered",
                   "Finger ID: " + String(finger.fingerID), "error");
    return;
  }

  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER) delay(80);

  unsigned long t1 = millis();
  while (finger.getImage() != FINGERPRINT_OK) {
    if (millis() - t1 > 5000) {
      sendStatusPage("Timeout", "Second scan failed", "warn");
      return;
    }
    delay(80);
  }

  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    sendStatusPage("Error", "Second scan failed", "error");
    return;
  }

  uint8_t newID = getNextID();

  if (finger.createModel() != FINGERPRINT_OK ||
      finger.storeModel(newID) != FINGERPRINT_OK) {
    sendStatusPage("Error", "Enrollment failed", "error");
    return;
  }

  String url = "http://" + serverIP +
               "/fingerprint/pages/enroll_form.php?fid=" + String(newID);
  server.sendHeader("Location", url);
  server.send(302);
}

/* ================= ATTENDANCE ================= */
void handleAttendance() {
  if (!scanFinger()) {
    sendStatusPage("Timeout", "Scan Timeout", "warn");
    return;
  }

  if (finger.fingerFastSearch() != FINGERPRINT_OK) {
    sendStatusPage("Access Denied", "Finger Not Registered", "error");
    return;
  }

  String url = "http://" + serverIP +
               "/fingerprint/api/check_student.php?fid=" +
               String(finger.fingerID);
  server.sendHeader("Location", url);
  server.send(302);
}

/* ================= DELETE ================= */
void handleDeleteFinger() {
  if (!server.hasArg("id")) {
    sendStatusPage("Error", "ID Missing", "error");
    return;
  }
  uint8_t id = server.arg("id").toInt();
  if (finger.deleteModel(id) == FINGERPRINT_OK)
    sendStatusPage("Success", "Finger ID Deleted", "success");
  else
    sendStatusPage("Error", "Delete Failed", "error");
}

void handleDeleteAll() {
  bool any = false;
  for (uint8_t i = 1; i < 128; i++) {
    if (finger.deleteModel(i) == FINGERPRINT_OK) {
      any = true;
      delay(30);
    }
  }
  sendStatusPage(any ? "Success" : "Info",
                 any ? "All Fingerprints Deleted" : "No Fingerprints Found",
                 any ? "success" : "info");
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(9600);
  Serial.println("\nBooting...");

  fingerSerial.begin(57600);
  finger.begin(57600);

  WiFi.mode(WIFI_STA);

  if (loadConfig()) {
    Serial.println("Connecting to saved WiFi...");
    WiFi.begin(savedSSID, savedPASS);
  }

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi failed → Captive Portal");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP_Fingerprint_Setup");
    dnsServer.start(53, "*", WiFi.softAPIP());

    server.on("/", []() {
      server.send(200, "text/html", wifiConfigPage());
    });

    server.on("/save", []() {
      saveConfig(server.arg("ssid"), server.arg("pass"), server.arg("server"));
      server.send(200, "text/html", "<h2>Saved. Rebooting…</h2>");
      delay(1500);
      ESP.restart();
    });

    server.onNotFound([]() {
      server.sendHeader("Location", "/");
      server.send(302);
    });

    server.begin();
    return;
  }

  serverIP = String(savedServerIP);

  Serial.println("\n====== NETWORK INFO ======");
  Serial.print("SSID      : "); Serial.println(WiFi.SSID());
  Serial.print("ESP IP    : "); Serial.println(WiFi.localIP());
  Serial.print("Server IP : "); Serial.println(serverIP);
  Serial.println("==========================");

  server.on("/", [](){ server.send(200, "text/html", dashboardPage()); });
  server.on("/style.css", [](){ server.send(200, "text/css", styleCSS()); });
  server.on("/enroll", handleEnroll);
  server.on("/attendance", handleAttendance);
  server.on("/delete", handleDeleteFinger);
  server.on("/delete_all", handleDeleteAll);

  server.begin();
}

/* ================= LOOP ================= */
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
