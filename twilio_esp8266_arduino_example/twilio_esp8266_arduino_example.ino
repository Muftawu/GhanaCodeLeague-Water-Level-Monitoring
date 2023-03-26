/*
 * Twilio SMS and MMS on ESP8266 Example.
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>

#include "twilio.hpp"

// Your network SSID and password
const char* ssid = "Mohammed";
const char* password = "545723325";

// Find the api.twilio.com SHA1 fingerprint, this one was valid as 
// of July 2020. This will change, please see 
// https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-esp8266-cpp
// to see how to update the fingerprint.
const char fingerprint[] = "93 B5 96 8C BC 63 8D A1 D5 AD 1E DA 30 A1 DB 04 73 DC BC 24";

// Twilio account specific details, from https://twilio.com/console
// Please see the article: 
// https://www.twilio.com/docs/guides/receive-and-reply-sms-and-mms-messages-esp8266-c-and-ngrok

// If this device is deployed in the field you should only deploy a revocable
// key. This code is only suitable for prototyping or if you retain physical
// control of the installation.
const char* account_sid = "ACdd2d23ea91c13dfcc1ed49eea1ba0cfe";
const char* auth_token = "284dca0c5d6a283975b099a2f39c39d4";

// Details for the SMS we'll send with Twilio.  Should be a number you own 
// (check the console, link above).
String to_number    = "+233545723325";
String from_number = "+14344236413";
String message_body    = "Hello. Tank two full.";

// The 'authorized number' to text the ESP8266 for our example
String master_number    = "+14344236413";

// Optional - a url to an image.  See 'MediaUrl' here: 
// https://www.twilio.com/docs/api/rest/sending-messages
String media_url = "";

// Global twilio objects
Twilio *twilio;
ESP8266WebServer twilio_server(8000);

/*
 * Callback function when we hit the /message route with a webhook.
 * Use the global 'twilio_server' object to respond.
 */
 void handle_message() {
        Serial.println("Incoming connection!  Printing body:");
        bool authorized = false;
        char command = '\0';

        // Parse Twilio's request to the ESP
        for (int i = 0; i < twilio_server.args(); ++i) {
                Serial.print(twilio_server.argName(i));
                Serial.print(": ");
                Serial.println(twilio_server.arg(i));

                if (twilio_server.argName(i) == "From" and 
                    twilio_server.arg(i) == master_number) {
                    authorized = true;
                } else if (twilio_server.argName(i) == "Body") {
                        if (twilio_server.arg(i) == "?" or
                            twilio_server.arg(i) == "0" or
                            twilio_server.arg(i) == "1") {
                                command = twilio_server.arg(i)[0];
                        }
                }
        } // end for loop parsing Twilio's request

        // Logic to handle the incoming SMS
        // (Some board are active low so the light will have inverse logic)
        String response = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        if (command != '\0') {
                if (authorized) {
                        switch (command) {
                        case '0':
                                digitalWrite(LED_BUILTIN, LOW);
                                response += "<Response><Message>"
                                "Turning light off!"
                                "</Message></Response>";
                                break;
                        case '1':
                                digitalWrite(LED_BUILTIN, HIGH);
                                response += "<Response><Message>"
                                "Turning light on!"
                                "</Message></Response>";
                                break;
                        case '?':
                        default:
                                response += "<Response><Message>"
                                "0 - Light off, 1 - Light On, "
                                "? - Help\n"
                                "The light is currently: ";
                                response += digitalRead(LED_BUILTIN);
                                response += "</Message></Response>";
                                break;               
                        }
                } else {
                        response += "<Response><Message>"
                        "Unauthorized!"
                        "</Message></Response>";
                }

        } else {
                response += "<Response><Message>"
                "Look: a SMS response from an ESP8266!"
                "</Message></Response>";
        }

        twilio_server.send(200, "application/xml", response);
}

/*
 * Setup function for ESP8266 Twilio Example.
 * 
 * Here we connect to a friendly wireless network, set the time, instantiate 
 * our twilio object, optionally set up software serial, then send a SMS 
 * or MMS message.
 */
char incomingByte;
 
void setup() {
        WiFi.begin(ssid, password);
        twilio = new Twilio(account_sid, auth_token, fingerprint);

        Serial.begin(115200);

            while (WiFi.status() != WL_CONNECTED) {
                    delay(1000);
                    Serial.print(".");
            }
            Serial.println("");
            Serial.println("Connected to WiFi, IP address: ");

            Serial.println(WiFi.localIP());
            while (WiFi.status() != WL_CONNECTED) delay(1000);

            // Response will be filled with connection info and Twilio API responses
            // from this initial SMS send.
            
}


/* 
 *  In our main loop, we listen for connections from Twilio in handleClient().
 */
void loop() {      
      if (Serial.available() > 0) {
              incomingByte = Serial.read();

      if (incomingByte == '$') {
            String response;
            bool success = twilio->send_message(
                    to_number,
                    from_number,
                    message_body,
                    response,
                    media_url
            );

            // Set up a route to /message which will be the webhook url
            twilio_server.on("/message", handle_message);
            twilio_server.begin();

            // Use LED_BUILTIN to find the LED pin and set the GPIO to output
            pinMode(LED_BUILTIN, OUTPUT);

            Serial.println(response);
          
              twilio_server.handleClient();
              Serial.end();
              delay(1200);
              Serial.begin(115200);
          }   

        else {
          Serial.println("Denied");
        }       
      }
}
