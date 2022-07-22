void alteraCredenciais() {
  WiFi.disconnect();

  delay(1000);
  WiFi.softAP(AP_ssid, AP_password, 1, 0, 1);
  WiFi.softAPConfig(AP_local_ip, AP_gateway, AP_subnet);
  server.on("/", handle_alteraCredenciaisAP);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server AP started");

  while (1) {
    server.handleClient();
    delay(2);
    if (reiniciaESP)
      ESP.restart();
  }
}

void handle_alteraCredenciaisAP() {
  String html;

  html += " <!DOCTYPE html>";
  html += "<html lang='pt-br'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0' />";
  html += "<title>ESP32</title>";
  html += "</head>";
  html += "<body id='body'>";
  html += "<div id='telaPrincipal'> ";
  html += "<div id='bloqueiaTela'></div>";
  html += "<h1>Informe as Credenciais de Conexão</h1>";
  html += "<div id='mensagens'><p>Insira novas credenciais para acesso. Em seguida, clique em Salvar para enviar os dados para o ESP32: </p></div>";
  html += "<div style='border-style:inset; width:200px; background-color: rgb(148, 187, 242)' id='divDoForm'>";
  html += "<form action='/' method='POST' style='margin:5px'>";
  html += "<label for='id_ssid'>Informe o SSID: </label>";
  html += "<input type='text' name='SSID' id='id_ssid' placeholder='SSID para conexão' required>";
  html += "<br>";
  html += "<br>";
  html += "<label for='id_password'>Informe a Senha: </label>";
  html += "<input type='text' name='PASS' id='id_password' placeholder='Senha para conexão' required>";
  html += "<br>";
  html += "<br>";
  html += "<input type='submit' name='SUBMIT_SALVAR' value='Salvar' id='id_salvar'>";
  html += "</form>";
  html += "</div>";
  html += "</div>";
  Serial.println("Enviando html");

  if (server.arg("SUBMIT_SALVAR").length() > 0) {
    preferences.begin("config_rede", false);
    preferences.putString("ssid", server.arg("SSID"));
    preferences.putString("pass", server.arg("PASS"));
    preferences.end();
    html += messageBoxEmHtml("Credenciais Salvas", "As credenciais foram gravadas. Para acessar o sistema, reconecte-se à rede sem fio do roteador e acesse o IP do ESP32.", sucesso);
    html += "<script>";
    html += "document.getElementById('mensagens').innerHTML = 'Para acessar o sistema, reconecte-se à rede sem fio do roteador e acesse o IP do ESP32.';";
    html += "document.getElementById('divDoForm').style.display = 'none';";

    html += "</script>";
    deveReiniciar = true;
  }
  html += "</body>";
  html += "</html>";


  server.send(200, "text/html", html);
  if (deveReiniciar == true) {
    reiniciaESP = true;
  }

}
