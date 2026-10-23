---
name: ir-capture-experiment
description: Use para executar ou documentar experimentos de captura dos sinais do controle do ar condicionado Samsung com o sketch Q0962-Experimento. Cobre o fluxo de captura, nomes de botão, salvamento em captures/ e o registro em captures/SCENARIOS.md.
---

# Experimento de captura IR

## Objetivo

Registrar, para cada cenário do controle Samsung, o vetor de tempos enviado pelo controle. Esses vetores depois são colados no `Q0962-WiFi` para replicar o comando.

## Fluxo com o Q0962-Experimento

1. Confirme que o sketch está gravado no ESP32 (`esp-code/Q0962-Experimento`).
2. No Serial Monitor (115200 baud), digite o nome do cenário + Enter. Ex.: `ligar 24`. O Serial confirma: `Aperte o botao: ligar_24`.
3. Aperte o botão no controle, com o controle apontado para o receptor e sempre na mesma distância.
4. O Serial imprime duas linhas:
   ```
   nome: ligar_24 | indice: 1
   const uint16_t ligar_24[347] = {...};
   ```
5. Salve a linha `const uint16_t ...` em `captures/samsung-ac/<nome>.txt`. Não edite os números.
6. Atualize a linha correspondente em `captures/SCENARIOS.md` (status, arquivo, observações).

## Regras de nomes

- Use o nome do cenário em snake_case, sem acentos: `ligar_24`, `desligar_27`, `temperatura_23`.
- O sketch troca espaços e `-` por `_`, remove acentos e outros símbolos, e acrescenta `b_` se começar com número. Use o nome já normalizado no arquivo e no `SCENARIOS.md`.

## Boas práticas de captura

- Antes de cada captura, anote o estado do ar (ligado/desligado, temperatura, modo, velocidade). O controle Samsung envia o estado completo a cada comando, então o mesmo botão pode gerar vetores diferentes em estados diferentes.
- Capture cada cenário pelo menos duas vezes. Se os vetores forem muito diferentes, algo deu errado (ruído, distância, ângulo).
- Se o vetor sair com poucos valores (<100 para Samsung), a captura pode ter sido cortada. Verifique o `RECORD_GAP_MICROS` e repita.
- Não use o sketch `Q0962-Receptor` para os experimentos: ele imprime muito mais texto e é mais difícil de copiar.

## Ao concluir

- Confira se todos os arquivos em `captures/samsung-ac/` começam com `const uint16_t` e terminam com `};`.
- Não envie nada ao controle nem ao ar condicionado sem o usuário pedir.
