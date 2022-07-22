/***************************************************
  Exemplo de Salvamento de Prefêrencias no ESP32

  Criado em 03 de Novembro de 2021 por Michel Galvão
 ****************************************************/

// Inclusão da(s) biblioteca(s)
#include <Preferences.h>

// Criação de objeto(s) da(s) classe(s)
Preferences preferences;

// Comente as linhas de acordo com o modo desejado
//#define MODO_LEITURA
#define MODO_ESCRITA
//#define MODO_DELETE true
//#define MODO_LIMPAR_NAMESPACES

// Inclusão de bibioteca somente no modo MODO_LIMPAR_NAMESPACES
#ifdef MODO_LIMPAR_NAMESPACES

// somente é necessário incluir esta biblioteca quando for limpar todos namespaces
#include <nvs_flash.h>

#endif

// Variáveis Globais
char caractere;
int numeroInteiro;
uint32_t numeroInteiroPositivo;
float numeroDecimal;
bool booleano;
String palavra;

/**
  setup(): Código de configuração aqui, para executar uma vez
*/
void setup() {
  Serial.begin(115200); // Configura a taxa de transferência em bits por
  //                      segundo (baud rate) para transmissão serial.
  delay(1000);
  Serial.println();

  /**
    Modo para impar todos namespaces
  */
#ifdef MODO_LIMPAR_NAMESPACES

  Serial.println("Partição NVS sendo limpa...");
  nvs_flash_erase(); // apague a partição NVS
  nvs_flash_init(); // inicializa a partição NVS
  while (1);

#endif

  /**
    bool begin(const char * name, bool readOnly=false);

    Abra Preferências com o nome de namespace informado.
    @param name:      indica o nome do namespace
    @param readOnly:  opcional, true indica abrir ou criar o namespace no modo somente leitura E
                      false indica abrir ou criar o namespace  no modo de leitura/gravação
  */
  preferences.begin("config_aleats", false);

#ifdef MODO_LEITURA

  // Obtenha os valores-chave (valor de leitura)
  /**
    int8_t getChar(const char* key, int8_t defaultValue = 0);

    Obtenha o valor da chave informada no tipo char(int8_t).
    @param key:           indica o nome da chave
    @param defaultValue:  opcional, valor para retornar caso não seja encontrado valor
  */
  caractere = preferences.getChar("key_carac", '-');
  numeroInteiro = preferences.getInt("key_nInt", 0);
  numeroInteiroPositivo = preferences.getUInt("key_nIntPosi", 0);  
  numeroDecimal = preferences.getFloat("key_nDec", 0);
  booleano = preferences.getBool("key_boolean", false);
  palavra = preferences.getString("key_palavra", "NULL");

#elif defined(MODO_ESCRITA)
  // Coloque valores-chave (salve um valor)
  /**
    size_t putChar(const char* key, int8_t value);

    Coloca o valor informado da chave no tipo char(int8_t).
    @param key:    indica o nome da chave
    @param value:  valor para setar
  */
  preferences.putChar("key_carac", 'e');
  preferences.putInt("key_nInt", 23);
  preferences.putUInt("key_nIntPosi", 8764);
  preferences.putFloat("key_nDec", 3.1415);
  preferences.putBool("key_boolean", true);
  preferences.putString("key_palavra", "Eletrogate - eletrogate.com");

  // Obtenha os valores-chave (valor de leitura)
  caractere = preferences.getChar("key_carac", '-');
  numeroInteiro = preferences.getInt("key_nInt", 0);
  numeroInteiroPositivo = preferences.getUInt("key_nIntPosi", 0);
  numeroDecimal = preferences.getFloat("key_nDec", 0);
  booleano = preferences.getBool("key_boolean", false);
  palavra = preferences.getString("key_palavra", "NULL");

#elif defined(MODO_DELETE)
  Serial.print("MODO_DELETE: ");
  Serial.println(MODO_DELETE);
  if (MODO_DELETE == true) {
    // Limpa todas as preferências no namespace aberto (não exclui o namespace)
    preferences.clear();
  } else {
    // Remova as chaves dos namespace's
    preferences.remove("key_carac");
    preferences.remove("key_nInt");
    preferences.remove("key_nIntPosi");
    preferences.remove("key_nDec");
    preferences.remove("key_boolean");
    //preferences.remove("key_palavra");
  }

  // Obtenha os valores-chave (valor de leitura)
  caractere = preferences.getChar("key_carac", '-');
  numeroInteiro = preferences.getInt("key_nInt", 0);
  numeroInteiroPositivo = preferences.getUInt("key_nIntPosi", 0);
  numeroDecimal = preferences.getFloat("key_nDec", 0);
  booleano = preferences.getBool("key_boolean", false);
  palavra = preferences.getString("key_palavra", "NULL");

#endif

  /**
    void end();

    Fecha as preferências no namespace aberto.
  */
  preferences.end();

  // Exibe no monitor Serial
  Serial.print("caractere : ");
  Serial.println(caractere);

  Serial.print("numeroInteiro : ");
  Serial.println(numeroInteiro);

  Serial.print("numeroInteiroPositivo : ");
  Serial.println(numeroInteiroPositivo);

  Serial.print("numeroDecimal : ");
  Serial.println(numeroDecimal);

  Serial.print("booleano : ");
  Serial.println(booleano);

  Serial.print("palavra : ");
  Serial.println(palavra);

  while (1); // estrutura de repetição infinita

}

/**
  loop(): Código principal aqui, para executar repetidamente
*/
void loop() {
  //Nenhum código necessário: toda operação é feita no setup
}