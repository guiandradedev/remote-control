#!/usr/bin/env python3
"""Lê as capturas Samsung em captures/samsung-ac/ e separa as partes (seções).

Uso: python3 -I scripts/parse_capturas.py [--bits] [arquivo ...]

Sem argumentos, processa todos os captures/samsung-ac/*.txt.
Estrutura observada: cada seção começa com uma marca longa (~3000 µs) seguida
de um espaço longo (~9000 µs), tem 56 bits (2 valores cada) e termina com uma
marca. Entre as seções há um espaço longo (~3000 µs).

Regra de bit (hipótese D1, a confirmar): espaço curto (~500 µs) = 0,
espaço longo (~1500 µs) = 1. Os bits de cada byte saem menos significativo primeiro.
"""

import glob
import os
import re
import sys

LIMITE_MARCA_CABECALHO_MICROS = 2000
LIMITE_ESPACO_BIT_MICROS = 1000
BITS_POR_SECAO = 56
PASTA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "captures", "samsung-ac")

PADRAO_ARRAY = re.compile(r"const\s+uint16_t\s+(\w+)\[\d+\]\s*=\s*\{([^}]*)\}")


def ler_captura(caminho):
    """Retorna (nome, lista de tempos) do arquivo de captura."""
    with open(caminho, encoding="utf-8") as f:
        texto = f.read()
    casamento = PADRAO_ARRAY.search(texto)
    if not casamento:
        raise ValueError(f"{caminho}: linha 'const uint16_t ...' não encontrada")
    nome = casamento.group(1)
    tempos = [int(v) for v in casamento.group(2).replace("\n", " ").split(",") if v.strip()]
    return nome, tempos


def separar_secoes(tempos):
    """Divide o vetor em seções. Uma seção começa em cada marca longa (cabeçalho).

    O espaço longo que separa duas seções fica no fim da seção anterior.
    """
    secoes = []
    atual = []
    for indice, tempo in enumerate(tempos):
        eh_marca = indice % 2 == 0
        if eh_marca and tempo > LIMITE_MARCA_CABECALHO_MICROS and atual:
            secoes.append(atual)
            atual = []
        atual.append(tempo)
    if atual:
        secoes.append(atual)
    return secoes


def bits_da_secao(secao):
    """Converte os espaços de dados da seção em bits. Ignora cabeçalho e marca final."""
    bits = []
    for k in range(BITS_POR_SECAO):
        indice_espaco = 3 + 2 * k
        if indice_espaco >= len(secao):
            break
        bits.append(1 if secao[indice_espaco] > LIMITE_ESPACO_BIT_MICROS else 0)
    return bits


def bytes_da_secao(bits):
    """Agrupa bits em bytes, menos significativo primeiro. Ignora bits incompletos no fim."""
    completos = len(bits) // 8
    resultado = []
    for i in range(completos):
        valor = 0
        for posicao, bit in enumerate(bits[i * 8:(i + 1) * 8]):
            valor |= bit << posicao
        resultado.append(valor)
    return resultado


def resumo(caminho):
    nome, tempos = ler_captura(caminho)
    return nome, tempos, separar_secoes(tempos)


def main(argumentos):
    mostrar_bits = "--bits" in argumentos
    argumentos = [a for a in argumentos if a != "--bits"]
    caminhos = argumentos or sorted(glob.glob(os.path.join(PASTA, "*.txt")))
    print(f"{'arquivo':<40} {'valores':>7} {'secoes':>6}  valores por secao")
    for caminho in caminhos:
        try:
            nome, tempos, secoes = resumo(caminho)
        except (OSError, ValueError) as erro:
            print(f"{os.path.basename(caminho):<40} ERRO: {erro}")
            continue
        por_secao = ", ".join(str(len(s)) for s in secoes)
        print(f"{nome:<40} {len(tempos):>7} {len(secoes):>6}  {por_secao}")
        if mostrar_bits:
            for numero, secao in enumerate(secoes, start=1):
                bits = bits_da_secao(secao)
                hex_bytes = " ".join(f"{b:02X}" for b in bytes_da_secao(bits))
                print(f"    secao {numero}: {len(bits)} bits | {hex_bytes}")


if __name__ == "__main__":
    main(sys.argv[1:])
