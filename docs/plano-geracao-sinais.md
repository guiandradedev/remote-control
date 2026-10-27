# Plano: decodificação e geração de sinais Samsung

Objetivo: gerar o sinal IR equivalente a qualquer estado do ar-condicionado (ligado/desligado, modo, temperatura, ventilador), em vez de depender de botões capturados. A proposta final está em `CLAUDE.md`, seção "Proposta final do aplicativo".

## Como ler este plano

- **Responsável:** `IA` = trabalho feito pelo Claude (análise, scripts, código, documentação). `Usuário` = captura, teste no hardware e decisões.
- **Status:** `pendente`, `em andamento` ou `concluído`. Atualize ao terminar cada subtask.
- **Critério de conclusão:** o que precisa ser verdade para a subtask ser marcada como concluída.

## Decisões em aberto

Estas decisões bloqueiam subtasks específicas. Registre a resposta aqui quando for tomada.

| # | Decisão | Bloqueia | Resposta |
|---|---------|----------|----------|
| D1 | Qual espaço representa 0 e qual representa 1 (espaço curto ou longo)? | 1.2 | **Adotado: espaço curto (~500 µs) = 0, espaço longo (~1500 µs) = 1, bits LSB primeiro em cada byte.** Hipótese de trabalho, consistente com todas as capturas. Confirmar com o usuário. |
| D2 | Onde o aplicativo roda: ESP32, computador ou celular? | 3.5, 5.1 | pendente |
| D3 | Quais temperaturas e modos devem ser suportados na v1? | 2.3, 3.1 | pendente |

## Fase 0: fechar a base de dados

Dependência: nenhuma. Status: `em andamento`.

| Subtask | Responsável | Descrição | Critério de conclusão |
|---------|-------------|-----------|-----------------------|
| 0.1 | Usuário | Recapturar `ligar_inicial_cool` duas vezes, no mesmo estado inicial. | Duas capturas novas em `captures/samsung-ac/`. |
| 0.2 | IA | Comparar `ligar_2_cool` com `ligar_inicial_cool` (mesmo estado inicial) e registrar as diferenças. | Relatório com número de valores e posições que mudam. Vetor estável confirmado ou falha registrada. |
| 0.3 | Usuário | Refazer `decrease_23_to_22_cool`, que está vazio. | Arquivo com 347 valores ou próximo disso. |
| 0.4 | Usuário | Capturar um par que muda só a temperatura em cool (ex.: 24 → 23 e 23 → 24). | Dois arquivos com estado inicial anotado. |
| 0.5 | Usuário | Capturar cool, heat, auto e fan com temperatura e ventilador fixos, para isolar o campo de modo. | Uma captura por modo, com a temperatura e o ventilador anotados. |
| 0.6 | IA + Usuário | Verificar os vetores curtos (`increase_24_to_25_heat` com 117, `fan_decrease_2_heat_23` com 119, `increase_22_to_23_cool` com 163). Marcar como válidos ou recapturar. | Cada vetor curto com status definido em `SCENARIOS.md`. |
| 0.7 | IA | Atualizar `captures/SCENARIOS.md` com as novas capturas e o estado inicial de cada uma. | Todas as capturas listadas com estado inicial e status. |

### Resultados (Fase 0)

- **0.2 concluído.** `ligar_2_cool` e `ligar_inicial_cool` têm 347 valores cada. Há 35 posições com tempo diferente (variação de temporização), mas os bits das 3 seções são idênticos. Vetor estável em bits.
- **0.6 concluído.** Os vetores curtos estão cortados na seção 2 (sem os 56 bits). Marcados como **inválidos** em `captures/SCENARIOS.md`: `increase_22_to_23_cool`, `increase_24_to_25_heat`, `fan_decrease_2_heat_23`, `decrease_24_to_23_heat`, `fan_decrease_1_heat_23`, `fan_decrease_2_cool`. Recaptura pendente (usuário).
- **0.7 parcial.** `SCENARIOS.md` tem o status de validade. O estado inicial de cada captura ainda não está anotado; depende de 0.1, 0.4 e 0.5.
- **0.1, 0.3, 0.4, 0.5 pendentes** (captura do usuário).

## Fase 1: estrutura dos frames

