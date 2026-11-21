#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
const char* ssid = "ВАШ_WIFI_SSID";
const char* password = "ВАШ_WIFI_ПАРОЛЬ";
const int PIN_RED = 25;
const int PIN_GREEN = 26;
const int PIN_BLUE = 27;
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;
const int PWM_CHANNEL_R = 0;
const int PWM_CHANNEL_G = 1;
const int PWM_CHANNEL_B = 2;
int redValue = 0;
int greenValue = 0;
int blueValue = 0;
int brightness = 255;
WebServer server(80);
const char* htmlPage = R"HTML(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>RGB LED Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            margin: 0;
            padding: 20px;
            min-height: 100vh;
        }
        .container {
            max-width: 600px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 30px;
        }
        .color-picker {
            margin-bottom: 20px;
        }
        label {
            display: block;
            margin-bottom: 10px;
            font-weight: bold;
            color: #555;
        }
        input[type="color"] {
            width: 100%;
            height: 60px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
        }
        input[type="range"] {
            width: 100%;
            margin: 10px 0;
        }
        .slider-container {
            margin: 20px 0;
        }
        .slider-value {
            display: inline-block;
            width: 50px;
            text-align: right;
            font-weight: bold;
        }
        button {
            width: 100%;
            padding: 15px;
            margin: 10px 0;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s;
        }
        .btn-primary {
            background: #667eea;
            color: white;
        }
        .btn-primary:hover {
            background: #5568d3;
        }
        .btn-danger {
            background: #e74c3c;
            color: white;
        }
        .btn-danger:hover {
            background: #c0392b;
        }
        .preset-btn {
            background: #2ecc71;
            color: white;
            margin: 5px;
            width: calc(50% - 10px);
            display: inline-block;
        }
        .preset-btn:hover {
            background: #27ae60;
        }
        .status {
            text-align: center;
            padding: 10px;
            margin: 20px 0;
            border-radius: 5px;
            background: #ecf0f1;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🎨 RGB LED Control</h1>
        <div class="color-picker">
            <label>Выберите цвет:</label>
            <input type="color" id="colorPicker" value="#000000">
        </div>
        <div class="slider-container">
            <label>Яркость: <span class="slider-value" id="brightnessValue">255</span></label>
            <input type="range" id="brightnessSlider" min="0" max="255" value="255">
        </div>
        <div>
            <label>Готовые эффекты:</label>
            <button class="preset-btn" onclick="setPreset('rainbow')">🌈 Радуга</button>
            <button class="preset-btn" onclick="setPreset('fade')">✨ Плавное затухание</button>
            <button class="preset-btn" onclick="setPreset('strobe')">⚡ Стробоскоп</button>
            <button class="preset-btn" onclick="setPreset('warm')">🔥 Теплый свет</button>
        </div>
        <button class="btn-primary" onclick="updateColor()">Применить цвет</button>
        <button class="btn-danger" onclick="turnOff()">Выключить</button>
        <div class="status" id="status">Готово к работе</div>
    </div>
    <script>
        const colorPicker = document.getElementById('colorPicker');
        const brightnessSlider = document.getElementById('brightnessSlider');
        const brightnessValue = document.getElementById('brightnessValue');
        const status = document.getElementById('status');
        brightnessSlider.addEventListener('input', function() {
            brightnessValue.textContent = this.value;
        });
        function hexToRgb(hex) {
            const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
            return result ? {
                r: parseInt(result[1], 16),
                g: parseInt(result[2], 16),
                b: parseInt(result[3], 16)
            } : null;
        }
        function updateColor() {
            const color = hexToRgb(colorPicker.value);
            const brightness = brightnessSlider.value;
            fetch('/setColor', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({
                    r: color.r,
                    g: color.g,
                    b: color.b,
                    brightness: parseInt(brightness)
                })
            })
            .then(response => response.json())
            .then(data => {
                status.textContent = 'Цвет обновлен!';
                status.style.background = '#2ecc71';
                status.style.color = 'white';
            })
            .catch(error => {
                status.textContent = 'Ошибка: ' + error;
                status.style.background = '#e74c3c';
                status.style.color = 'white';
            });
        }
        function turnOff() {
            fetch('/setColor', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({r: 0, g: 0, b: 0, brightness: 0})
            })
            .then(response => response.json())
            .then(data => {
                status.textContent = 'Лента выключена';
                status.style.background = '#34495e';
                status.style.color = 'white';
            });
        }
        function setPreset(preset) {
            fetch('/preset', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({effect: preset})
            })
            .then(response => response.json())
            .then(data => {
                status.textContent = 'Эффект активирован: ' + preset;
                status.style.background = '#3498db';
                status.style.color = 'white';
            });
        }
    </script>
