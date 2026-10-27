# CLAUDE.md — Engenharia reversa do controle do ar condicionado Samsung

## Objetivo

Engenharia reversa do controle remoto de um ar condicionado Samsung usando um dispositivo IoT com ESP32.
O dispositivo:

- **Recebe** os sinais IR do controle original (módulo receptor IR, tipo VS1838) e os registra como vetores de tempos.
- **Replica** esses sinais (LED emissor IR) para controlar o ar condicionado a partir de uma página web na rede local.
- Tem um **LED de status**. As sketches atuais ainda não o usam; o sketch original usava `pinLED` (GPIO 26) e `STATUS_PIN` (GPIO 2).

Base: projeto Q0962 do canal BrincandoComIdeias (originais fora deste repositório, em `../Q0962-main` e `../Q0962-Sketch`). A biblioteca é o **IRremote 4.7.1**, instalada em `~/Arduino/libraries/IRremote`.

## Hardware e pinos

- **Pinos vêm do código, não do esquemático.** O esquemático (`Q0962_Esquematico.png`) mostra D15/D4, mas o usuário pediu explicitamente que os pinos do código fossem usados.
- Em `PinDefinitionsAndMore.h` (bloco ESP32): `IR_RECEIVE_PIN = 27`, `IR_SEND_PIN = 25`.
- Cada sketch tem uma cópia de `PinDefinitionsAndMore.h`. Se mudar pino, altere em todas.

## Estrutura do repositório

```
remote-control/
├── CLAUDE.md              # este arquivo: contexto do projeto
├── README.md              # documentação para humanos
├── .claude/
│   ├── skills/            # procedimentos reutilizáveis (firmware, experimentos, análise)
│   └── agents/            # agentes especializados (firmware, análise de sinais)
├── esp-code/              # sketches do ESP32
│   ├── Q0962-Receptor/    # despeja o vetor bruto de cada sinal recebido
│   ├── Q0962-Transmissor/ # envia comandos da tabela COMANDO(...), via Serial (0-9)
│   ├── Q0962-Experimento/ # captura nomeada para os experimentos (Serial limpo)
│   └── Q0962-WiFi/        # servidor HTTP na porta 80 + API; index.html standalone
├── captures/              # dados dos experimentos (vetores capturados)
│   ├── SCENARIOS.md       # cenários a capturar e status de cada um
│   └── samsung-ac/        # saídas copiadas do Serial do Q0962-Experimento
└── scripts/
    └── build-sketch.sh    # compila um sketch com PlatformIO em pasta temporária
```

## Sketches e fluxo de dados

1. **Q0962-Experimento** (principal para a pesquisa): você digita o nome do botão (ex.: `ligar 24`) + Enter, aperta o botão no controle, e o Serial imprime:
   ```
   nome: ligar_24 | indice: 1
   const uint16_t ligar_24[347] = {9000, 4500, ...};
   ```
   Espaços e `-` viram `_`; acentos são removidos; nomes que começam com número ganham `b_`.
2. Copie o array para `captures/samsung-ac/` e registre o cenário em `captures/SCENARIOS.md`.
3. Para usar no sistema, cole o array no **Q0962-WiFi** (ou no Transmissor) e adicione `COMANDO(nome, "Rótulo")` na tabela `comandos[]`.
4. **Q0962-WiFi**: conecta no Wi-Fi (`WIFI_SSID`/`WIFI_SENHA` são placeholders), serve `/`, `GET /api/comandos` (lista JSON) e `GET /api/comando?id=N` (envia o sinal). Envia `Access-Control-Allow-Origin: *`. O `index.html` é a página standalone que chama essa API.
5. **Q0962-Receptor** e **Q0962-Transmissor** são as versões anteriores, mantidas como referência.

## Aprendizados técnicos (não óbvios)

- **`RECORD_GAP_MICROS 12000`** deve ser definido antes do `#include <IRremote.hpp>`. O padrão da biblioteca é 8000 µs, e o controle do ar pausa ~9 ms entre as partes do sinal; sem isso, a captura é cortada (foi o bug que gerou um vetor de 115 valores incompleto).
- **Ruído:** com a janela de 12000 µs, pulsos isolados viram frames curtos (ex.: `{580}`). O filtro `MINIMO_TIMINGS_SINAL = 10` os descarta.
- **Repetições:** ao segurar um botão, chegam frames com `IRDATA_FLAGS_IS_REPEAT` / `IS_AUTO_REPEAT`; são ignorados.
- **Compensação:** marcas são reduzidas e espaços aumentados em `MARK_EXCESS_MICROS = 20`, como na biblioteca.
- **Formato de envio:** `IrSender.sendRaw(const uint16_t[], len, 38)` recebe tempos em microssegundos, alternando marca/espaço começando por marca.
- O sketch original forçava `protocol = UNKNOWN` (sempre raw). Os sketches novos também enviam tudo como raw.
- Os sinais do Samsung têm vários frames (partes separadas por pausas longas) e vetores de ~350 valores.

## Convenções

- **Idioma:** comentários, mensagens do Serial e identificadores em português, como no código original. Respostas ao usuário em português.
- **Serial:** mensagens curtas e sem ruído da biblioteca no sketch de experimento; a saída precisa ser fácil de copiar.
- **Compilação:** use `scripts/build-sketch.sh esp-code/<sketch>`. Ele usa PlatformIO (`~/.platformio/penv/bin/platformio`) com ESP32 `espressif32`. A IA não tem acesso ao hardware: a validação feita por ela é só de compilação. O usuário tem o dispositivo físico e faz os testes no ESP32 e no ar-condicionado; os resultados dele são a validação de funcionamento.
- **Commits:** Não commite sem pedido. Quando commitar, use a skill `padroes-de-commits` (formato `:emoji: tipo(escopo): descrição`, tipos e emojis) e a linha de atribuição indicada no ambiente.

## Estado atual e pendências

- Nenhum sketch foi testado no hardware ainda. O Experimento e o WiFi compilam com sucesso. Os testes no dispositivo físico são feitos pelo usuário.
- O vetor `sinal_0` do transmissor foi capturado com o bug do gap (incompleto); precisa ser recapturado com o Experimento.
- Credenciais Wi-Fi são placeholders em `Q0962-WiFi.ino`; não colocar senhas reais no repositório.
- Cenários a capturar: ligar em 24, ligar em 21, desligar em 27, aumentar velocidade, reduzir temperatura para 23 (lista completa em `captures/SCENARIOS.md`).
- Falta decidir se o LED de status volta a ser usado e em qual pino (o código atual não usa).
- Ainda não há hipótese decodificada do protocolo Samsung; a análise está nas skills `ir-signal-analysis` e no agente `ir-signal-analyst`.
