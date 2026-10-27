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

## Capturas registradas

Arquivos em `captures/samsung-ac/`. O nome do arquivo é igual ao nome do array dentro dele. O sufixo indica o modo do ar (`_cool`, `_heat`). A descrição vem do nome do arquivo; o que não está claro no nome fica como "confirmar".

| Arquivo | Valores | Descrição (pelo nome) | Observações |
|---------|---------|-----------------------|-------------|
| `ligar_inicial_cool.txt` | 347 | Ligar, modo cool | Capturada na primeira sessão, junto com `desligar_inicial_cool`. |
| `desligar_inicial_cool.txt` | 347 | Desligar, modo cool | Capturada na primeira sessão. |
| `ligar_2_cool.txt` | 347 | Ligar, modo cool | Segunda captura de ligar. Confirmar a temperatura. |
| `decrease_22_to_21_cool.txt` | 231 | Diminuir de 22 para 21 °C, modo cool | |
| `increase_21_to_22_cool.txt` | 231 | Aumentar de 21 para 22 °C, modo cool | |
| `increase_22_to_23_cool.txt` | 163 | Aumentar de 22 para 23 °C, modo cool | Vetor mais curto que os demais. Conferir se a captura não foi cortada. |
| `decrease_23_to_22_cool.txt` | 0 | Diminuir de 23 para 22 °C, modo cool | **Vazio. Refazer a captura.** |
| `decrease_24_to_23_heat.txt` | 187 | Diminuir de 24 para 23 °C, modo heat | |
| `decrease_25_to_24_heat.txt` | 231 | Diminuir de 25 para 24 °C, modo heat | |
| `decrease_26_to_25_heat.txt` | 231 | Diminuir de 26 para 25 °C, modo heat | |
| `increase_24_to_25_heat.txt` | 117 | Aumentar de 24 para 25 °C, modo heat | Vetor curto (117). Conferir se a captura não foi cortada. |
| `increase_25_to_26_heat.txt` | 231 | Aumentar de 25 para 26 °C, modo heat | |
| `fan_decrease_1_cool.txt` | 231 | Ventilador: diminuir (1), modo cool | Um aperto a partir do estado inicial. |
| `fan_decrease_2_cool.txt` | 225 | Ventilador: diminuir (2), modo cool | Dois apertos a partir do estado inicial (o segundo logo após o primeiro). |
| `fan_increase_1_cool.txt` | 231 | Ventilador: aumentar (1), modo cool | Um aperto a partir do estado inicial. |
| `fan_increase_2_cool.txt` | 231 | Ventilador: aumentar (2), modo cool | Dois apertos a partir do estado inicial (o segundo logo após o primeiro). |
| `fan_move_cool.txt` | 231 | Ventilador: mover aleta, modo cool | |
| `fan_stop_moving_cool.txt` | 231 | Ventilador: parar aleta, modo cool | |
| `fan_decrease_1_heat_23.txt` | 193 | Ventilador: diminuir (1), modo heat, 23 °C | Um aperto a partir do estado inicial. |
| `fan_decrease_2_heat_23.txt` | 119 | Ventilador: diminuir (2), modo heat, 23 °C | Dois apertos a partir do estado inicial. Vetor curto (119). Conferir se a captura não foi cortada. |
| `fan_increase_1_heat_23.txt` | 231 | Ventilador: aumentar (1), modo heat, 23 °C | Um aperto a partir do estado inicial. |
| `fan_increase_2_heat_23.txt` | 231 | Ventilador: aumentar (2), modo heat, 23 °C | Dois apertos a partir do estado inicial. |
| `mode_auto_from_heat.txt` | 231 | Troca de modo para auto, vindo de heat | |
| `mode_auto_from_heat_2_23_to_22.txt` | 231 | Troca de modo para auto, vindo de heat, 23 → 22 °C | Confirmar as temperaturas. |
| `mode_cool_from_auto.txt` | 231 | Troca de modo para cool, vindo de auto | |
| `mode_cool_from_auto_2_22_to_22.txt` | 231 | Troca de modo para cool, vindo de auto, 22 → 22 °C | Confirmar as temperaturas. |
| `mode_heat_from_fan.txt` | 231 | Troca de modo para heat, vindo de fan | |

Cenários ainda sem captura nesta lista: a tabela de planejamento acima continua valendo como referência.

## Regras para comparar cenários

- Capture o mesmo cenário mais de uma vez para verificar se o vetor é estável.
- Anote o estado do ar **antes** de apertar o botão (ex.: ligado em 27 °C, modo frio), porque o controle Samsung envia o estado completo a cada comando.
- Capture com o controle a poucos centímetros do receptor, sempre na mesma posição.