</body>
</html>
)HTML";
void setColor(int r, int g, int b, int bright) {
    r = (r * bright) / 255;
    g = (g * bright) / 255;
    b = (b * bright) / 255;
    ledcWrite(PWM_CHANNEL_R, r);
    ledcWrite(PWM_CHANNEL_G, g);
    ledcWrite(PWM_CHANNEL_B, b);
    redValue = r;
    greenValue = g;
    blueValue = b;
    brightness = bright;
}
void handleRoot() {
    server.send(200, "text/html", htmlPage);
}
void handleSetColor() {
    if (server.hasArg("plain")) {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, server.arg("plain"));
        int r = doc["r"] | 0;
        int g = doc["g"] | 0;
        int b = doc["b"] | 0;
        int bright = doc["brightness"] | 255;
        setColor(r, g, b, bright);
        JsonObject response = doc.to<JsonObject>();
        response["status"] = "ok";
        response["r"] = r;
        response["g"] = g;
        response["b"] = b;
        response["brightness"] = bright;
        String responseStr;
        serializeJson(response, responseStr);
        server.send(200, "application/json", responseStr);
    } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid request\"}");
    }
}
void handlePreset() {
    if (server.hasArg("plain")) {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, server.arg("plain"));
        String effect = doc["effect"] | "";
        if (effect == "rainbow") {
            server.send(200, "application/json", "{\"status\":\"ok\",\"effect\":\"rainbow\"}");
        } else if (effect == "fade") {
            for (int i = 255; i >= 0; i--) {
                setColor(redValue, greenValue, blueValue, i);
                delay(10);
            }
            server.send(200, "application/json", "{\"status\":\"ok\",\"effect\":\"fade\"}");
        } else if (effect == "strobe") {
            for (int i = 0; i < 3; i++) {
                setColor(255, 255, 255, 255);
                delay(100);
                setColor(0, 0, 0, 0);
                delay(100);
            }
            server.send(200, "application/json", "{\"status\":\"ok\",\"effect\":\"strobe\"}");
        } else if (effect == "warm") {
            setColor(255, 150, 50, 255);
            server.send(200, "application/json", "{\"status\":\"ok\",\"effect\":\"warm\"}");
        } else {
            server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Unknown effect\"}");
        }
    } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid request\"}");
    }
}
void handleGetStatus() {
    StaticJsonDocument<200> doc;
    doc["r"] = redValue;
    doc["g"] = greenValue;
    doc["b"] = blueValue;
    doc["brightness"] = brightness;
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}
void setup() {
    Serial.begin(115200);
    delay(1000);
    ledcSetup(PWM_CHANNEL_R, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_G, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(PIN_RED, PWM_CHANNEL_R);
    ledcAttachPin(PIN_GREEN, PWM_CHANNEL_G);
    ledcAttachPin(PIN_BLUE, PWM_CHANNEL_B);
    setColor(0, 0, 0, 0);
    Serial.println();
    Serial.print("Подключение к Wi-Fi: ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("Wi-Fi подключен!");
        Serial.print("IP адрес: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("Не удалось подключиться к Wi-Fi!");
    }
    server.on("/", handleRoot);
    server.on("/setColor", HTTP_POST, handleSetColor);
    server.on("/preset", HTTP_POST, handlePreset);
    server.on("/status", HTTP_GET, handleGetStatus);
    server.begin();
    Serial.println("Веб-сервер запущен!");
    Serial.println("Откройте в браузере: http://" + WiFi.localIP().toString());
}
void loop() {
    server.handleClient();
    delay(10);
}