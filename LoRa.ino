#include <SPI.h>
#include <LoRa.h>

// ======================================================
// IDENTIFICACION
// ======================================================
const int MI_ID = 5;
const int DESTINO_ID = 6;

// Cambia este texto por el mensaje que desees enviar
String mensajeOriginal = "HOLA MUNDO LORA";

// ======================================================
// PINES LORA RFM96W (SPI)
// ======================================================
#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_NSS   5
#define LORA_RST   -1
#define LORA_DIO0  -1

// ======================================================
// SENSOR TOUCH (D4 = GPIO 4 = Touch0)
// ======================================================
#define PIN_TOUCH     4
#define UMBRAL_TOUCH  30
bool tocadoAnterior = false; // Bandera para evitar envíos continuos

// ======================================================
// ATBASH
// ======================================================
char convertirAtbash(char letra) {
  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }
  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }
  return letra;
}

String cifrarAtbash(String mensaje) {
  String mensajeCifrado = "";
  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }
  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================
void enviarMensaje() {
  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  LoRa.beginPacket();
  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);
  LoRa.endPacket();

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("--- TRANSMISOR LORA ---");
  Serial.print("Mi ID: ");
  Serial.println(MI_ID);
  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  // Inicializar bus SPI con pines personalizados
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

  // Configurar pines para la librería LoRa (NSS, RST, DIO0)
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);

  // Inicializar módulo en 433 MHz
  if (!LoRa.begin(433E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa. Revisa conexiones.");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente a 433 MHz");
  Serial.println("Esperando toque en D4...");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
  int valorTouch = touchRead(PIN_TOUCH);

  // Comprueba si el pin está siendo presionado
  bool tocadoActual = (valorTouch < UMBRAL_TOUCH);

  // Detección por flanco de subida (solo al tocar por primera vez)
  if (tocadoActual && !tocadoAnterior) {
    enviarMensaje();
  }

  tocadoAnterior = tocadoActual;

  delay(50);
}
