# Cenários de captura — Samsung AC

Use o `Q0962-Experimento` para capturar cada cenário. Nome sugerido: o próprio nome do cenário, sem acentos (o sketch já normaliza).

Arquivo de saída: `captures/samsung-ac/<nome>.txt` (cole a linha `const uint16_t ...` completa).

| Cenário | Nome sugerido | Status | Arquivo | Observações |
|---------|---------------|--------|---------|-------------|
| Ligar em 24 °C | `ligar_24` | pendente | | |
| Ligar em 21 °C | `ligar_21` | pendente | | |
| Desligar em 27 °C | `desligar_27` | pendente | | |
| Aumentar velocidade do ventilador | `velocidade_aumentar` | pendente | | |
| Reduzir temperatura para 23 °C | `temperatura_23` | pendente | | |

## Regras para comparar cenários

- Capture o mesmo cenário mais de uma vez para verificar se o vetor é estável.
- Anote o estado do ar **antes** de apertar o botão (ex.: ligado em 27 °C, modo frio), porque o controle Samsung envia o estado completo a cada comando.
- Capture com o controle a poucos centímetros do receptor, sempre na mesma posição.