Dependência: Fase 0 (pelo menos 0.2 e 0.4). Status: `em andamento` (1.1 a 1.6 feitos sobre as capturas existentes; 0.4 e 0.5 ainda pendentes, então os pares de comparação são os já disponíveis).

| Subtask | Responsável | Descrição | Critério de conclusão |
|---------|-------------|-----------|-----------------------|
| 1.1 | IA | Script `scripts/parse_capturas.py` (executado com `python3 -I`). Lê `captures/samsung-ac/*.txt`, separa as partes pelas pausas longas (> 2000 µs) e imprime nº de partes e nº de valores de cada parte. | Script roda sem erro em todas as capturas válidas. |
| 1.2 | IA (depende de D1) | Converter cada par marca/espaço em bit, pela regra definida em D1. | Sequência de bits para cada parte de cada captura. |
| 1.3 | IA | Agrupar os bits em bytes e documentar cabeçalhos e constantes de cada frame. | Tabela de bytes por frame para cada captura. |
| 1.4 | IA | Comparar bit a bit os pares da 0.4 e da 0.5. Listar os bits que mudam e a qual parâmetro cada par pertence. | Lista de campos candidatos: temperatura, modo, ventilador. |
| 1.5 | IA | Testar somas de verificação comuns (soma mod 256, XOR, complemento) sobre os bytes de cada frame. | Algoritmo que bate em todas as capturas válidas, ou registro de que nenhum bate. |
| 1.6 | IA | Revisar os resultados com o agente `ir-signal-analyst` e registrar as hipóteses como hipótese, não como fato. | Seção de resultados desta Fase preenchida. |

### Resultados (Fase 1)

Ferramentas: `scripts/parse_capturas.py` (1.1 e 1.2, com `--bits` para a tabela de bytes) e `scripts/analise_samsung.py` (0.2, 1.4 e 1.5).

**Estrutura (1.1, observado).**
- Cada seção começa com uma marca longa (~3000 µs) seguida de espaço longo (~9000 µs).
- Cada seção tem 56 bits (2 valores por bit: marca de ~500 µs e espaço) e termina com uma marca.
- Seções são separadas por um espaço longo (~3000 µs).
- Capturas de 231 valores têm 2 seções (116 + 115 valores). Capturas de 347 valores têm 3 seções (116 + 116 + 115).
- Vetores com seção incompleta (menos de 56 bits) aparecem nas capturas cortadas (ver 0.6). O parser não filtra ruído; o `MINIMO_TIMINGS_SINAL` do sketch já faz isso na captura.

**Bits (1.2, sob a hipótese D1).** Espaço curto = 0, espaço longo = 1, LSB primeiro. Um bit ausente no fim (vetor cortado) é ignorado pelo conversor de bytes.

**Bytes (1.3).** Exemplo com as três seções de `ligar_inicial_cool` (estado ligado, 22 °C, cool):

```
secao 1: 02 92 0F 00 00 00 F0
secao 2: 01 D2 0F 00 00 00 00
secao 3: 01 C2 FE 71 60 19 F0
```

A tabela completa das 49 seções completas está na saída de `python3 -I scripts/analise_samsung.py`.

**Comparação (1.4, parcial).** Sem os pares 0.4 e 0.5, a comparação usou os pares já capturados. Candidatos, todos como **hipótese**:

- **Temperatura (forte):** nibble alto do byte 4 da seção de estado = temperatura − 16. Valores observados: `0x50` = 21 °C, `0x60` = 22 °C, `0x70` = 23 °C, `0x80` = 24 °C, `0x90` = 25 °C, `0xA0` = 26 °C. Vale em cool e heat e bate com os nomes dos arquivos.
- **Ligado/desligado (hipótese):** bit `0x20` do byte 1 da seção 1. `92` em ligar, `B2` em desligar. O byte 6 da seção 1 também muda (`F0` ↔ `C0`).
- **Modo (hipótese, sem confirmação):** byte 5 da seção de estado. Valores `19` (cool), `0D` (auto), `41` (heat). Não está claro qual nibble é o modo.
- **Ventilador (hipótese, sem confirmação):** nibble baixo do byte 5. Em cool: `19` → `1B` → `1F` com um e dois apertos. Em heat: `41` → `45` → `49`. Os passos não são iguais entre os modos, então o mapeamento ainda não está fechado.
- **Aleta/swing (hipótese):** byte 2 da seção de estado, `FE` ↔ `AE` (`fan_move_cool`).
- **Não explicado:** nibble alto do byte 1 da seção de estado varia (`A2` a `E2`) sem correspondência clara com os parâmetros acima.

