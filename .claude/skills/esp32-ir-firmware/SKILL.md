---
name: esp32-ir-firmware
description: Use ao criar, editar ou compilar sketches ESP32 do projeto (esp-code/), inclusive mudanças em pinos, IRremote, Serial ou servidor HTTP. Cobre convenções, pinos, a biblioteca IRremote 4.7.1 e a verificação por compilação.
---

# Firmware ESP32 (IRremote)

## Antes de editar

1. Leia `CLAUDE.md` (seções "Hardware e pinos" e "Aprendizados técnicos").
2. Pinos vêm do bloco ESP32 de `PinDefinitionsAndMore.h`, não do esquemático. Hoje: receptor `IR_RECEIVE_PIN = 27`, emissor `IR_SEND_PIN = 25`.
3. Cada sketch tem sua própria cópia de `PinDefinitionsAndMore.h`. Mudou um pino? Atualize todas as cópias.

## Regras obrigatórias para sinais IR

- `#define RECORD_GAP_MICROS 12000` antes de `#include <IRremote.hpp>` em qualquer sketch que capture sinais. Sem isso, o sinal do Samsung é cortado.
- Frames de captura passam pelo filtro `MINIMO_TIMINGS_SINAL` (10) e ignoram `IS_REPEAT`, `IS_AUTO_REPEAT` e `PARITY_FAILED`.
- Compensação: marcas −`MARK_EXCESS_MICROS`, espaços +`MARK_EXCESS_MICROS` (20 µs).
- Envio: `IrSender.sendRaw(vetor_uint16_t, tamanho, 38)`. O vetor começa com marca.
- `RAW_BUFFER_LENGTH` é 750; sinais do Samsung têm ~350 valores.

## Convenções do código

- Comentários, mensagens e identificadores em português, como no código original.
- No Serial dos sketches de experimento, imprima só o necessário para copiar o resultado. Não use `printIRResultShort` nem outros prints da biblioteca nesses sketches.
- Strings de HTML e JSON no WiFi usam `PROGMEM` / `FPSTR` quando são grandes.
- Credenciais Wi-Fi ficam como placeholders. Nunca escreva senhas reais.

## Verificação

Toda alteração em `.ino` ou `.h` deve compilar:

```bash
scripts/build-sketch.sh esp-code/<nome-do-sketch>
```

Resultado esperado: `[SUCCESS]`. Avisos `#pragma message` da biblioteca são normais. Um erro de compilação deve ser corrigido antes de terminar.

Não há hardware no ambiente. Diga explicitamente que a verificação foi só de compilação; não afirme que o sketch funciona com o ar condicionado.

## Armadilhas já encontradas

- Um `asd` solto após uma `}` de struct quebrou a compilação do transmissor. Ao colar arrays gerados, confira o fim de cada linha.
- Ao usar `sizeof(nome) / sizeof(nome[0])` na tabela de comandos, o nome precisa ser o array, não um ponteiro.
- `isalnum`/`isdigit` exigem `(unsigned char)` como argumento.
