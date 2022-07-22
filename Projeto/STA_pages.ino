void handle_indexSTA() {
  String html;
  if (server.arg("SUBMIT_SALVAR") == "Salvar") {
    if (server.arg("temperature") == "celsius") {
      preferences.begin("temp", false);
      preferences.putString("faixa", "celsius");
      preferences.end();
    } else if (server.arg("temperature") == "fahrenheit") {
      preferences.begin("temp", false);
      preferences.putString("faixa", "fahrenheit");
      preferences.end();
    }
  }



  if (server.arg("SUBMIT_SALVAR") == "remover") {
    if (server.arg("horariosLista").length() > 0) {
      String _horarios = preferences.getString("dados");
      preferences.begin("horarios", false);
      _horarios = preferences.getString("dados", "");
      preferences.end();

      int _qtnDeHorarios;
      preferences.begin("qtnDeHorarios", false);
      _qtnDeHorarios = preferences.getInt("qtnDeHorarios", 0);
      preferences.end();

      //DEBUG
      Serial.print("antes _horarios:");
      Serial.println(_horarios);
      Serial.print("antes _qtnDeHorarios:");
      Serial.println(_qtnDeHorarios);

      String aSubstituir = "[";
      aSubstituir += server.arg("horariosLista");
      aSubstituir += "]";
      _horarios.replace(aSubstituir, "");
      _qtnDeHorarios--;

      //DEBUG
      Serial.print("aSubstituir:");
      Serial.println(aSubstituir);

      preferences.begin("horarios", false);
      preferences.putString("dados", _horarios);
      preferences.end();

      preferences.begin("qtnDeHorarios", false);
      preferences.putInt("qtnDeHorarios", _qtnDeHorarios);
      preferences.end();

      //DEBUG
      Serial.print("_horarios:");
      Serial.println(_horarios);
      Serial.print("_qtnDeHorarios:");
      Serial.println(_qtnDeHorarios);
    }
  }

  if (server.arg("SUBMIT_SALVAR") == "adicionar") {
    if (server.arg("horarioAdicionar").length() > 0) {
      int qtnDeHorarios;
      preferences.begin("qtnDeHorarios", false);
      qtnDeHorarios = preferences.getInt("qtnDeHorarios", 0);
      qtnDeHorarios++;
      preferences.putInt("qtnDeHorarios", qtnDeHorarios);
      preferences.end();

      String aAdicionar = "[";
      aAdicionar += server.arg("horarioAdicionar");
      aAdicionar += "]";
      preferences.begin("horarios", false);
      aAdicionar += preferences.getString("dados", "");

      preferences.putString("dados", aAdicionar);
      preferences.end();
      Serial.print("aAdicionar: ");
      Serial.println(aAdicionar);

    }
  }

  if (server.arg("SUBMIT_SALVAR") == "salvarUTC") {
    if (server.arg("UTCSalvar").length() > 0) {
      preferences.begin("UTC", false);
      char stringToChar[sizeof(server.arg("UTCSalvar"))];
      server.arg("UTCSalvar").toCharArray(stringToChar, sizeof(stringToChar));
      preferences.putLong("dados", atol(stringToChar));
      preferences.end();
      Serial.print("UTCSalvar: ");
      Serial.println(server.arg("UTCSalvar"));
    }
    preferences.begin("UTC", false);
    timeZone = preferences.getLong("dados");
    horarioDeVerao = preferences.getInt("verao");
    preferences.end();

    gmtOffset_sec = timeZone * 3600;
    daylightOffset_sec = horarioDeVerao * 3600;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }

  if (server.arg("SUBMIT_SALVAR") == "verao") {
    if (server.arg("horarioDeVerao").length() > 0) {
      preferences.begin("UTC", false);
      char stringToChar[sizeof(server.arg("horarioDeVerao"))];
      server.arg("horarioDeVerao").toCharArray(stringToChar, sizeof(stringToChar));
      preferences.putInt("verao", atol(stringToChar));
      preferences.end();
    }

    preferences.begin("UTC", false);
    timeZone = preferences.getLong("dados");
    horarioDeVerao = preferences.getInt("verao");
    preferences.end();

    gmtOffset_sec = timeZone * 3600;
    daylightOffset_sec = horarioDeVerao * 3600;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }

  preferences.begin("temp", false);
  String faixaDeTemperatura = preferences.getString("faixa", "");
  preferences.end();


  html += "<!DOCTYPE html>";
  html += "<html lang='pt-br'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0' />";
  html += "<title>ESP32</title>";
  html += "<script>";
  html += "function trocaEscalaTemperatura()";
  html += "{";
  html += "document.getElementById('id_salvarTemperature').click();";
  html += "}";

  html += "function removeHorario()";
  html += "{";
  html += "document.getElementById('horarioAdicionar').value = ''; /* Limpa campo adiciona horário*/";

  html += "var elementoLista = document.getElementById('horariosLista');";
  html += "var elementoSubmit = document.getElementById('id_salvarAlerta');";

  html += "if((elementoLista.options[elementoLista.selectedIndex].value) != 'Selecione um horário'){";
  html += "  elementoSubmit.value = 'remover';";
  html += "  elementoSubmit.click();";
  html += "}else";
  html += "{";
  html += " alert('Falha na Remoção: Não é possível remover um horário de acionamento de evento sem haver um horário selecionado. Por favor selecione um horário no campo destacado e tente novamente.');";
  html += " elementoLista.focus();";
  html += "}";
  html += "}";

  html += "function adicionaHorario()";
  html += "{";
  html += "document.getElementById('horariosLista').value = document.getElementById('horariosLista').options[0].value; /* Limpa campo remoção horário*/";

  html += "var elementoAdicionar = document.getElementById('horarioAdicionar');";
  html += "var elementoSubmit = document.getElementById('id_salvarAlerta');";

  html += "if((elementoAdicionar.value).length > 0){";
  html += " elementoSubmit.value = 'adicionar';";
  html += " elementoSubmit.click();";
  html += "}else{";
  html += " alert('Falha na Adição: Não é possível adicionar um novo acionamento de evento sem haver um horário preenchido. Por favor prencha o campo destacado e tente novamente.');";
  html += " elementoAdicionar.focus();";
  html += "}";


  html += "}";

  html += "function salvarUTC()";
  html += "{";
  html += "var elementoLista = document.getElementById('UTCSalvar');";
  html += "var elementoSubmit = document.getElementById('id_salvarUTC');";
  html += "if((elementoLista.options[elementoLista.selectedIndex].value) != 'Selecione um UTC'){";
  html += " elementoSubmit.value = 'salvarUTC';";
  html += " elementoSubmit.click();";
  html += "}else{";
  html += " alert('Falha na Adição: Não é possível adicionar um novo acionamento de evento sem haver um UTC preenchido. Por favor prencha o campo destacado e tente novamente.');";
  html += " elementoLista.focus();";
  html += "}";



  html += "}";

  html += "function salvarVerao()";
  html += "{";
  html += "document.getElementById('id_salvarVerao').click();";
  html += "}";



  html += "function esconderDIV(div,msg)";
  html += "{";
  html += "if(document.getElementById(div).style.display == 'block')";
  html += "{";
  html += "document.getElementById(msg).innerHTML = '+';";
  html += "document.getElementById(div).style.display = 'none';";
  html += "}";
  html += "else";
  html += "{";
  html += "document.getElementById(msg).innerHTML = '-';";
  html += "document.getElementById(div).style.display = 'block';";
  html += "}";
  html += "}";

  html += "</script>";
  html += "</head>";
  html += "<body id='body'>";

  html += "<div id='telaPrincipal'> ";
  html += "<div id='bloqueiaTela'></div>";
  html += "<h1>ESP32 - Exemplo de Preferences</h1>";

  html += "<div id='utc'>";
  html += "<form action='/' method='POST' id='utc_select_form'>";
  html += "UTC: ";
  html += "<input type='submit' name='SUBMIT_SALVAR' value='verao' id='id_salvarUTC' style='display: none;'>";
  html += "</form>";


  html += "<select Name='UTCSalvar' id='UTCSalvar' form='utc_select_form'>";
  html += "<option> Selecione um UTC </option>";
  html += "<option value=-12";
  if (timeZone == -12)
    html += " selected";
  html += "> -12:00 </option>";


  html += "<option value=-11";
  if (timeZone == -11)
    html += " selected";
  html += "> -11:00 </option>";

  html += "<option value=-10";
  if (timeZone == -10)
    html += " selected";
  html += "> -10:00 </option>";

  html += "<option value=-9.5";
  if (timeZone == -9.5)
    html += " selected";
  html += "> -09:30 </option>";

  html += "<option value=-9";
  if (timeZone == -9)
    html += " selected";
  html += "> -09:00 </option>";

  html += "<option value=-8";
  if (timeZone == -8)
    html += " selected";
  html += "> -08:00 </option>";

  html += "<option value=-7";
  if (timeZone == -7)
    html += " selected";
  html += "> -07:00 </option>";

  html += "<option value=-6";
  if (timeZone == -6)
    html += " selected";
  html += "> -06:00 </option>";

  html += "<option value=-5";
  if (timeZone == -5)
    html += " selected";
  html += "> -05:00 </option>";

  html += "<option value=-4";
  if (timeZone == -4)
    html += " selected";
  html += "> -04:00 </option>";

  html += "<option value=-3.5";
  if (timeZone == -3.5)
    html += " selected";
  html += "> -03:30 </option>";

  html += "<option value=-3";
  if (timeZone == -3)
    html += " selected";
  html += "> -03:00 </option>";

  html += "<option value=-2.5";
  if (timeZone == -2.5)
    html += " selected";
  html += "> -02:30 </option>";

  html += "<option value=-2";
  if (timeZone == -2)
    html += " selected";
  html += "> -02:00 </option>";

  html += "<option value=-1";
  if (timeZone == -1)
    html += " selected";
  html += "> -01:00 </option>";

  html += "<option value=0";
  if (timeZone == 0)
    html += " selected";
  html += "> &nbsp;00:00 </option>";

  html += "<option value=1";
  if (timeZone == 1)
    html += " selected";
  html += "> +01:00 </option>";

  html += "<option value=2";
  if (timeZone == 2)
    html += " selected";
  html += "> +02:00 </option>";

  html += "<option value=3";
  if (timeZone == 3)
    html += " selected";
  html += "> +03:00 </option>";

  html += "<option value=3.5";
  if (timeZone == 3.5)
    html += " selected";
  html += "> +03:30 </option>";

  html += "<option value=4";
  if (timeZone == 4)
    html += " selected";
  html += "> +04:00 </option>";

  html += "<option value=4.5";
  if (timeZone == 4.5)
    html += " selected";
  html += "> +04:30 </option>";

  html += "<option value=5>";
  if (timeZone == 5)
    html += " selected";
  html += " +05:00 </option>";

  html += "<option value=5.5";
  if (timeZone == 5.5)
    html += " selected";
  html += "> +05:30 </option>";

  html += "<option value=5.75";
  if (timeZone == 5.75)
    html += " selected";
  html += "> +05:45 </option>";

  html += "<option value=6";
  if (timeZone == 6)
    html += " selected";
  html += "> +06:00 </option>";

  html += "<option value=6.5";
  if (timeZone == 6.5)
    html += " selected";
  html += "> +06:30 </option>";

  html += "<option value=7";
  if (timeZone == 7)
    html += " selected";
  html += "> +07:00 </option>";

  html += "<option value=8";
  if (timeZone == 8)
    html += " selected";
  html += "> +08:00 </option>";

  html += "<option value=8.75";
  if (timeZone == 8.75 )
    html += " selected";
  html += "> +08:45 </option>";

  html += "<option value=9";
  if (timeZone == 9)
    html += " selected";
  html += "> +09:00 </option>";

  html += "<option value=9.5";
  if (timeZone == 9.5)
    html += " selected";
  html += "> +09:30 </option>";

  html += "<option value=10";
  if (timeZone == 10)
    html += " selected";
  html += "> +10:00 </option>";

  html += "<option value=10.5";
  if (timeZone == 10.5)
    html += " selected";
  html += "> +10:30 </option>";

  html += "<option value=11";
  if (timeZone == 11)
    html += " selected";
  html += "> +11:00 </option>";

  html += "<option value=12";
  if (timeZone == 12)
    html += " selected";
  html += "> +12:00 </option>";

  html += "<option value=12.75";
  if (timeZone == 12.75)
    html += " selected";
  html += "> +12:45 </option>";

  html += "<option value=13";
  if (timeZone == 13)
    html += " selected";
  html += "> +13:00 </option>";

  html += "<option value=13.75";
  if (timeZone == 13.75)
    html += " selected";
  html += "> +13:45 </option>";

  html += "<option value=14";
  if (timeZone == 14)
    html += " selected";
  html += "> +14:00 </option>";

  html += "</select>";
  html += "<button name='buttonUTC' id='buttonUTC' value='salvarUTC' type='button' onclick='salvarUTC()'>Salvar UTC</button>";
  html += "</div>";
  html += "<br>";
  html += "<br>";

  html += "<div id='verao'>";
  html += "<form action='/' method='POST'>";
  html += "Horário de Verão? ";
  html += "<input type='radio' name='horarioDeVerao' id='horario_sim' onchange='salvarVerao()' value=3600 ";

  if (horarioDeVerao == 3600) {
    html += "checked";
  }
  html += ">";
  html += "";
  html += "<label for='horario_sim'>Sim (1h)</label>";

  html += "<input type='radio' name='horarioDeVerao' id='horario_nao' onchange='salvarVerao()' value=0  ";
  if (horarioDeVerao == 0) {
    html += "checked";
  }
  html += ">";
  html += "<label for='horario_nao'>Não</label>";
  html += "<input type='submit' name='SUBMIT_SALVAR' value='verao' id='id_salvarVerao' style='display: none;'>";
  html += "</form>";
  html += "</div>";
  html += "<br>";
  html += "<br>";

  html += "<div id='temperature'>";
  html += "<form action='/' method='POST'>";
  html += "Temperatura: ";
  if (faixaDeTemperatura == "celsius") {
    if (isnan(dht.readTemperature())) {
      html += "Falha na obtenção de Temperatura. Verifique o hardware.";
    }
    else {
      html += String(dht.readTemperature());
    }
  } else if (faixaDeTemperatura == "fahrenheit") {
    if (isnan(dht.readTemperature(true))) {
      html += "Falha na obtenção de Temperatura. Verifique o hardware.";
    }
    else {
      html += String(dht.readTemperature(true));
    }
  } else {
    html += "ERRO: Defina uma escala de temperatura.";
  }

  html += "º";

  html += "<input type='radio' name='temperature' id='celsius' onchange='trocaEscalaTemperatura()' value='celsius' ";
  if (faixaDeTemperatura == "celsius") {
    html += "checked";
  }
  html += ">";
  html += "";
  html += "<label for='celsius'>Celsius</label>";

  html += "<input type='radio' name='temperature' id='fahrenheit' onchange='trocaEscalaTemperatura()' value='fahrenheit'  ";
  if (faixaDeTemperatura == "fahrenheit") {
    html += "checked";
  }
  html += ">";
  html += "<label for='fahrenheit'>Fahrenheit</label>";

  html += "<input type='submit' name='SUBMIT_SALVAR' value='Salvar' id='id_salvarTemperature' style='display: none;'>";
  html += "</form>";
  html += "</div>";

  html += "<br>";



  html += "<form action='/' method='POST'>";
  html += "Acionamento do Evento";
  html += "<div id='alerta' style=' margin-left:20px;'> <!-- Espaçamento-->";
  html += "<div style=' width:250px;  margin-top:1px;'> <!-- Estilo Remover-->";
  html += "Remoção <button name='buttonRemove' id='remover' type='button' onclick='esconderDIV(\"divRemover\",this.id)'>+</button>";
  html += "<div style='margin:5px; display:none;' id='divRemover'> <!-- Margem Interna-->";
  html += "<select Name='horariosLista' id='horariosLista'>";
  html += "<option> Selecione um horário </option>";

  String dadosDeHorarios = preferences.getString("dados");
  preferences.begin("horarios", false);
  dadosDeHorarios = preferences.getString("dados", "");
  preferences.end();

  int qtnDeHorarios;
  preferences.begin("qtnDeHorarios", false);
  qtnDeHorarios = preferences.getInt("qtnDeHorarios", 0);
  preferences.end();

  for (int i = 0; i < qtnDeHorarios; i++) {
    int indiceInclusivo;
    int indiceExclusivo;
    String horario;
    indiceInclusivo = 1 + dadosDeHorarios.indexOf('[', i * 7);
    indiceExclusivo = dadosDeHorarios.indexOf(']', i * 7);

    horario = dadosDeHorarios.substring(indiceInclusivo, indiceExclusivo);

    Serial.println(horario);
    html += "<option>";
    html += horario;
    html += "</option>";
  }

  html += "</select>";
  html += "<button name='buttonRemove' id='remover' value='remover' type='button' onclick='removeHorario()'>Remover</button>";
  html += "</div> <!-- Fim Margem Interna-->";
  html += "</div><!-- Fim Estilo Remover-->";
  html += "<div style=' width:250px;margin-top:1px;'><!-- Estilo Adicionar-->";
  html += "Adição <button name='buttonRemove' id='adicionar' type='button' onclick='esconderDIV(\"divAdicionar\",this.id)'>+</button>";
  html += "<div style='margin:5px;display:none;' id='divAdicionar'> <!-- Margem Interna-->";
  html += "<input type='time' name='horarioAdicionar' id='horarioAdicionar'>";

  html += "<button name='buttonAdiciona' id='adicionarButton' value='adicionar' type='button' onclick='adicionaHorario()'>Adicionar</button>";
  html += "</div><!-- Fim Margem Interna-->";
  html += "</div><!-- Fim Estilo Adicionar-->";

  html += "</div> <!-- Fim Espaçamento-->";

  html += "<input type='submit' name='SUBMIT_SALVAR' value='Salvar' id='id_salvarAlerta' style='display: none;'>";

  html += "</form>";
  html += "<br>";
  html += "<form action='/' method='POST'><input type='submit' name='SUBMIT_RESET' value='Resetar para configs. de Fábrica' id='id_sair'></form>";
  html += "</div>";


  if (eventoAcionado == true) {

    html += messageBoxEmHtml("Acionamento do Evento", "O Evento --:-- foi acionado", informacao);
  }
  html += "<form>";
  if (server.arg("SUBMIT_RESET").length() > 0) {
    deletaTodosNamespaces();
    html += messageBoxEmHtml("Configurações de Fábrica", "As informações foram resetadas para o estado inicial de fábrica. O dispositivo será reiniciado em seguida. Por favor faça novamente a conexão sem fio com o Access Point com o Dispositivo.", sucesso);
    deveReiniciar = true;
  }
  html += "</body>";
  html += "</html>";



  server.send(200, "text/html", html);
  if (deveReiniciar == true) {
    reiniciaESP = true;
  }
}

