/*
     CÓDIGO:	  Q0962 - EXPERIMENTO
     BASEADO EM: Q0962-Receptor
     SKETCH:    Captura de comandos para experimento
     FUNÇÃO:    1) Digite o nome do botão e pressione Enter.
                2) Aperte o botão no controle.
                O Serial mostra o nome, o índice da captura e o array para replicação.
*/

#include <Arduino.h>

#define RAW_BUFFER_LENGTH  750
#define MARK_EXCESS_MICROS  20 // 20 é recomendado para os módulos VS1838
#define RECORD_GAP_MICROS 12000 // maior que a pausa de ~9ms entre as partes do controle do ar (padrão da biblioteca: 8000)

#include "PinDefinitionsAndMore.h" // Define IR_RECEIVE_PIN (27 no ESP32), entre outros
#include <IRremote.hpp>

#define MINIMO_TIMINGS_SINAL 10 // Frames com menos marcas/espaços que isso são ruído
#define TAMANHO_MAXIMO_NOME  32

String nomeBotao = ""; // Nome digitado para a próxima captura
uint16_t indice = 0;

String sanitizaNome(const String &bruto);
bool sinalValido();
void imprimeSinalComoArray();

void setup() {
    Serial.begin(115200);
    delay(500);

    IrReceiver.begin(IR_RECEIVE_PIN);

    Serial.println(F("Digite o nome do botao e pressione Enter."));
}

void loop() {
    if (Serial.available()) {
        String linha = Serial.readStringUntil('\n');
        linha.trim();

        if (linha.length() > 0) {
            nomeBotao = sanitizaNome(linha);
            Serial.print(F("Aperte o botao: "));
            Serial.println(nomeBotao);
        }
    }

    if (IrReceiver.decode()) {
        if (sinalValido()) {
            indice++;
            imprimeSinalComoArray();
            nomeBotao = "";
            Serial.println(F("Digite o proximo nome do botao."));
        }
        IrReceiver.resume();
    }
}

/*
 * Aceita apenas o sinal de um botão pressionado com nome definido.
 * Ignora repetições, erros de paridade, ruído e sinais sem nome.
 */
bool sinalValido() {
    if (nomeBotao.length() == 0) {
        return false;
    }
    if (IrReceiver.decodedIRData.flags & (IRDATA_FLAGS_IS_REPEAT | IRDATA_FLAGS_IS_AUTO_REPEAT | IRDATA_FLAGS_PARITY_FAILED)) {
        return false;
    }
    return IrReceiver.decodedIRData.rawlen >= MINIMO_TIMINGS_SINAL;
}

/*
 * Deixa o nome seguro para virar identificador C: letras, números e "_".
 * Espaços e "-" viram "_"; os demais caracteres (inclusive acentos) são removidos.
 */
String sanitizaNome(const String &bruto) {
    String limpo = "";
    for (uint16_t i = 0; i < bruto.length() && limpo.length() < TAMANHO_MAXIMO_NOME; i++) {
        char c = bruto.charAt(i);
        if (isalnum((unsigned char) c)) {
            limpo += c;
        } else if (c == ' ' || c == '-') {
            limpo += '_';
        }
    }
    if (limpo.length() > 0 && isdigit((unsigned char) limpo.charAt(0))) {
        limpo = "b_" + limpo;
    }
    return limpo;
}

/*
 * Imprime o último sinal capturado no formato:
 * nome: ligar_24 | indice: 1
 * const uint16_t ligar_24[67] = {9000, 4500, ...};
 */
void imprimeSinalComoArray() {
    Serial.print(F("nome: "));
    Serial.print(nomeBotao);
    Serial.print(F(" | indice: "));
    Serial.println(indice);

    Serial.print(F("const uint16_t "));
    Serial.print(nomeBotao);
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
