# SHELL 1.0

    Integrantes: 
    - Luiza Lopes RA: 23013823
    - Maria Manzini RA: 23003063
    - Pedro Rodolfo RA: 22886287
    - Rafael Ramos RA: 23001236
    - Taynara Araújo RA: 22904270
    - Vitor Takei RA: 22

___

## Como instalar o shell
- Clone o repositório: 
    https://github.com/FaelMos23/projeto_SO.git

- Acesse a pasta em que o repositório foi clonado: 
    cd projeto_SO/ 

## Como rodar o shell
- Caso a pasta bin esteja vazia se recirecione às pastas 'src' e 'lib' (são as mínimas necessárias para executar o projeto):
    - Existe um arquivo batch (script de comandos) que cria os arquivos binários base.
    - Execute: 
    > ./src/script/create_bin.sh

- Caso a pasta bin já esteja com os arquivos binarios se redirecione a ela.
    -Execute:
    > ./bin/shell

## Para adicionar novos comandos
- Uma forma é adicionar o arquivo binário na pasta bin/comm.
- Outra forma é, ao rodar o arquivo, inserir a pasta que possui o comando desejado no path.

        Para utilização de variáveis de ambiente, seguimos um padrão próprio. Recomendo observar no arquivo src/test.c como lidar com as variáveis de ambiente.

## Pastas do projeto
- bin: arquivos binários  
- docs: comentários desenvolvedores em .txt
- lib: bibliotecas personalizadas .h
- src: comandos .c e script .sh

## Features do projeto
- Rodar arquivos executáveis em caminhos relativos(necessita do './' ou '../') ou absolutos:
    - Ex.: "./bin/extras/sum 4 5 6" -> "15"
- Rodar sequências de comandos em um script usando o comando interno 'script':
    - Ex.: "script ./test/runme.sh"
- Rodar múltiplos comandos, incluindo interação de pipeline e escrita em arquivos
    - Ex.: "sum 4 5 & sum 34 6 | pretty > ./result.txt"
- Como utilizar os comandos base:
    - exit
        - o programa fecha
        > exit
    - cd 
        - caminho relativo 
        >cd ./diretorio 

        >cd diretorio
        - caminho absoluto
        >cd /diretorio

    - path
        - variável caminho do shell atual
        > path 
        - modifica caminho da shell atual para PATH:diretorio
        > path /diterório
    - ls
        - listagem de pastas e arquivos
        > ls
        - listagem com detalhes como permissão e tamanho
        > ls -l
    - cat
        - imprime no terminal o conteúdo de um arquivo
        > cat target.file
    - pwd
        - listagem do diretorio de trabalho atual
        > pwd
