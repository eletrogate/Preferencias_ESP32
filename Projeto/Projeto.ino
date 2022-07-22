/******************************************************************************
                         Salvando Preferências no ESP32

                        Criado em 03 de Novembro de 2021                        
                                por Michel Galvão
                                
              Modificado em 21 de Dezembro de 2021, para correção de bug
                                por Michel Galvão

  Eletrogate - Loja de Arduino \\ Robótica \\ Automação \\ Apostilas \\ Kits
                            https://www.eletrogate.com/
******************************************************************************/

// Inclusão da(s) biblioteca(s)
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
#include <nvs_flash.h>
#include "time.h"
#include "DHT.h"

#define DHTPIN 16 // pino utilizado pelo dht11
#define DHTTYPE DHT11   // DHT 11

// Inclusão das configurações do servidor NTP
const char* ntpServer = "time.google.com"; // endereço servidor
long  gmtOffset_sec; // define o deslocamento em segundos entre o fuso horário local e o GMT
int   daylightOffset_sec; // define o deslocamento em segundos para o horário de verão

long timeZone; // armazenará a zona horária do fuso horário local
int horarioDeVerao; // controlará se será utlzado horário de verão, ou não.
struct tm timeinfo; // estrutura armazenará dados de hora, minuto, segundo, etc. atuais obtidos do servidor NTP

// Criação de objeto(s) da(s) classe(s)
Preferences preferences;
IPAddress AP_local_ip(192, 168, 1, 1);
IPAddress AP_gateway(192, 168, 1, 1);
IPAddress AP_subnet(255, 255, 255, 0);
WebServer server(80); // Define o número da porta do servidor web para 80
DHT dht(DHTPIN, DHTTYPE);

const char* AP_ssid = "ESP32 Ponto de Acesso"; // ssid para o ponto de acesso
const char* AP_password = "senha1234567890"; // senha para o ponto de acesso

bool deveReiniciar = false; // controla se deve, ou não, reiniciar o ESP32
bool reiniciaESP = false; // reinicia o ESP32 se o valor for true

// Configurações de Rede modo estação
String ssid  = "NULL";
String pass  = "NULL";


bool eventoAcionado = false; // controla o acionamento do evento cadastrado

const int LED_BUILTIN = 2; // define o pino do LED builtin (embutido na placa)

// declaração do tipo 'tipoMensagem' para controle dos tipos de mensagem para codigo html
enum tipoMensagem {
  alerta, // Indica uma ação perigosa ou potencialmente negativa.
  aviso, // Indica um aviso que pode precisar de atenção.
  informacao, // Indica uma mudança ou ação informativa neutra.
  sucesso // Indica uma ação bem-sucedida ou positiva.
};

// Construtores das funções
void alteraCredenciais();
void handle_alteraCredenciaisAP();
void handle_NotFound();
void tentaConexao();
String messageBoxEmHtml(String titulo, String corpoMensagem, tipoMensagem mensagem);


/**
  setup(): Código de configuração aqui, para executar uma vez 
*/
void setup() {
  Serial.begin(115200); // Configura a taxa de transferência em bits por 
  //                        segundo (baud rate) para transmissão serial.
  delay(1000);
  Serial.println();
  
  pinMode(LED_BUILTIN, OUTPUT); // Configura o pino 'LED_BUILTIN' como saída

  dht.begin();


  /**
    bool begin(const char * name, bool readOnly=false);

    Abra Preferências com o nome de namespace informado.
    @param name:      indica o nome do namespace
    @param readOnly:  true indica abrir ou criar o namespace no modo somente leitura E
                      false indica abrir ou criar o namespace  no modo de leitura/gravação
  */
  preferences.begin("config_rede", true);

  ssid = preferences.getString("ssid", "NULL");
  pass = preferences.getString("pass", "NULL");
  preferences.end();

  digitalWrite(LED_BUILTIN, LOW);
  if (ssid == "NULL" || pass == "NULL") {

    Serial.println("SSID e/ou Password não encontrados");
    alteraCredenciais();
  }

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.print("Conectando à rede de SSID ");
  Serial.println(ssid);



  server.on("/", handle_indexSTA);
  server.onNotFound(handle_NotFound);

  server.begin();

  tentaConexao();
  IPAddress ip;
  ip = WiFi.localIP();
  Serial.print("No IP ");
  Serial.println(ip);

  preferences.begin("UTC", false);
  timeZone = preferences.getLong("dados");
  horarioDeVerao = preferences.getInt("verao");
  preferences.end();
  Serial.print("timeZone: ");
  Serial.println(timeZone);
  Serial.print("horarioDeVerao: ");
  Serial.println(horarioDeVerao);
  gmtOffset_sec = timeZone * 3600;
  daylightOffset_sec = horarioDeVerao * 3600;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

/**
  loop(): Código principal aqui, para executar repetidamente 
*/
void loop() {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  String dadosDeHorarios;
  preferences.begin("horarios", false);
  dadosDeHorarios = preferences.getString("dados", "");
  preferences.end();

  int qtnDeHorarios;
  preferences.begin("qtnDeHorarios", false);
  qtnDeHorarios = preferences.getInt("qtnDeHorarios", 0);
  preferences.end();

  static int cont;
  cont++;
  if (cont >= 500) {
    Serial.print(timeinfo.tm_hour);
    Serial.print(":");
    Serial.println(timeinfo.tm_min);
    cont = 0;
  }

  for (int i = 0; i < qtnDeHorarios; i++) {
    int indiceInclusivo;
    int indiceExclusivo;
    String horario;
    indiceInclusivo = 1 + dadosDeHorarios.indexOf('[', i * 7);
    indiceExclusivo = dadosDeHorarios.indexOf(']', i * 7);

    horario = dadosDeHorarios.substring(indiceInclusivo, indiceExclusivo);
    int hour = horario.substring(0, 2).toInt();
    int minute = horario.substring(3).toInt();

    if (hour == timeinfo.tm_hour && minute == timeinfo.tm_min && timeinfo.tm_sec < 2) {
      Serial.print("Evento acionado: ");
      Serial.print("Evento acionado: ");
      Serial.print(hour);
      Serial.print(":");
      Serial.println(minute);
      eventoAcionado = true;
      acionaEvento();
    }

  }

  tentaConexao();
  server.handleClient();
  delay(1);
  if (reiniciaESP)
    ESP.restart();
}

void deletaTodosNamespaces() {
  Serial.println("Partição NVS sendo limpa...");
  nvs_flash_erase(); // apague a partição NVS
  nvs_flash_init(); // inicializa a partição NVS
}

void handle_NotFound() {
  String html;
  html += "<!DOCTYPE html>";
  html += "<html lang='pt-br'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0' />";
  html += "<title>ESP32</title>";
  html += "</head>";
  html += "<body>";
  html += "<h1>Página Não encontrada</h1>";
  html += "<div id='mensagens'></div>";
  html += "<script>";

  html += "document.getElementById('mensagens').innerHTML = '";
  html += "<p>URL: ";
  html += server.uri();
  html += "</p>";

  html += "<p>Método: ";
  html += (server.method() == HTTP_GET) ? "GET" : "POST";
  html += "</p>";

  html += "<p>Argumentos: ";
  html += server.args();
  html += "</p>";
  for (uint8_t i = 0; i < server.args(); i++) {
    html += "<p> " + server.argName(i) + ": " + server.arg(i) + "</p>";
  }
  html += "';";
  html += "</script>";
  html += "</body>";
  html += "</html>";
  server.send(200, "text/html", html);
}

void acionaEvento() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  digitalWrite(LED_BUILTIN, LOW);
  eventoAcionado = false;
}
