# Projeto de Jogo

Este guia irá ajudá-lo a configurar o ambiente de desenvolvimento, compilar e executar o projeto, e entender a estrutura de diretórios. O projeto utiliza um **DevContainer** para garantir que o ambiente de desenvolvimento seja configurado de maneira rápida e eficiente usando **Docker** no **Visual Studio Code (VSCode)**.

## Requisitos

Antes de começar, você precisará garantir que alguns requisitos estão instalados no seu sistema.

### Requisitos do Sistema

- **Docker**: O Docker é necessário para criar e rodar o DevContainer.
  - Instalação: [Instruções para instalar o Docker](https://docs.docker.com/get-docker/)

### Requisitos para o VSCode

- **Visual Studio Code**: O editor de código recomendado para trabalhar com este projeto.
  - Instalação: [Baixar Visual Studio Code](https://code.visualstudio.com/Download)

- **Extensões do VSCode**:
  - **Remote - Containers**: Extensão necessária para rodar o DevContainer.
    - Instalação: [VSCode Remote - Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

## Estrutura de Diretórios

Aqui está a estrutura do projeto e uma breve explicação sobre cada diretório e arquivo:

```
.
├── assets          # Pasta para armazenar os assets do jogo (imagens, sons, etc.)
├── bin             # Diretório onde o binário do projeto será gerado
│   └── projeto     # Binário executável gerado após a compilação. Executado com ./bin/projeto
├── build.py        # Script em Python para compilar o projeto automaticamente
├── CMakeLists.txt  # Arquivo de configuração do CMake para o build do projeto
├── deps            # Submódulo contendo a biblioteca Raylib, necessária para o jogo
│   └── raylib      # Raylib como submódulo, não editar manualmente
├── .devcontainer   # Arquivos de configuração do DevContainer
│   ├── devcontainer.json # Configuração do DevContainer
│   └── setup.sh    # Script de configuração adicional para o DevContainer
├── .git            # Diretório de controle de versão Git
├── .gitignore      # Arquivo de exclusão de arquivos para o Git
├── .gitmodules     # Arquivo de configuração dos submódulos Git
├── includes        # Diretório de headers (arquivos .h) do projeto
│   └── main.h      # Header principal do projeto
├── src             # Diretório do código fonte (arquivos .c)
│   └── main.c      # Código principal do jogo
└── .vscode         # Configurações do VSCode
    └── cmake-kits.json # Configuração dos kits do CMake
```

## Configuração e Execução

### Etapas para Configurar o Ambiente

1. **Clone o Repositório**:
   Abra o terminal e clone o projeto com o comando:
   ```bash
   git clone --recurse-submodules https://github.com/thiag0bezerra/projeto-ic
   ```

2. **Abrir o Projeto no VSCode**:
   Abra o VSCode na raiz do projeto:
   ```bash
   code .
   ```

3. **Abrir no DevContainer**:
   Assim que o projeto for aberto no VSCode, o editor irá detectar o arquivo `devcontainer.json` e perguntará se você deseja abrir o projeto no DevContainer. Confirme e o Docker irá configurar automaticamente o ambiente para você.

4. **Compilação**:
   - O projeto possui um script Python (`build.py`) que facilita a compilação. Para compilar o projeto, execute no terminal:
     ```bash
     python3 build.py
     ```
   - O binário gerado será salvo no diretório `bin/` com o nome `projeto`.

5. **Executar o Projeto**:
   Após a compilação, você pode rodar o jogo executando o binário gerado:
   ```bash
   ./bin/projeto
   ```


## Contribuindo

Para contribuir com este projeto, siga as instruções abaixo:

1. **Faça um Fork do Projeto**.
2. **Crie uma branch para sua feature**:
   ```bash
   git checkout -b minha-feature
   ```
3. **Faça o Commit das suas alterações**:
   ```bash
   git commit -m 'Minha nova feature'
   ```
4. **Envie para o repositório remoto**:
   ```bash
   git push origin minha-feature
   ```
5. **Abra um Pull Request**.

