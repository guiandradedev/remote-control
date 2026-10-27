#!/usr/bin/env python3
"""Análises das Fases 0.2, 1.4 e 1.5 do plano (docs/plano-geracao-sinais.md).

Uso: python3 -I scripts/analise_samsung.py

Usa o parser de scripts/parse_capturas.py (mesma pasta). Só considera seções
completas (56 bits = 7 bytes). Os resultados são observações; hipóteses
ficam marcadas como tal na saída.
"""

import glob
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from parse_capturas import (  # noqa: E402
    BITS_POR_SECAO,
    PASTA,
    bits_da_secao,
    bytes_da_secao,
    ler_captura,
    separar_secoes,
)


def carregar():
    """Retorna {nome: (tempos, secoes)} de todas as capturas."""
    capturas = {}
    for caminho in sorted(glob.glob(os.path.join(PASTA, "*.txt"))):
        nome, tempos = ler_captura(caminho)
        capturas[nome] = (tempos, separar_secoes(tempos))
    return capturas


def bytes_completos(secao):
    """Bytes de uma seção, ou None se a seção não tem os 56 bits."""
    bits = bits_da_secao(secao)
    if len(bits) != BITS_POR_SECAO:
        return None
    return bytes_da_secao(bits)


def fase_0_2(capturas):
    print("== 0.2 Estabilidade: ligar_2_cool x ligar_inicial_cool ==")
    a_tempos, _ = capturas["ligar_inicial_cool"]
    b_tempos, _ = capturas["ligar_2_cool"]
    print(f"valores: {len(a_tempos)} x {len(b_tempos)}")
    diferentes = [i for i, (x, y) in enumerate(zip(a_tempos, b_tempos)) if x != y]
    print(f"posições com tempo diferente: {len(diferentes)}")
    print(f"posições: {diferentes[:20]}{' ...' if len(diferentes) > 20 else ''}")
    _, a_secoes = capturas["ligar_inicial_cool"]
    _, b_secoes = capturas["ligar_2_cool"]
    for numero, (sa, sb) in enumerate(zip(a_secoes, b_secoes), start=1):
        ba, bb = bytes_completos(sa), bytes_completos(sb)
        iguais = ba == bb
        print(f"seção {numero}: bytes iguais = {iguais}")
    print()


def fase_1_4(capturas):
    print("== 1.4 Bytes por captura (seções completas) ==")
    tabela = {}
    for nome, (_, secoes) in capturas.items():
        for numero, secao in enumerate(secoes, start=1):
            dados = bytes_completos(secao)
            if dados is not None:
                tabela[(nome, numero)] = dados
    for (nome, numero), dados in sorted(tabela.items()):
        print(f"{nome:<36} s{numero}  " + " ".join(f"{b:02X}" for b in dados))
    print()

    print("== 1.4 Posições de byte que variam, por seção (hipótese: campos) ==")
    for numero in (1, 2, 3):
        grupo = [dados for (nome, n), dados in tabela.items() if n == numero]
        if not grupo:
            continue
        variam = [i for i in range(7) if len({d[i] for d in grupo}) > 1]
        print(f"seção {numero}: {len(grupo)} capturas, bytes que variam: {variam}")
        for i in variam:
            valores = sorted({d[i] for d in grupo})
            print(f"    byte {i}: {' '.join(f'{v:02X}' for v in valores)}")
    print()
    return tabela


def candidatos_checksum(dados):
    """Possíveis checksums dos 6 primeiros bytes da seção, comparados ao 7º byte."""
    seis = dados[:6]
    soma = sum(seis) & 0xFF
    xor = 0
    for b in seis:
        xor ^= b
    nibbles = sum((b >> 4) + (b & 0x0F) for b in seis) & 0xFF
    return {
        "soma mod 256": soma,
        "~soma": (~soma) & 0xFF,
        "xor": xor,
        "~xor": (~xor) & 0xFF,
        "soma dos nibbles": nibbles,
        "soma mod 256 + 0xAF": (soma + 0xAF) & 0xFF,
    }


def fase_1_5(tabela):
    print("== 1.5 Somas de verificação (hipótese, só o 7º byte de cada seção) ==")
    alvo = {}
    for chave, dados in tabela.items():
        for nome_candidato, valor in candidatos_checksum(dados).items():
            alvo.setdefault(nome_candidato, []).append(valor == dados[6])
    for nome_candidato, resultados in alvo.items():
        print(f"{nome_candidato:<22} acerta {sum(resultados)}/{len(resultados)} seções")
    print()


def main():
    capturas = carregar()
    fase_0_2(capturas)
    tabela = fase_1_4(capturas)
    fase_1_5(tabela)


if __name__ == "__main__":
    main()