Os campos confirmados por par de comparação ainda dependem de 0.4 e 0.5.

**Somas de verificação (1.5, negativo).** Testados sobre o 7º byte de cada seção completa (49 seções): soma mod 256, complemento, XOR, complemento do XOR, soma de nibbles, soma + constante, soma ± constante e somas com exclusão de bytes. O melhor candidato acerta 25 de 49, o que é o nível do acaso. **Nenhum checksum simples foi encontrado.** Consequência: o gerador não pode calcular o último byte pela regra conhecida; precisa de outra hipótese (por exemplo, uma tabela aprendida das capturas) ou de mais dados.

**Revisão (1.6).** A análise foi feita diretamente, seguindo o procedimento da skill `ir-signal-analysis` e as regras do agente `ir-signal-analyst`. O agente não foi executado como sub-agente nesta rodada. Hipóteses acima ficam como hipótese.

## Fase 2: tabela de estados

Dependência: Fase 1 (1.4 e 1.5). Status: `em andamento` (2.1 e parte de 2.2 feitos; 2.3 bloqueado por D3).

| Subtask | Responsável | Descrição | Critério de conclusão |
|---------|-------------|-----------|-----------------------|
| 2.1 | IA | Criar `captures/estados.csv` com as colunas: `arquivo`, `ligado`, `modo`, `temperatura`, `ventilador`, `valido`. | Uma linha por captura válida. |
| 2.2 | IA | Para cada parâmetro confirmado na 1.4, adicionar a coluna e a faixa de bits que ele ocupa. | Coluna e faixa de bits documentadas para cada parâmetro confirmado. |
| 2.3 | IA (depende de D3) | Listar os parâmetros ainda não decodificados e o valor de referência de cada um, que fica fixo no gerador. | Lista de campos fixos com valor de referência. |

### Resultados (Fase 2)

**2.1 concluído.** `captures/estados.csv` tem 26 linhas: 20 válidas e 6 inválidas (as cortadas da 0.6). Colunas `ligado` e `temperatura` vêm da decodificação; `modo` e `ventilador` ficam vazios até serem decodificados. Em `valido = nao`, os campos de estado ficam vazios.

**2.2 parcial.**

| Parâmetro | Seção | Byte / bits | Status |
|-----------|-------|-------------|--------|
| Temperatura | seção de estado (a que começa com `01` e tem `71` no byte 3) | byte 4, nibble alto (`+16` para °C) | Hipótese forte. Consistente em 21 a 26 °C. Confirmar com 0.4. |
| Ligado | seção 1 | byte 1, bit `0x20` (0 = ligado) | Hipótese. Confirmar com 0.1 ou 0.4. |
| Modo | seção de estado | byte 5 (a definir) | Não decodificado. |
| Ventilador | seção de estado | byte 5, nibble baixo (a definir) | Não decodificado. |

**2.3 bloqueado por D3.** Lista prévia dos campos que parecem fixos nas capturas (valores de referência em cool e heat, com `byte 1` alto e `byte 6` ainda variáveis):

- Seção 1: `02`, `0F`, `00`, `00`, `00`.
- Seção 2 (quando há 3 seções): `01`, `D2`, `0F`, `00`, `00`, `00`, `00`.
- Seção de estado: `01`, `FE` (ou `AE` com a aleta), `71`.

A lista final depende de D3 (temperaturas e modos da v1).

## Fase 3: gerador v1

Dependência: Fase 2 e D3. Status: `pendente`. Não iniciada: a Fase 2 não fechou modo nem ventilador, e D3 não foi decidida.

