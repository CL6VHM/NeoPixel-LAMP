#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
// Configuración de la tira NeoPixel
#define NEOPIXEL_PIN 6      // Pin de datos conectado a la tira
#define NUMPIXELS 72         // Número total de LEDs en la tira

// Configuración de SoftwareSerial para el módulo Bluetooth
#define BT_RX_PIN 2         // Conecta al pin TX del módulo Bluetooth
#define BT_TX_PIN 3         // Conecta al pin RX del módulo Bluetooth

// Inicializa SoftwareSerial (Bluetooth)
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN); // (RX, TX)

// Inicializa la tira NeoPixel (parámetro: número de pixeles, pin de datos, tipo de LED)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ400);

void setup() {
  // Inicializa el puerto serial de hardware para depuración
  Serial.begin(9600);
 
  // Inicializa el puerto de SoftwareSerial para Bluetooth
  BTSerial.begin(9600);
  
  // Inicializa la tira NeoPixel
  strip.begin();
  strip.show();  // Asegura que todos los LEDs estén apagados inicialmente
  
  // Establece el color predeterminado (37, 166, 154)
  setColor(37, 166, 154);
  
  // Establece un brillo por defecto (por ejemplo, máximo 255)
  strip.setBrightness(255);

  Serial.println("Sistema iniciado. Envía:");
  Serial.println("'Bxxx' por Serial o Bluetooth para ajustar el brillo (0-255).");
  Serial.println("'RRRGGGBBB' por Bluetooth para cambiar el color.");
  Serial.println("'R' para modo arcoiris y 'L' para modo blanco.");
}

void loop() {
  // --- Control de comandos vía Bluetooth (SoftwareSerial) ---
  if (BTSerial.available()) {
    String command = BTSerial.readStringUntil('\n');
    Serial.print("Info en bruto desde Bluetooth: ");
    Serial.println(command);
     
    command.trim();

    processCommand(command);
  }
  
  // --- Control de comandos vía puerto Serial (depuración) ---
  if (Serial.available()) {
    String dbgCommand = Serial.readStringUntil('\n');
    dbgCommand.trim();

    Serial.print("Info en bruto desde Serial: ");
    Serial.println(dbgCommand);

    processCommand(dbgCommand);
  }
}

// Función para procesar comandos de brillo, color, modo arcoiris y modo blanco
void processCommand(String command) {
  // Control de brillo
  if (command.startsWith("B") || command.startsWith("b")) {
    int brillo = command.substring(1).toInt();
    if (brillo < 0) {
      brillo = 0;
    } else if (brillo > 255) {
      brillo = 255;
    }
    
    strip.setBrightness(brillo);
    strip.show();  // Aplica la modificación de brillo re-renderizando los colores actuales
    Serial.print("Brillo actualizado a: ");
    Serial.println(brillo);
  }
  // Control de modo arcoiris
  else if (command.equalsIgnoreCase("R")) {
    Serial.println("Activando modo arcoiris...");
    rainbowMode();
  }
  // Control de modo blanco
  else if (command.equalsIgnoreCase("L")) {
    Serial.println("Activando modo blanco...");
    setColor(255, 255, 255); // Blanco puro
  }
  // Control de color específico
  else if (command.length() == 9) {
    // Extrae los valores de color de la cadena
    int redValue   = command.substring(0, 3).toInt();
    int greenValue = command.substring(3, 6).toInt();
    int blueValue  = command.substring(6, 9).toInt();

    // Actualiza el color de todos los LEDs en la tira
    setColor(redValue, greenValue, blueValue);

    // Envía datos al monitor serie para depuración
    Serial.print("Nuevo color establecido: ");
    Serial.print(redValue);
    Serial.print(", ");
    Serial.print(greenValue);
    Serial.print(", ");
    Serial.println(blueValue);
    Serial.println(" ");
  }
  else {
    Serial.println("Comando no reconocido.");
  }
}

// Función para actualizar el color de cada LED en la tira
void setColor(int red, int green, int blue) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
}

// Función para activar el modo arcoiris
void rainbowMode() {
  for (int j = 0; j < 256; j++) { // Ciclo completo del arcoiris (256 pasos)
    for (int i = 0; i < NUMPIXELS; i++) {
      int pixelHue = (i * 256 / NUMPIXELS + j) & 255; // Calcula el color basado en el índice y el paso
      strip.setPixelColor(i, strip.ColorHSV(pixelHue * 65536 / 256)); // Convierte a color HSV
    }
    strip.show();
    delay(10); // Controla la velocidad del arcoiris
  }
}
