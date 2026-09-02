#include <Wire.h>
#include <math.h>

#define GY33_ADDRESS 0x5A
#define NUMERO_DIREZIONI 4

struct RGB {
  uint16_t rosso;
  uint16_t verde;
  uint16_t blu;
  uint16_t clear;
};

struct DirezioneColore {
  const char *nome;
  float r;
  float g;
  float b;
};

// Valori ricavati dalle letture valide della calibrazione
DirezioneColore direzioni[NUMERO_DIREZIONI] = {
  {"DESTRA",   0.2226, 0.4528, 0.3245},
  {"SINISTRA", 0.3440, 0.3432, 0.3128},
  {"INDIETRO", 0.2081, 0.3734, 0.4185},
  {"AVANTI",   0.1943, 0.4264, 0.3793}
};

// Più basso = riconoscimento più selettivo
const float SOGLIA_MASSIMA = 0.035;

bool leggiGY33(RGB &colore) {
  uint8_t dati[8];

  Wire.beginTransmission(GY33_ADDRESS);
  Wire.write(0x00);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  uint8_t ricevuti =
    Wire.requestFrom(
      (uint8_t)GY33_ADDRESS,
      (uint8_t)8
    );

  if (ricevuti != 8) {
    return false;
  }

  for (int i = 0; i < 8; i++) {
    dati[i] = Wire.read();
  }

  colore.rosso =
    ((uint16_t)dati[0] << 8) | dati[1];

  colore.verde =
    ((uint16_t)dati[2] << 8) | dati[3];

  colore.blu =
    ((uint16_t)dati[4] << 8) | dati[5];

  colore.clear =
    ((uint16_t)dati[6] << 8) | dati[7];

  return true;
}

float distanzaColore(
  float r1,
  float g1,
  float b1,
  float r2,
  float g2,
  float b2
) {
  float differenzaR = r1 - r2;
  float differenzaG = g1 - g2;
  float differenzaB = b1 - b2;

  return sqrt(
    differenzaR * differenzaR +
    differenzaG * differenzaG +
    differenzaB * differenzaB
  );
}

int riconosciDirezione(
  float r,
  float g,
  float b,
  float &distanzaMigliore
) {
  int migliore = -1;
  distanzaMigliore = 1000;

  for (int i = 0; i < NUMERO_DIREZIONI; i++) {
    float distanza = distanzaColore(
      r,
      g,
      b,
      direzioni[i].r,
      direzioni[i].g,
      direzioni[i].b
    );

    if (distanza < distanzaMigliore) {
      distanzaMigliore = distanza;
      migliore = i;
    }
  }

  if (distanzaMigliore > SOGLIA_MASSIMA) {
    return -1;
  }

  return migliore;
}

void eseguiDirezione(int indice) {
  Serial.print(">>> DIREZIONE ATTIVATA: ");
  Serial.println(direzioni[indice].nome);

  switch (indice) {
    case 0:
      // DESTRA
      // Inserisci qui il relativo comando Roomba
      // oppure il comando per il driver motori.
      break;

    case 1:
      // SINISTRA
      break;

    case 2:
      // INDIETRO
      break;

    case 3:
      // AVANTI
      break;
  }
}

void eseguiStop() {
  Serial.println(">>> DIREZIONE ATTIVATA: STOP");

  // Inserisci qui il comando di arresto del Roomba
  // oppure del driver motori.
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Collegamenti GY-33:
  // DR/SDA → GPIO4
  // CT/SCL → GPIO5
  Wire.begin(4, 5);
  Wire.setClock(100000);

  Serial.println();
  Serial.println("Riconoscimento direzioni tramite GY-33");
  Serial.println("Colore non riconosciuto = STOP");
}

void loop() {
  RGB colore;

  if (!leggiGY33(colore)) {
    Serial.println("Errore I2C | STOP");
    eseguiStop();
    delay(500);
    return;
  }

  float totale =
    (float)colore.rosso +
    colore.verde +
    colore.blu;

  if (totale <= 0) {
    Serial.println("Lettura non valida | STOP");
    eseguiStop();
    delay(500);
    return;
  }

  float r = colore.rosso / totale;
  float g = colore.verde / totale;
  float b = colore.blu / totale;

  float distanza;

  int direzione =
    riconosciDirezione(r, g, b, distanza);

  Serial.print("Raw R:");
  Serial.print(colore.rosso);

  Serial.print(" G:");
  Serial.print(colore.verde);

  Serial.print(" B:");
  Serial.print(colore.blu);

  Serial.print(" Clear:");
  Serial.print(colore.clear);

  Serial.print(" | R:");
  Serial.print(r, 4);

  Serial.print(" G:");
  Serial.print(g, 4);

  Serial.print(" B:");
  Serial.print(b, 4);

  Serial.print(" | Distanza:");
  Serial.print(distanza, 4);

  if (direzione >= 0) {
    Serial.print(" | RICONOSCIUTO: ");
    Serial.println(direzioni[direzione].nome);

    // Ripete l’azione a ogni lettura riconosciuta
    eseguiDirezione(direzione);
  } else {
    Serial.println(" | NESSUNA DIREZIONE: STOP");

    eseguiStop();
  }

  delay(500);
}