| Subtask | Responsável | Descrição | Critério de conclusão |
|---------|-------------|-----------|-----------------------|
| 3.1 | IA | Definir o template do vetor: frames, constantes, campos variáveis e checksum. | Template documentado em `docs/`. |
| 3.2 | IA | Implementar `gerar(estado) -> vetor` em `scripts/gerador.py`. Os valores mapeados vêm de tabela de conversão (valor → bits). Os campos fixos vêm da captura de referência. | Função executa para os estados da tabela. |
| 3.3 | IA | Validação offline: gerar o vetor para cada captura com estado conhecido e comparar bit a bit com o original. | 100% dos vetores iguais ao original, ou lista de diferenças explicada. |
| 3.4 | IA | Tratar entradas fora da tabela (temperatura ou modo não mapeado): erro explícito, sem gerar um vetor silenciosamente errado. | Teste com entradas inválidas que falha de forma explícita. |
| 3.5 | IA (depende de D2) | Se o gerador for para o ESP32, portar para C++ no sketch. Se não, manter em Python ou JavaScript. | Decisão registrada e implementação compilando (quando for C++). |

## Fase 4: validação no hardware

Dependência: Fase 3. Status: `pendente`. Os testes são feitos pelo usuário, conforme `CLAUDE.md`.

| Subtask | Responsável | Descrição | Critério de conclusão |
|---------|-------------|-----------|-----------------------|
| 4.1 | Usuário | Escolher de 3 a 5 estados que **não** foram capturados (ex.: cool 20 °C, heat 22 °C, ventilador em outro nível). | Lista de estados de teste. |
| 4.2 | IA | Gerar os vetores desses estados e preparar o código para o ESP32 ou o endpoint de teste. | Código compilando com `scripts/build-sketch.sh`. |
| 4.3 | Usuário | Enviar cada estado ao ar-condicionado e registrar a resposta na tabela de testes abaixo. | Tabela preenchida para todos os estados. |
| 4.4 | Usuário | Testar uma sequência: ligar → mudar temperatura → mudar modo → desligar. | Resultado de cada passo registrado. |
| 4.5 | IA | Se houver efeito colateral (ventilador, modo ou aleta mudando sem pedido), investigar o campo fixo responsável e ajustar. | Efeito colateral explicado ou corrigido. |

Tabela de testes (preenchida pelo usuário):

| Estado pedido | Resposta do ar-condicionado | Aceito? | Observações |
|---------------|-----------------------------|---------|-------------|
| | | | |

## Fase 5: aplicativo

Dependência: Fase 4 concluída e D2 decidida. Status: `pendente`.

| Subtask | Responsável | Descrição | Critério de conclusão |
|---------|-------------|-----------|-----------------------|
| 5.1 | IA + Usuário | Confirmar D2 e escrever a arquitetura: onde roda o gerador, como o estado chega até ele e como o sinal chega ao ESP32. | Documento de arquitetura em `docs/`. |
| 5.2 | IA | Definir o contrato da API: entrada (estado em JSON) e saída (vetor ou ID do comando). | Contrato documentado. |
| 5.3 | IA | Implementar a interface para escolher ligado/desligado, modo, temperatura e ventilador. | Interface envia estados válidos e recusa os inválidos. |
| 5.4 | IA | Integrar a interface com o envio IR pelo ESP32. | Envio funciona com o gerador. |
| 5.5 | Usuário | Testar o aplicativo ponta a ponta no hardware. | Teste registrado na tabela da Fase 4 ou em tabela própria. |

## Riscos

- **Mapeamento incompleto:** temperaturas e modos não capturados não estão cobertos. Cada valor novo precisa de validação no hardware (Fase 4).
- **Campo fixo errado:** um valor de referência que não corresponde à configuração atual do ar pode mudar o comportamento do ar-condicionado.
- **Checksum não encontrado:** a Fase 1.5 não achou soma conhecida. Ela exige uma tabela aprendida das capturas ou uma análise mais profunda do protocolo, o que atrasa a Fase 3.
- **Regra de bits incorreta:** se D1 estiver errada, todos os campos decodificados ficam errados. A regra adotada (espaço longo = 1) é hipótese até o usuário confirmar.
- **Modo e ventilador não decodificados:** sem eles, a Fase 2 não gera a tabela de estados completa, e a Fase 3 depende de decisão de D3.

## Histórico

