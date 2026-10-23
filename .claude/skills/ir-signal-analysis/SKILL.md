---
name: ir-signal-analysis
description: Use para analisar os vetores IR capturados em captures/samsung-ac/ — comparar cenários, medir tamanho e estrutura de frames, identificar partes do sinal e formular hipóteses sobre o protocolo Samsung. Use quando o usuário pedir comparação entre botões, decodificação ou explicação de um vetor.
---

# Análise de sinais IR

## Entrada

Cada arquivo em `captures/samsung-ac/*.txt` contém uma linha:

```
const uint16_t <nome>[N] = {t0, t1, t2, ..., tN-1};
```

- Índices pares (`t0`, `t2`, ...) são **marcas** (LED aceso, portadora 38 kHz).
- Índices ímpares (`t1`, `t3`, ...) são **espaços** (LED apagado).
- Os tempos já estão em microssegundos e já têm a compensação de `MARK_EXCESS_MICROS` (20 µs).

## Procedimento

1. **Parse:** extraia o nome e a lista de valores de cada arquivo. Use Python com `-I` (ou um script de análise em `scripts/`) para não depender do diretório atual.
2. **Estrutura:** para cada vetor, identifique:
   - Número de valores e se é par (começa e termina em marca?).
   - Cabeçalho: marca longa + espaço longo no início (ex.: ~9000 / ~4500 µs).
   - Os tempos mais frequentes de marca e de espaço (usualmente ~480–580 µs de marca e ~480 µs / ~1500 µs de espaço, dependendo do protocolo).
   - Pausas longas (>2000 µs) que separam **partes/frames** do mesmo comando.
3. **Comparação:** para dois cenários, compare:
   - Tamanho total e número de partes.
   - Posições onde os valores diferem (bits de dados) versus onde são iguais (cabeçalho, bits fixos).
   - Possível decodificação em bits: marca curta + espaço curto = 0; marca curta + espaço longo = 1 (ou o inverso; confirme com dados).
4. **Hipótese:** registre o que foi comprovado (padrão observado em vários vetores) e o que é especulação. Não afirme o protocolo Samsung sem evidência em mais de um cenário.

## Saída esperada

- Tabela resumida: cenário, número de valores, número de partes, duração total aproximada.
- Diferenças entre cenários em termos de posições de bits, quando possível.
- Lista de perguntas ou capturas adicionais necessárias para confirmar a hipótese.

## Cuidados

- Vetores muito curtos (<100 valores para Samsung) indicam captura cortada. Não os use para comparação e sinalize-os.
- Não modifique os arquivos em `captures/`. Análises vão para um arquivo separado ou para a resposta.
- O objetivo final é replicar comandos. Se a decodificação em bits não for necessária para isso, diga que a replicação do vetor original já resolve o caso.
