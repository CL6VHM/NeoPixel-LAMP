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

  Serial.println("Sistema iniciado. Envía 'Bxxx' por Serial para ajustar el brillo (0-255).");
  Serial.println("Envía 'RRRGGGBBB' por Bluetooth para cambiar el color.");
}

void loop() {
  // --- Control de color vía Bluetooth (SoftwareSerial) ---
  if (BTSerial.available()) {
    String command = BTSerial.readStringUntil('\n');
      Serial.print("Info en bruto:  ");
      Serial.println(command);
     
    command.trim();
      
    // Se espera un comando de 9 caracteres en el formato "RRRGGGBBB"
    if (command.length() == 9) {
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
  }
  
  // --- Control de brillo vía puerto Serial (depuración) ---
  if (Serial.available()) {
    String dbgCommand = Serial.readStringUntil('\n');
    dbgCommand.trim();
    
    // Verifica si el comando inicia con 'B' o 'b'
    if (dbgCommand.startsWith("B") || dbgCommand.startsWith("b")) {
      // Extrae el valor numérico del brillo (rango 0 a 255)
      int brillo = dbgCommand.substring(1).toInt();
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
    else {
      Serial.println("Comando no reconocido para brillo. Usa 'B' seguido de un valor (0-255).");
    }
  }
}

// Función para actualizar el color de cada LED en la tira
void setColor(int red, int green, int blue) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
}
