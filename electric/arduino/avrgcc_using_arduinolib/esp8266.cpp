#include "Arduino.h"

// int main(void)
// {
//     init();
//     pinMode(13, OUTPUT);
//     while (1)
//     {
//         digitalWrite(13, HIGH);
//         delay(1000);
//         digitalWrite(13, LOW);
//         delay(1000);
//     }
//     return 0;
// }

//[ESP8266 Android 与 Arduino 通信](https://blog.csdn.net/gnf_cc/article/details/53667312)
// #include <avr/io.h>
#include "SoftwareSerial.h"


///////////////////////////////////////

#define DEBUG true

class MySerial : public SoftwareSerial
{
  public:
    // construct
    MySerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false)
        : SoftwareSerial(receivePin, transmitPin, inverse_logic){};

  public:
    /**
    * Name: sendData
    * Description: Function used to send data to ESP8266.
    * Params: command - the data/command to send; timeout - the time to wait for a 
    *         response; debug - print to Serial window?(true = yes, false = no)
    * Returns: The response from the esp8266 (if there is a reponse)
    */
    String sendData(String command, const int timeout, boolean debug)
    {
        String response = "";

        int dataSize = command.length();
        char data[dataSize];
        command.toCharArray(data, dataSize);

        write(data, dataSize); // send the read character to the esp8266
        if (debug)
        {
            Serial.println("\r\n====== HTTP Response From Arduino ======");
            Serial.write(data, dataSize);
            Serial.println("\r\n========================================");
        }

        long int time = millis();

        while ((time + timeout) > millis())
        {
            while (available())
            {

                // The esp has data so display its output to the serial window
                char c = read(); // read the next character.
                response += c;
            }
        }

        if (debug)
        {
            Serial.print(response);
        }

        return response;
    }

    /*
    * Name: sendCommand
    * Description: Function used to send data to ESP8266.
    * Params: command - the data/command to send; timeout - the time to wait for 
    *         a response; debug - print to Serial window?(true = yes, false = no)
    * Returns: The response from the esp8266 (if there is a reponse)
    */
    String sendCommand(String command, const int timeout, boolean debug)
    {
        String response = "";
        this->print(command); // send the read character to the esp8266
        long int time = millis();

        while ((time + timeout) > millis())
        {
            while (this->available())
            {

                // The esp has data so display its output to the serial window
                char c = this->read(); // read the next character.
                response += c;
            }
        }

        if (debug)
        {
            Serial.print(response);
        }
        return response;
    }
    /*
    * Name: sendHTTPResponse
    * Description: Function that sends HTTP 200, HTML UTF-8 response
    */
    void sendHTTPResponse(int connectionId, String content)
    {
        // build HTTP response
        String httpResponse;
        String httpHeader;
        // HTTP Header
        httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n";
        httpHeader += "Content-Length: ";
        httpHeader += content.length();
        httpHeader += "\r\n";
        httpHeader += "Connection: close\r\n\r\n";
        httpResponse = httpHeader + content + " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
        sendCIPData(connectionId, httpResponse);
    }

    /*
    * Name: sendCIPDATA
    * Description: sends a CIPSEND=<connectionId>,<data> command
    *
    */
    void sendCIPData(int connectionId, String data)
    {
        String cipSend = "AT+CIPSEND=";
        cipSend += connectionId;
        cipSend += ",";
        cipSend += data.length();
        cipSend += "\r\n";
        sendCommand(cipSend, 1000, DEBUG);
        sendData(data, 1000, DEBUG);
    }

}

// make RX Arduino line is pin 3, make TX Arduino line is pin 4.
// This means that you need to connect the TX line from the esp to the Arduino's pin 2
// and the RX line from the esp to the Arduino's pin 3
esp8266 = MySerial(3 /* receivePin */, 4 /* transmitPin */);

void mysetup()
{
    Serial.begin(57600);
    esp8266.begin(57600); // your esp's baud rate might be different

    pinMode(11, OUTPUT);
    digitalWrite(11, LOW);

    pinMode(12, OUTPUT);
    digitalWrite(12, LOW);

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    pinMode(10, OUTPUT);
    digitalWrite(10, LOW);

    esp8266.sendCommand("AT+RST\r\n", 2000, DEBUG);      // reset module
    esp8266.sendCommand("AT+CWMODE=1\r\n", 1000, DEBUG); // configure as access point
    esp8266.sendCommand("AT+CWJAP=\"mySSID\",\"myPassword\"\r\n", 3000, DEBUG);
    delay(10000);
    esp8266.sendCommand("AT+CIFSR\r\n", 1000, DEBUG);          // get ip address
    esp8266.sendCommand("AT+CIPMUX=1\r\n", 1000, DEBUG);       // configure for multiple connections
    esp8266.sendCommand("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // turn on server on port 80

    Serial.println("Server Ready");
}

//回显测试
void testEcholoop()
{
    if (esp8266.available())
    {
        Serial.write(esp8266.read());
    }
    if (Serial.available())
    {
        esp8266.write(Serial.read());
    }
}

/* void loop()
{
    if (esp8266.available()) // check if the esp is sending a message
    {

        if (esp8266.find("+IPD,"))
        {
            delay(1000); // wait for the serial buffer to fill up (read all the serial data)
            // get the connection id so that we can then disconnect
            int connectionId = esp8266.read() - 48; // subtract 48 because the read() function returns
                                                    // the ASCII decimal value and 0 (the first decimal number) starts at 48

            esp8266.find("pin="); // advance cursor to "pin="

            int pinNumber = (esp8266.read() - 48); // get first number i.e. if the pin 13 then the 1st number is 1
            int secondNumber = (esp8266.read() - 48);
            if (secondNumber >= 0 && secondNumber <= 9)
            {
                pinNumber *= 10;
                pinNumber += secondNumber; // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
            }

            digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin

            // build string that is send back to device that is requesting pin toggle
            String content;
            content = "Pin ";
            content += pinNumber;
            content += " is ";

            if (digitalRead(pinNumber))
            {
                content += "ON";
            }
            else
            {
                content += "OFF";
            }

            esp8266.sendHTTPResponse(connectionId, content);

            // make close command
            String closeCommand = "AT+CIPCLOSE=";
            closeCommand += connectionId; // append connection id
            closeCommand += "\r\n";

            esp8266.sendCommand(closeCommand, 1000, DEBUG); // close connection
        }
    }
}
 */

int main(void)
{
    init();

    mysetup();

    while (1)
    {
        testEcholoop();
    }
    return 0;
}
