#include <base64.h>

#include "ESP8266WiFi.h"
#include <WiFiClientSecure.h>
#include <string>

// Your network SSID and password
const char* ssid = "Mohammed";
const char* password = "mmyiwere";

const char* account_sid = "ACab89ba4e0d89f7ad901b741207c9af4f";
const char* auth_token = "a6c06c31c714369fda3f32fff920bc9a";
String from_number      = "+19403948496";
String to_number        = "+233545723325";
String message_body     = "Tank Full. Please attend to tank 2";

const char fingerprint[] = "72 1C 17 31 85 E2 7E 0D F9 D4 C2 5B A0 0E BD B7 E2 06 26 ED";

// FUNCTION TO ENCODE STRING

String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
    yield();
  }
  return encodedString;
}


// FUNCTION NUMBER TWO
String get_auth_header(const String& user, const String& password) {
  size_t toencodeLen = user.length() + password.length() + 2;
  char toencode[toencodeLen];
  memset(toencode, 0, toencodeLen);
  snprintf(toencode, toencodeLen, "%s:%s", user.c_str(), password.c_str());
  String encoded = base64::encode((uint8_t*)toencode, toencodeLen - 1);
  String encoded_string = String(encoded);
  std::string::size_type i = 0;
  // Strip newlines (after every 72 characters in spec)
  while (i < encoded_string.length()) {
    i = encoded_string.indexOf('\n', i);
    if (i == -1) {
      break;
    }
    encoded_string.remove(i, 1);
  }
  return "Authorization: Basic " + encoded_string;
}

void setup() {
  Serial.begin(112500); // 115200 or 9600
  }

void loop() {

  // *****************************************

    if (Serial.available() > 1) {
    char c = Serial.read();
    Serial.write(c);
    if ( c == '$') {
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
  Serial.println("+ Connect to WiFi. ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("+ Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());

  // ----------------------------------------------------
  // Use WiFiClientSecure to create a TLS 1.2 connection.
  //  Note, using a cert fingerprint is required.
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
  Serial.printf("+ Using fingerprint '%s'\n", fingerprint);
  const char* host = "api.twilio.com";
  const int   httpsPort = 443;
  Serial.print("+ Connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("- Connection failed.");
    return; // Skips to loop();
  }
  Serial.println("+ Connected.");
  Serial.println("+ Post an HTTP send SMS request.");
  String post_data = "To=" + urlencode(to_number)
                     + "&From=" + urlencode(from_number)
                     + "&Body=" + urlencode(message_body);
  String auth_header = get_auth_header(account_sid, auth_token);
  String http_request = "POST /2010-04-01/Accounts/" + String(account_sid) + "/Messages HTTP/1.1\r\n"
                        + auth_header + "\r\n"
                        + "Host: " + host + "\r\n"
                        + "Cache-control: no-cache\r\n"
                        + "User-Agent: ESP8266 Twilio Example\r\n"
                        + "Content-Type: application/x-www-form-urlencoded\r\n"
                        + "Content-Length: " + post_data.length() + "\r\n"
                        + "Connection: close\r\n"
                        + "\r\n"
                        + post_data
                        + "\r\n";
  client.println(http_request);
//   Read the response.
//  String response = "";
//  while (client.connected()) {
//    String line = client.readStringUntil('\n');
//    response += (line);
//    response += ("\r\n");
//  }
  Serial.println("+ Connection is closed.");
//  Serial.println("+ Response:");
//  Serial.println(response);

  // ---------------------------------------------------------------------------
  Serial.println(F("+ Starting the loop."));
    }
  }

  //******************************************
  delay(1000);
}
