# SHELL 1.0

    Integrantes: 
    - Luiza Lopes RA: 23013823
    - Maria Manzini RA: 23003063
    - Pedro Rodolfo RA: 22
    - Rafael Ramos RA: 23001236
    - Taynara Araújo RA: 22
    - Vitor Takei RA: 22

___

## Pastas
- bin: Possui arquivos binários 
- docs: Arquivos .txt
- lib: Arquivos .h
- src: Arquivos .c e .sh

## Features
- Rodar arquivos executáveis em caminhos relativos(necessita do './' ou '../') ou absolutos:
    - Ex.: "./bin/extras/sum 4 5 6" -> "15"
- Rodar sequências de comandos em um script usando o comando interno 'script':
    - Ex.: "script ./test/runme.sh"
- Rodar múltiplos comandos, incluindo interação de pipeline e escrita em arquivos
    - Ex.: "sum 4 5 & sum 34 6 | pretty > ./result.txt"
-   Possui os comandos base:
    - exit
    - cd
    - path
    - ls
    - cat
    - pwd

## Como instalar o shell

nao entendo de git :(

## Como rodar o shell
> Se tiver apenas a pasta 'src':

    Existe um arquivo batch (script de comandos) que cria os arquivos binários base.
    Apenas execute (se estiver na pasta 'projeto_SO'):
    > ./src/script/create_bin.sh

> Se tiver a pasta bin:

    Execute (se estiver na pasta 'projeto_SO'):
    > ./bin/shell