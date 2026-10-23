#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "PinDefinitionsAndMore.h" // Define IR_SEND_PIN (25 no ESP32), entre outros
#include <IRremote.hpp>

#define FREQUENCIA_IR_KHZ  38

const char *WIFI_SSID  = "";
const char *WIFI_SENHA = "";

/*
 * ===== SINAIS CAPTURADOS PELO RECEPTOR =====
 */
const uint16_t sinal_0[347] = {2980, 8920, 530, 520, 480, 1470, 530, 520, 430, 570, 430, 570, 430, 520, 480, 520, 480, 520, 480, 520, 480, 1520, 480, 520, 480, 520, 480, 1470, 480, 520, 480, 520, 480, 1520, 480, 1520, 480, 1520, 430, 1570, 480, 1520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 470, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 1570, 430, 1570, 430, 1520, 480, 1520, 480, 3020, 2930, 9020, 430, 1520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 570, 430, 1570, 480, 520, 480, 520, 430, 1520, 480, 520, 480, 1520, 480, 1520, 480, 1520, 430, 1520, 480, 1520, 480, 1520, 480, 520, 480, 520, 430, 570, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 570, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 470, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 3020, 2980, 8970, 430, 1570, 430, 570, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 1470, 480, 520, 480, 520, 480, 1520, 480, 520, 480, 1520, 480, 1520, 480, 520, 480, 1520, 480, 1470, 480, 1520, 480, 1520, 480, 1520, 480, 1520, 480, 1470, 480, 1570, 430, 520, 480, 520, 480, 520, 480, 1520, 480, 1520, 480, 1520, 430, 520, 530, 470, 480, 520, 530, 470, 480, 520, 480, 520, 480, 520, 480, 520, 480, 1520, 480, 1520, 480, 520, 430, 1520, 480, 1520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 470, 530, 470, 530, 1470, 480, 1520, 480, 1520, 480, 1520, 480};
const uint16_t sinal_1[347] = {2980, 8970, 480, 520, 480, 1520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 1520, 480, 520, 480, 520, 480, 1520, 430, 1520, 480, 520, 480, 1570, 430, 1520, 480, 1520, 480, 1520, 480, 1520, 480, 470, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 570, 430, 570, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 1520, 480, 1470, 480, 3020, 2980, 8970, 480, 1520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 570, 430, 520, 480, 520, 480, 520, 480, 1520, 480, 520, 480, 520, 480, 1520, 430, 520, 480, 1520, 480, 1520, 480, 1520, 480, 1520, 430, 1520, 480, 1570, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 570, 430, 570, 480, 2970, 2980, 8970, 480, 1520, 480, 520, 480, 520, 480, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 1520, 480, 520, 480, 520, 430, 1570, 430, 1570, 480, 1470, 480, 1520, 480, 520, 480, 1520, 480, 1520, 480, 1470, 530, 1470, 530, 1470, 480, 1520, 480, 1520, 480, 1520, 480, 520, 480, 520, 430, 520, 480, 1520, 480, 1520, 480, 1520, 480, 520, 480, 520, 480, 470, 530, 470, 530, 520, 480, 520, 430, 520, 480, 520, 480, 1520, 480, 1520, 480, 520, 480, 1520, 430, 1520, 530, 470, 530, 470, 530, 520, 430, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 520, 480, 1520, 430, 1570, 480};

/*
 * ===== TABELA DE COMANDOS =====
 * Para cada sinal acima, adicione uma linha: COMANDO(sinal_N, "Rotulo do botao")
 * A posição na tabela é o id usado pela API.
 */
struct Comando {
    const uint16_t *sinal;
    uint16_t tamanho;
    const char *rotulo;
};

#define COMANDO(nome, rotulo) {nome, sizeof(nome) / sizeof(nome[0]), rotulo}

const Comando comandos[] = {
    COMANDO(sinal_0, "Ligar ar"),
    COMANDO(sinal_1, "Desligar ar"),
};

const uint8_t NUMERO_COMANDOS = sizeof(comandos) / sizeof(comandos[0]);

WebServer servidor(80);

const char PAGINA_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Controle IR</title>
<style>
body { font-family: sans-serif; text-align: center; margin: 0; padding: 16px; }
h1 { font-size: 1.5em; }
.botoes { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; max-width: 420px; margin: 0 auto; }
button { padding: 20px 8px; font-size: 1em; border: 0; border-radius: 10px; background: #1565c0; color: #fff; }
button:active { background: #0d47a1; }
#status { margin-top: 16px; min-height: 1.2em; }
</style>
</head>
<body>
<h1>Controle IR</h1>
<div class="botoes">
%BOTOES%
</div>
<p id="status"></p>
<script>
function enviar(id) {
  const status = document.getElementById('status');
  status.textContent = 'Enviando...';
  fetch('/api/comando?id=' + id)
    .then(r => r.json())
    .then(d => status.textContent = d.ok ? 'Enviado: ' + d.rotulo : 'Erro: ' + d.erro)
    .catch(() => status.textContent = 'Falha de conexao');
}
</script>
</body>
</html>
)rawliteral";

void tratarPagina() {
    String botoes;
    for (uint8_t i = 0; i < NUMERO_COMANDOS; i++) {
        botoes += "<button onclick=\"enviar(" + String(i) + ")\">" + comandos[i].rotulo + "</button>\n";
    }

    String pagina = FPSTR(PAGINA_HTML);
    pagina.replace("%BOTOES%", botoes);
    servidor.send(200, "text/html", pagina);
}

void tratarComando() {
    // Permite que a página seja servida de outro endereço (outra origem)
    servidor.sendHeader("Access-Control-Allow-Origin", "*");

    if (!servidor.hasArg("id")) {
        servidor.send(400, "application/json", "{\"ok\":false,\"erro\":\"parametro id ausente\"}");
        return;
    }

    int id = servidor.arg("id").toInt();
    if (id < 0 || id >= NUMERO_COMANDOS) {
        servidor.send(404, "application/json", "{\"ok\":false,\"erro\":\"comando nao cadastrado\"}");
        return;
    }

    Serial.print(F("Enviando comando "));
    Serial.println(id);
    IrSender.sendRaw(comandos[id].sinal, comandos[id].tamanho, FREQUENCIA_IR_KHZ);

    String resposta = "{\"ok\":true,\"rotulo\":\"" + String(comandos[id].rotulo) + "\"}";
    servidor.send(200, "application/json", resposta);
}

void tratarListaComandos() {
    servidor.sendHeader("Access-Control-Allow-Origin", "*");

    String resposta = "[";
    for (uint8_t i = 0; i < NUMERO_COMANDOS; i++) {
        if (i > 0) resposta += ",";
        resposta += "{\"id\":" + String(i) + ",\"rotulo\":\"" + comandos[i].rotulo + "\"}";
    }
    resposta += "]";
    servidor.send(200, "application/json", resposta);
}

void tratarNaoEncontrado() {
    servidor.send(404, "text/plain", "Pagina nao encontrada");
}

void setup() {
    Serial.begin(115200);
    delay(500);

    IrSender.begin(IR_SEND_PIN);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_SENHA);
    Serial.print(F("Conectando ao Wi-Fi"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print('.');
    }
    Serial.println();
    Serial.print(F("Conectado. Abra no navegador: http://"));
    Serial.println(WiFi.localIP());

    servidor.on("/", tratarPagina);
    servidor.on("/api/comando", tratarComando);
    servidor.on("/api/comandos", tratarListaComandos);
    servidor.onNotFound(tratarNaoEncontrado);
    servidor.begin();
}

void loop() {
    servidor.handleClient();
}