String messageBoxEmHtml(String titulo, String corpoMensagem, tipoMensagem mensagem) {
  String html;
  html = "<div id ='mensagem' style='";
  html += "margin: 0;";
  html += "display:block;";
  html += "position: fixed;";
  html += "top: 50%;";
  html += "left: 50%;";
  html += "min-width: 300px;";
  html += "min-height:120px;";
  html += "-ms-transform: translate(-50%, -50%);";
  html += "transform: translate(-50%, -50%);";

  switch (mensagem) {
    case alerta:
      html += "border: 3px solid #ff5c5c;";
      html += "background-color:#ff8c8c;";
      break;

    case aviso:
      html += "border: 3px solid #ffa05c;";
      html += "background-color:#ffbc8c;";
      break;

    case informacao:
      html += "border: 3px solid #5cb3ff;";
      html += "background-color:#8cc9ff;";
      break;

    case sucesso:
      html += "border: 3px solid #5cffa3;";
      html += "background-color:#8cffbe;";
      break;

    default:
      html += "border: 3px solid #5cb3ff;";
      html += "background-color:#8cc9ff;";
      break;

  }

  html += "border: 3px solid #5cb3ff;";
  html += "background-color:#8cc9ff;";
  html += "'>";
  html += "<div style='margin:5px;float: right;'><button id='escButton' onclick='botao()'>X</button></div>";
  html += "<div style='margin-bottom:30px;'>";
  html += "<div style='margin:5px; font-family:Maiandra GD; font-weight:bold'>";
  html += titulo;
  html += "</div>";
  html += "<div style='margin:5px; margin-top:10px; max-height:300px;overflow-y: auto;'>";
  html += corpoMensagem;
  html += "</div>";
  html += "</div>";
  html += "<div style='position: absolute;";
  html += "bottom: 5px;";
  html += "left: 5px;'>";
  html += "<button id='enterButton'  onclick='botao()'>OK</button>";
  html += "</div>";
  html += "<script>";
  html += "function botao(){";
  html += "document.getElementById('body').style = 'height:100%;';";
  html += "document.getElementById('telaPrincipal').style = 'width:100%; height:100%;';";
  html += "document.getElementById('bloqueiaTela').style = '';";
  html += "document.getElementById('mensagem').style.display = 'none';";
  html += "}";
  html += "document.addEventListener('keydown', function(e) {";
  html += "if(e.key == 'Enter' && document.getElementById('mensagem').style.display == 'block'){";
  html += " document.getElementById('enterButton').click();";
  html += "}";
  html += "else if(e.key == 'Escape' && document.getElementById('mensagem').style.display == 'block'){";
  html += "document.getElementById('escButton').click();";
  html += "}";
  html += "});";
  html += "document.getElementById('body').style = 'height:100%; background-color: rgb(100,100,100);';";
  html += "document.getElementById('bloqueiaTela').style = 'position:fixed;width:100%; height:100%';";
  html += "document.getElementById('telaPrincipal').style = 'width:100%; height:100%; opacity: 0.2;';";
  html += "</script>";
  html += "</div>";
  return html;
}
