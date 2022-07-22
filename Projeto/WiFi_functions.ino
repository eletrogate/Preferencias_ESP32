void tentaConexao() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Desconectado da rede. Tentando conexão");
    int indiceConexao = 70;
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.print(".");
      delay(50);
      digitalWrite(LED_BUILTIN, LOW);
      delay(50);
      indiceConexao--;
      if (indiceConexao <= 0) {
        indiceConexao = 70;
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(ssid.c_str(), pass.c_str());
      }
    }
    Serial.println();
    Serial.println("Conectado!");
    digitalWrite(LED_BUILTIN, LOW);
  }
}