- Plano criado com base nas capturas em `captures/samsung-ac/` e na proposta final de `CLAUDE.md`.
- Execução inicial: scripts `parse_capturas.py` e `analise_samsung.py`; `captures/estados.csv`; marcação de vetores cortados em `SCENARIOS.md`; resultados das Fases 0, 1 e 2 registrados aqui. D1 adotada como hipótese de trabalho.

## Apêndice A: partes restantes

Este apêndice lista tudo o que ainda falta, separado por responsável, com o que bloqueia cada item. Ele não substitui as tabelas das fases; serve de checklist.

### A.1 Do usuário (captura e decisão)

| Item | O que fazer | Desbloqueia |
|------|-------------|-------------|
| 0.1 | Recapturar `ligar_inicial_cool` duas vezes, mesmo estado inicial. | 0.7, 1.4 (estabilidade) |
| 0.3 | Refazer `decrease_23_to_22_cool` (vazio). | 2.1 (linha nova) |
| 0.4 | Par de captura em cool: 24 → 23 °C e 23 → 24 °C, com estado inicial anotado. | 1.4, 2.2 (confirmar temperatura) |
| 0.5 | Uma captura por modo (cool, heat, auto, fan), temperatura e ventilador fixos. | 1.4, 2.2 (modo) |
| 0.6 | Recapturar os seis vetores cortados listados em `captures/SCENARIOS.md`. | 2.1, 1.4 |
| D1 | Confirmar que espaço longo = 1 (hipótese atual). | Validação de 1.2 e de todos os campos |
| D2 | Escolher onde o app roda: ESP32, computador ou celular. | 3.5, 5.1 |
| D3 | Definir temperaturas e modos da v1. | 2.3, 3.1 |
| 4.1 | Escolher de 3 a 5 estados de teste que não foram capturados. | Fase 4 |
| 4.3, 4.4, 5.5 | Testar no ar-condicionado e registrar na tabela da Fase 4. | Fechamento das Fases 4 e 5 |

### A.2 Da IA (trabalho que depende de dados ou decisões)

| Item | Depende de | Observação |
|------|------------|------------|
| 0.7 | 0.1, 0.4, 0.5 | Anotar estado inicial de cada captura nova em `SCENARIOS.md`. |
| 1.4 (completo) | 0.4, 0.5 | Comparar bit a bit os pares e fechar os campos de temperatura, modo e ventilador. |
| 1.5 (revisão) | Mais dados | Sem checksum simples. Testar uma tabela aprendida das capturas ou outra hipótese. |
| 2.1 (atualização) | 1.4, 0.1 a 0.6 | Adicionar linhas e colunas `modo` e `ventilador` quando decodificadas. |
| 2.2 (completo) | 1.4 | Confirmar ligado (bit `0x20`, seção 1) e decodificar modo e ventilador (byte 5). |
| 2.3 | D3 | Lista final de campos fixos com valor de referência. |
| 3.1 a 3.4 | Fase 2 e D3 | Template do vetor, `scripts/gerador.py`, validação offline e tratamento de erro. |
| 3.5 | D2 | Porte para C++ se o gerador rodar no ESP32; compilar com `scripts/build-sketch.sh`. |
| 4.2, 4.5 | Fase 3 e resultados da Fase 4 | Código de teste e investigação de efeitos colaterais. |
| 5.1 a 5.4 | D2, Fase 4 | Arquitetura, contrato da API, interface e integração com o ESP32. |

### A.3 Bloqueios técnicos em aberto

- **Checksum do último byte de cada seção:** nenhuma soma simples bate (1.5). É o maior risco para a Fase 3.
- **Nibble alto do byte 1 da seção de estado:** varia (`A2` a `E2`) sem correspondência confirmada.
- **Aleta (byte 2, `FE` ↔ `AE`):** hipótese única, sem captura controlada que a confirme.

### A.4 Ordem sugerida

1. Responder D1 (é a única que muda os resultados já calculados).
2. Fazer 0.1, 0.4 e 0.5, que desbloqueiam a maior parte da Fase 1 e da Fase 2.
3. Recapturar os vetores cortados (0.6) e fazer 0.3.
4. Responder D3, que desbloqueia 2.3 e a Fase 3.
5. Responder D2, que desbloqueia 3.5 e a Fase 5.
6. Só então começar a Fase 3 e a validação offline (3.3).
