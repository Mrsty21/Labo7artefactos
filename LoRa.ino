  #include <SPI.h>
  #include <LoRa.h>

  // ======================================================
  // IDENTIFICACION
  // ======================================================

  const int MI_ID = 1;
  const int DESTINO_ID = 2;

  String mensajeOriginal = "SISTEMAS EMBEBIDOS";

  // ======================================================
  // PINES Y CONFIGURACIÓN HARDWARE
  // ======================================================

  // Pines SPI para el módulo LoRa RFM96W
  #define SCK_PIN   18
  #define MISO_PIN  19
  #define MOSI_PIN  23
  #define SS_PIN     5   // NSS

  // Pin del sensor Capacitivo Touch y Umbral
  #define TOUCH_PIN    27
  #define TOUCH_UMBRAL 30  // Ajustable según la sensibilidad de tu placa

  // Variable para controlar el estado del toque y evitar múltiples envíos seguidos
  bool presionado = false;

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

    Serial.println("TRANSMISOR LORA");

    Serial.print("Mi ID: ");
    Serial.println(MI_ID);

    Serial.print("Destino: ");
    Serial.println(DESTINO_ID);

    // Iniciar bus SPI asignando los pines explícitos (SCK, MISO, MOSI, SS)
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

    LoRa.setPins(SS_PIN, -1, -1);

    // Inicializar radio a 433 MHz
    if (!LoRa.begin(433E6)) {
      Serial.println("ERROR: No se pudo iniciar LoRa");
      while (true);
    }

    Serial.println("LoRa iniciado correctamente");
    Serial.println("Esperando toque en GPIO 27...");
  }

  // ======================================================
  // LOOP
  // ======================================================

  void loop() {

    // Leer el valor capacitivo del sensor Touch en GPIO 27
    int valorTouch = touchRead(TOUCH_PIN);

    // Si el valor cae por debajo del umbral, se detectó un toque
    if (valorTouch < TOUCH_UMBRAL) {
      if (!presionado) {
        enviarMensaje();   // Transmitir mensaje
        presionado = true; // Bloquear nuevos envíos mientras se siga tocando
      }
    } else {
      presionado = false;  // Resetear la bandera cuando se suelta el sensor
    }

    delay(50);
  }