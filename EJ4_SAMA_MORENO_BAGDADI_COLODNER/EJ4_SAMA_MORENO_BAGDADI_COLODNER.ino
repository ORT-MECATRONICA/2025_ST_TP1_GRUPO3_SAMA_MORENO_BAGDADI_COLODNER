// GRUPO 3 - SAMA MORENO BAGDADI COLODNER

#include <U8g2lib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 23  // pin del dht11
#define DHTTYPE DHT11 // tipo de dht (hay otros)

DHT dht(DHTPIN, DHTTYPE); // inicializar sensor


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

#define PIN_SW1 35
#define PIN_SW2 34
#define LED_BUILTIN 2

#define RST 0
#define PANTALLA1 1
#define PANTALLA2 5

int estadoActual = RST;
int umbralTemperatura = 28;
int lecturaSensor;

// delay sin bloquear
unsigned long TiempoUltimoCambio = 0;
const long Intervalo = 1000;

struct {
  bool estadoAnterior;
  bool estadoActual;
  int ticks;
  bool flag = false;
  int pin;
} antiReboteData[2];

void antiRebote() {
  static auto prevMillis = millis();  // static auto = se delcara una vez, no todas las veces en las que la funcion se ejecuta
  if (millis() > prevMillis) {
    for (int i = 0; i < 2; i++) {
      antiReboteData[i].estadoActual = digitalRead(antiReboteData[i].pin);  // guardo el estado del boton
      if (antiReboteData[i].estadoAnterior != antiReboteData[i].estadoActual) {
        antiReboteData[i].ticks = 5;
      } else if (antiReboteData[i].ticks > 0) {
        antiReboteData[i].ticks--;           // empiezo a restar
        if (antiReboteData[i].ticks == 0) {  // si llega a 0, significa que el robot esta presionado
          antiReboteData[i].flag = true;
        }
      }
      antiReboteData[i].estadoAnterior = antiReboteData[i].estadoActual;
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_SW1, INPUT);
  pinMode(PIN_SW2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  antiReboteData[0].pin = PIN_SW1;
  antiReboteData[1].pin = PIN_SW2;

  Serial.begin(9600);
  dht.begin(); // inicializo el dht
}

void loop() {

  float temperatura = dht.readTemperature();
  unsigned long TiempoAhora = 0;
  // MAQUINA DE ESTADOS
  switch (estadoActual) {
    case RST:
      TiempoUltimoCambio = 0;
      estadoActual = PANTALLA1;
      break;

    case PANTALLA1:
      antiRebote();

      TiempoAhora = millis();


      if ((antiReboteData[1].flag && antiReboteData[1].estadoActual) && (antiReboteData[0].flag && antiReboteData[0].estadoActual)) {
        antiReboteData[1].flag = false;
        antiReboteData[0].flag = false;
        TiempoUltimoCambio = 0;
        estadoActual = PANTALLA2;
      }
      if (TiempoAhora - TiempoUltimoCambio >= Intervalo) {  //delay sin bloqueo
        TiempoUltimoCambio = TiempoAhora;
        char buffer[5];
        char buffer2[5];
        sprintf(buffer, "%d", temperatura); // guardo el resultado formateado
        sprintf(buffer2, "%d", umbralTemperatura);

        u8g2.clearBuffer();                             // borro la memoria interna
        u8g2.setFont(u8g2_font_helvB18_tr);             // defino fuenre
        u8g2.drawStr(10, 30, "Temperatura:");  // Muestra "
        u8g2.drawStr(10, 50, buffer);
        u8g2.drawStr(10, 70, "Umbral Temperatura:");  // Muestra "
        u8g2.drawStr(10, 90, buffer2);

        u8g2.sendBuffer();  // transfiero la memoria interna a la pantalla      }
      }
        break;

        case PANTALLA2:

          antiRebote();

          TiempoAhora = millis();

          if ((antiReboteData[1].flag && antiReboteData[1].estadoActual) && (antiReboteData[0].flag && antiReboteData[0].estadoActual)) {
            antiReboteData[1].flag = false;
            antiReboteData[0].flag = false;
            estadoActual = PANTALLA1;
          }

          if (antiReboteData[0].flag && !antiReboteData[0].estadoActual) {
            antiReboteData[0].flag = false;
            umbralTemperatura++;
          }

          if (antiReboteData[1].flag && !antiReboteData[1].estadoActual) {
            antiReboteData[1].flag = false;
            umbralTemperatura--;
          }

          if (TiempoAhora - TiempoUltimoCambio >= Intervalo) {  //delay sin bloqueo
            TiempoUltimoCambio = TiempoAhora;
            char buffer[5];
            sprintf(buffer, "%d", umbralTemperatura);

            u8g2.clearBuffer();                             // borro la memoria interna
            u8g2.setFont(u8g2_font_helvB18_tr);             // defino fuenre
            u8g2.drawStr(10, 30, "Umbral Temperatura:");  
            u8g2.drawStr(10, 50, buffer);
            u8g2.sendBuffer();  // transfiero la memoria interna a la pantalla
          }
        break;
      
  }
}