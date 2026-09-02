#include <Wire.h>

#define GY33_ADDRESS 0x5A
#define NUMERO_COMANDI 10
#define NUMERO_LETTURE 10

struct RGB {
  uint16_t rosso;
  uint16_t verde;
  uint16_t blu;
  uint16_t clear;
};

struct Calibrazione {
  const char *nome;
  float r;
  float g;
  float b;
  uint32_t clear;
};

Calibrazione impostazioni[NUMERO_COMANDI] = {
  {"DESTRA",         0, 0, 0, 0},
  {"SINISTRA",       0, 0, 0, 0},
  {"INDIETRO",       0, 0, 0, 0},
  {"AVANTI",         0, 0, 0, 0},
  {"STOP",           0, 0, 0, 0},
  {"DOCK",           0, 0, 0, 0},
  {"MOTORI PULIZIA", 0, 0, 0, 0},
  {"CLEAN",          0, 0, 0, 0},
  {"SPOT",           0, 0, 0, 0},
  {"STOP PULIZIA",   0, 0, 0, 0}
};

bool leggiGY33(RGB &colore) {
  uint8_t dati[8];

  Wire.beginTransmission(GY33_ADDRESS);
  Wire.write(0x00);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  uint8_t ricevuti =
    Wire.requestFrom((uint8_t)GY33_ADDRESS, (uint8_t)8);

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

void contoAllaRovescia(int secondi) {
  for (int i = secondi; i > 0; i--) {
    Serial.print("Inizio registrazione tra ");
    Serial.print(i);
    Serial.println(" secondi...");
    delay(1000);
  }
}

void registraColore(int indice) {
  Serial.println();
  Serial.println("========================================");
  Serial.print("INVIA ADESSO IL COMANDO: ");
  Serial.println(impostazioni[indice].nome);
  Serial.println("Posiziona il sensore sul quadrato.");
  Serial.println("========================================");

  // Dieci secondi per inviare il comando
  // e posizionare il sensore
  contoAllaRovescia(10);

  Serial.println();
  Serial.println("INIZIO DELLE 10 LETTURE");
  Serial.println("Una lettura ogni secondo.");
  Serial.println();

  uint64_t sommaRosso = 0;
  uint64_t sommaVerde = 0;
  uint64_t sommaBlu = 0;
  uint64_t sommaClear = 0;

  int lettureValide = 0;

  // Continua fino a ottenere esattamente 10 letture valide
  while (lettureValide < NUMERO_LETTURE) {
    RGB colore;

    if (leggiGY33(colore)) {
      float totale =
        (float)colore.rosso +
        colore.verde +
        colore.blu;

      if (totale > 0) {
        float percentualeR = colore.rosso / totale;
        float percentualeG = colore.verde / totale;
        float percentualeB = colore.blu / totale;

        sommaRosso += colore.rosso;
        sommaVerde += colore.verde;
        sommaBlu += colore.blu;
        sommaClear += colore.clear;

        lettureValide++;

        Serial.print(impostazioni[indice].nome);
        Serial.print(" - Lettura ");
        Serial.print(lettureValide);
        Serial.print("/10");

        Serial.print(" | Raw R:");
        Serial.print(colore.rosso);

        Serial.print(" G:");
        Serial.print(colore.verde);

        Serial.print(" B:");
        Serial.print(colore.blu);

        Serial.print(" Clear:");
        Serial.print(colore.clear);

        Serial.print(" | R:");
        Serial.print(percentualeR, 4);

        Serial.print(" G:");
        Serial.print(percentualeG, 4);

        Serial.print(" B:");
        Serial.println(percentualeB, 4);
      }
    } else {
      Serial.println("Errore I2C: lettura non riuscita, riprovo.");
    }

    delay(1000);
  }

  float mediaRosso =
    (float)sommaRosso / NUMERO_LETTURE;

  float mediaVerde =
    (float)sommaVerde / NUMERO_LETTURE;

  float mediaBlu =
    (float)sommaBlu / NUMERO_LETTURE;

  float mediaClear =
    (float)sommaClear / NUMERO_LETTURE;

  float totaleMedio =
    mediaRosso + mediaVerde + mediaBlu;

  impostazioni[indice].r =
    mediaRosso / totaleMedio;

  impostazioni[indice].g =
    mediaVerde / totaleMedio;

  impostazioni[indice].b =
    mediaBlu / totaleMedio;

  impostazioni[indice].clear =
    (uint32_t)mediaClear;

  Serial.println();
  Serial.print("MEDIA DEL COMANDO ");
  Serial.println(impostazioni[indice].nome);

  Serial.print("R:");
  Serial.print(impostazioni[indice].r, 4);

  Serial.print(" G:");
  Serial.print(impostazioni[indice].g, 4);

  Serial.print(" B:");
  Serial.print(impostazioni[indice].b, 4);

  Serial.print(" Clear:");
  Serial.println(impostazioni[indice].clear);

  Serial.println("----------------------------------------");
}

void stampaRisultati() {
  Serial.println();
  Serial.println();
  Serial.println("******** RISULTATI CALIBRAZIONE ********");
  Serial.println("Copia tutto il blocco seguente:");
  Serial.println();

  Serial.println("ComandoColore comandi[] = {");

  for (int i = 0; i < NUMERO_COMANDI; i++) {
    Serial.print("  {\"");
    Serial.print(impostazioni[i].nome);
    Serial.print("\", ");

    Serial.print(impostazioni[i].r, 4);
    Serial.print(", ");

    Serial.print(impostazioni[i].g, 4);
    Serial.print(", ");

    Serial.print(impostazioni[i].b, 4);
    Serial.print("}");

    if (i < NUMERO_COMANDI - 1) {
      Serial.print(",");
    }

    Serial.println();
  }

  Serial.println("};");

  Serial.println();
  Serial.println("Valori Clear medi:");

  for (int i = 0; i < NUMERO_COMANDI; i++) {
    Serial.print(impostazioni[i].nome);
    Serial.print(": ");
    Serial.println(impostazioni[i].clear);
  }

  Serial.println();
  Serial.println("CALIBRAZIONE TERMINATA.");
  Serial.println("Il programma adesso è fermo.");
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  // DR/SDA → GPIO4
  // CT/SCL → GPIO5
  Wire.begin(4, 5);
  Wire.setClock(100000);

  Serial.println();
  Serial.println("CALIBRAZIONE COMANDI WALKEREMOTE");
  Serial.println("Per ogni comando:");
  Serial.println("- 10 secondi di preparazione");
  Serial.println("- 10 letture, una ogni secondo");
  Serial.println();

  delay(3000);

  for (int i = 0; i < NUMERO_COMANDI; i++) {
    registraColore(i);
  }

  stampaRisultati();

  // Ferma definitivamente il programma
  while (true) {
    delay(1000);
  }
}

void loop() {
  // La calibrazione viene eseguita una volta nel setup.
}
