#include <Arduino.h>

#define RAW_BUFFER_LENGTH  750
#define MARK_EXCESS_MICROS  20 // 20 é recomendado para os módulos VS1838
#define RECORD_GAP_MICROS 12000 // maior que a pausa de ~9ms entre as partes do controle do ar (padrão da biblioteca: 8000)

#include "PinDefinitionsAndMore.h" // Define IR_RECEIVE_PIN (27 no ESP32), entre outros
#include <IRremote.hpp>

#define MINIMO_TIMINGS_SINAL 10 // Frames com menos marcas/espaços que isso são ruído

uint16_t contadorSinais = 0;

void imprimeSinalComoArray();

void setup() {
    Serial.begin(115200);
    delay(500);

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.println(F("Receptor IR pronto. Aponte o controle para o receptor."));
}

void loop() {
    if (IrReceiver.decode()) {

        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            IrReceiver.resume();
            return;
        }
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
            IrReceiver.resume();
            return;
        }
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_PARITY_FAILED) {
            Serial.println(F("Erro de paridade, sinal ignorado."));
            IrReceiver.resume();
            return;
        }
        if (IrReceiver.decodedIRData.rawlen < MINIMO_TIMINGS_SINAL) {
            // Pulso de ruído isolado, não é um comando
            IrReceiver.resume();
            return;
        }

        contadorSinais++;
        Serial.println();
        Serial.print(F("Sinal #"));
        Serial.print(contadorSinais);
        Serial.print(F(": "));
        IrReceiver.printIRResultShort(&Serial);
        IrReceiver.printIRSendUsage(&Serial);
        Serial.println();

        imprimeSinalComoArray();

        IrReceiver.resume();
    }
}

/*
 * Imprime o último sinal recebido como array C (tempos em microssegundos),
 * com a mesma compensação usada na decodificação.
 * Exemplo de saída: const uint16_t sinal_1[67] = {9000, 4500, 560, ...};
 */
void imprimeSinalComoArray() {
    Serial.print(F("const uint16_t sinal_"));
    Serial.print(contadorSinais);
    Serial.print(F("["));
    Serial.print(IrReceiver.decodedIRData.rawlen - 1);
    Serial.print(F("] = {"));

    for (uint16_t i = 1; i < IrReceiver.decodedIRData.rawlen; i++) {
        int32_t duracao = (int32_t) IrReceiver.irparams.rawbuf[i] * MICROS_PER_TICK;

        // Marca (índice ímpar) encurta, espaço (índice par) alonga
        if (i & 1) {
            duracao -= MARK_EXCESS_MICROS;
        } else {
            duracao += MARK_EXCESS_MICROS;
        }

        Serial.print(duracao);
        if (i + 1 < IrReceiver.decodedIRData.rawlen) {
            Serial.print(F(", "));
        }
    }

    Serial.println(F("};"));
}
