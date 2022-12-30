#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "*********" // WIFI SSID
#define STAPSK "*********"  // WIFI password
#endif

#define PIN0 0
#define PIN1 2

const int g_base_r = 80;
const int g_switch_r = 40;
const int servo_runtime = 200;

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80); // server端口号
Servo g_servo;

// html
String html1 = R"rawliteral(
<!DOCTYPE html>
<html lang="cn">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Welcome ESP8266 for Light</title>
    <script>
        function local(state) {
            var xmlhttp = new XMLHttpRequest();
            xmlhttp.open("GET", "/pin?state=" + state, true);
            xmlhttp.send();
        }
    </script>
    <style>
        body{
            background-color:rgb(10, 10, 10);
        }
        input{
            margin: 20px;
            font-size: 24px;
            padding: 15px;
            rotate: 0px;
            color: white;
        }
    </style>
</head>

<body>
    <div>
        <a href="#" onclick="local('on')">
            <input type="button" value="CLOSE" style="background-color: rgb(182, 0, 0);border-color: rgb(173, 0, 0);">
        </a>
        <a href="#" onclick="local('off')">
            <input type="button" value="OPEN" style="float: right;background-color: rgb(0, 0, 130);border-color: rgb(0, 0, 133);">
        </a>
    </div>
</body>

</html>
)rawliteral";

//-----------控制light fun------//
void controller()
{
  String json = R"rawliteral({"mag":"OK"})rawliteral";
  String state = server.arg("state");
  if (state == "on")
  {
    //    myservo.write(servooff);
    Serial.println("GPIO pin1 on");
    digitalWrite(PIN0, HIGH);
    g_servo.write(g_base_r - g_switch_r);
    delay(servo_runtime);
    g_servo.write(g_base_r);
    digitalWrite(PIN0, LOW);
  }
  else if (state == "off")
  {
    //    myservo.write(servoon);
    Serial.println("GPIO pin off");
    digitalWrite(PIN0, HIGH);
    g_servo.write(g_base_r + g_switch_r);
    delay(servo_runtime);
    g_servo.write(g_base_r);
    digitalWrite(PIN0, LOW);
  }
  server.send(200, "text/plain", json);
}

//--------返回html文件-----------//

void responseHtml(String html)
{
  server.send(200, "text/html", html);
}
/**
 * 必须写两个函数 
 * 如果删除 responseHtml 请求主页时会堆栈溢出
 */
void handleRoot()
{
  responseHtml(html1);
}

//----------响应信息--------------//
void handleNotFound()
{
  //  myservo.write(servooff);
  //  digitalWrite(PIN_LIGHT,LOW);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //  digitalWrite(PIN_LIGHT,HIGH);
}

void setup(void)
{
  //------------初始化------------//

  pinMode(PIN0, OUTPUT);
  digitalWrite(PIN0, LOW);

  g_servo.attach(PIN1);
  g_servo.write(g_base_r);

  Serial.begin(115200);

  //-----------WIFI模式-------------
  WiFi.mode(WIFI_STA);
  //  IPAddress ip(192, 168, 128, 1);
  //  IPAddress ip2(192, 168, 128, 1);
  //  IPAddress su(255, 255, 255, 0);
  //  WiFi.softAPConfig(ip, ip2, su);
  WiFi.begin(ssid, password);
  //  WiFi.softAP(ssid,password);

  Serial.println(WiFi.softAPIP());
  Serial.println("");

  //------------检查连接----------------
  while (WiFi.status() != WL_CONNECTED)
  {
    //    break;
    delay(200);
    Serial.print(".");
  }
  // WIFI connection
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  ///******************************************///

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  //-------------路由------------------
  server.on("/", handleRoot);       // 主页
  server.on("/pin", HTTP_GET, controller); // 控制

  //-------------响应-----------------
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
  MDNS.update();
}