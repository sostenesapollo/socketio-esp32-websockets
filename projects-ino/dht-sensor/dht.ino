#include <DHT.h>

#define DHTPIN 2     // Pino ao qual o sensor DHT está conectado
#define DHTTYPE DHT11   // Tipo do sensor (DHT11 ou DHT22, por exemplo)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");

  dht.begin();
}

void loop() {
  // Aguarda um tempo mínimo entre leituras
  delay(2000);

  // Lê a umidade relativa
  float humidity = dht.readHumidity();

  // Lê a temperatura em graus Celsius
  float temperature = dht.readTemperature();

  // Verifica se a leitura foi bem-sucedida
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha ao ler o sensor DHT!");
    return;
  }

  // Imprime os valores lidos
  Serial.print("Umidade: ");
  Serial.print(humidity);
  Serial.print("%\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println("°C");
}
