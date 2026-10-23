# remote-control

Engenharia reversa do controle remoto de um ar condicionado Samsung usando um ESP32 com LED emissor IR, módulo receptor IR e LED de status.

Para o contexto completo do projeto (hardware, pinos, decisões e pendências), veja [CLAUDE.md](CLAUDE.md).

## Requisitos

- ESP32 com o core Arduino (`espressif32` no PlatformIO)
- Biblioteca [IRremote 4.7.1](https://github.com/Arduino-IRremote/Arduino-IRremote) instalada em `~/Arduino/libraries/IRremote`
- PlatformIO (`~/.platformio/penv/bin/platformio`), usado por `scripts/build-sketch.sh`

## Como usar

### 1. Capturar os sinais do controle

Grave o sketch `esp-code/Q0962-Experimento` no ESP32 e abra o Serial Monitor a 115200 baud.

1. Digite o nome do botão (ex.: `ligar 24`) e pressione Enter.
2. Aperte o botão no controle apontado para o receptor.
3. Copie a linha `const uint16_t ...` impressa e salve em `captures/samsung-ac/`.
4. Registre o cenário em `captures/SCENARIOS.md`.

### 2. Enviar os sinais

Cole os arrays no `esp-code/Q0962-WiFi/Q0962-WiFi.ino`, adicione `COMANDO(nome, "Rótulo")` na tabela `comandos[]`, configure `WIFI_SSID` e `WIFI_SENHA` e grave o sketch.

Com o ESP32 na rede, há duas formas de controlar:

- Abra `http://<IP do ESP32>/` no navegador. A página é servida pelo próprio ESP32.
- Abra `esp-code/Q0962-WiFi/index.html` (pode ser servido por outro servidor na porta 80), informe o endereço do ESP32 e clique em **Conectar**.

API:

- `GET /api/comandos` — lista os comandos em JSON
- `GET /api/comando?id=N` — envia o comando de índice `N`

### 3. Compilar um sketch

```bash
scripts/build-sketch.sh esp-code/Q0962-Experimento
```

O script copia o sketch para uma pasta temporária, gera o `platformio.ini` e compila para ESP32.

## Estrutura

Veja a seção "Estrutura do repositório" em [CLAUDE.md](CLAUDE.md). Os recursos para desenvolvimento assistido por IA ficam em `.claude/` (skills e agentes).
