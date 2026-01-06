#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include "ESPAsyncWebServer.h"
#include "credentials.h"
#include "EPD_Graphics.h"
#include <Fonts/FreeSansBold12pt7b.h>

EPD_Graphics display;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", -10800, 60000);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

struct TimeInfo {
  int dia;
  int mes;
  int ano;
  int diaSemana;
};

TimeInfo dataAtual;

const char *mesesNomes[12] = {
  "JANEIRO", "FEVEREIRO", "MARÇO", "ABRIL", "MAIO", "JUNHO",
  "JULHO", "AGOSTO", "SETEMBRO", "OUTUBRO", "NOVEMBRO", "DEZEMBRO"
};

int diasMeses[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define BISSEXTO(Y) ((Y > 0) && !((Y) % 4) && (((Y) % 100) || !((Y) % 400)))

int diaDaSemana(uint8_t dia, uint8_t mes, uint16_t ano) {
  static const uint16_t mesesAcum[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };

  uint32_t dias = (uint32_t)ano * 365;

  for (uint16_t i = 4; i < ano; i += 4) {
    if (BISSEXTO(i)) dias++;
  }

  dias += mesesAcum[mes - 1] + dia;

  if (mes > 2 && BISSEXTO(ano)) dias++;

  return (dias + 6) % 7;
}

void atualizarDataAtual() {
  timeClient.update();

  dataAtual.dia = timeClient.Dia();
  dataAtual.mes = timeClient.Mes();
  dataAtual.ano = timeClient.Ano();
  dataAtual.diaSemana = timeClient.Dia() % 7;

  diasMeses[1] = BISSEXTO(dataAtual.ano) ? 29 : 28;
}

void desenharCalendario(const String &agendaJson) {
  display.setRotation(3);
  display.clearScreen(EPD_WHITE);

  display.setTextSize(1);
  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(EPD_BLACK);
  display.setCursor(36, 45);
  display.print(mesesNomes[dataAtual.mes - 1]);
  display.setCursor(382, 45);
  display.print(dataAtual.ano);

  display.setTextColor(EPD_RED);
  display.setCursor(43, 85);
  display.print("D");
  display.setTextColor(EPD_BLACK);
  display.setCursor(105, 85);
  display.print("S");
  display.setCursor(168, 85);
  display.print("T");
  display.setCursor(229, 85);
  display.print("Q");
  display.setCursor(288, 85);
  display.print("Q");
  display.setCursor(354, 85);
  display.print("S");
  display.setCursor(416, 85);
  display.print("S");

  int y = 125;
  for (int j = 0; j < diasMeses[dataAtual.mes - 1]; j++) {
    int dia = j + 1;
    int diaSem = diaDaSemana(dia, dataAtual.mes, dataAtual.ano);
    int x = (diaSem * 62) + 38;
    if (dia == dataAtual.dia) {
      display.fillRect((diaSem * 62) + 32, y - 26, 44, 36, EPD_RED);
      display.setTextColor(EPD_WHITE);
      display.setCursor(x, y);
    } else {
      bool ehFeriado =
        diaSem == 0 || (dia == 1 && dataAtual.mes == 1) || (dia == 21 && dataAtual.mes == 4) || (dia == 1 && dataAtual.mes == 5) || (dia == 7 && dataAtual.mes == 9) || (dia == 12 && dataAtual.mes == 10) || (dia == 2 && dataAtual.mes == 11) || (dia == 15 && dataAtual.mes == 11) || (dia == 20 && dataAtual.mes == 11) || (dia == 25 && dataAtual.mes == 12);

      display.setTextColor(ehFeriado ? EPD_RED : EPD_BLACK);
      display.setCursor(x, y);
    }

    if (dia < 10) display.print('0');
    display.print(dia);

    if (diaSem == 6) {
      y += 45;
    }
  }

  display.drawLine(40, 430, 440, 430, EPD_BLACK);
  ;
  display.setTextColor(EPD_WHITE);

  if (agendaJson.length() > 2) {
    StaticJsonDocument<1024> doc;
    if (deserializeJson(doc, agendaJson) == DeserializationError::Ok) {
      JsonArray agenda = doc.as<JsonArray>();
      int yAgenda = 460;
      int contador = 0;

      for (JsonVariant item : agenda) {
        if (contador >= 7) break;
        String horario = item.as<String>();

        display.fillRect(20, yAgenda - 14, 432, 32, EPD_BLUE);
        display.fillRect(20, yAgenda - 14, 8, 32, EPD_RED);
        display.setCursor(36, yAgenda + 9);
        display.print(horario);

        yAgenda += 45;
        contador++;
      }
    }
  }
  display.setTextColor(EPD_RED);
  display.setCursor(32, 390);
  display.setFont();
  feriado(dataAtual.mes);
  display.display();
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    client->text("READY");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len) {
      String mensagem;
      mensagem.reserve(len);
      for (size_t i = 0; i < len; i++) {
        mensagem += (char)data[i];
      }

      if (mensagem.startsWith("SCHEDULE:")) {
        String agendaJson = mensagem.substring(9);
        atualizarDataAtual();
        desenharCalendario(agendaJson);

        display.waitBusy();

        client->text("OK_UPDATED");
      } else {
        client->text("UNKNOWN_COMMAND");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin()) {
    while (true) {
      delay(100);
    }
  }

  if (!display.begin()) {
    while (true) {
      delay(100);
    }
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_credentials.ssid, wifi_credentials.password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 30) {
    delay(500);
    tentativas++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    while (true) {
      delay(100);
    }
  }

  timeClient.begin();
  atualizarDataAtual();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html.gz", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html.gz", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.begin();
}

void loop() {
  ws.cleanupClients();
  delay(10);
}