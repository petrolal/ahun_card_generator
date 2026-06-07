# Ahun Card Generator (ESP32 Edition) 🚀

Este projeto é um gerador de cards e cartazes automatizado para ESP32, capaz de processar templates JSON e renderizar textos sobre imagens em alta resolução utilizando memória RAM externa (PSRAM).

## 🛠️ Requisitos de Hardware

*   **Microcontrolador:** ESP32-WROVER (Obrigatório possuir **PSRAM**).
*   **Armazenamento:** Módulo de Cartão MicroSD (Conectado via SPI).
*   **Cartão SD:** Formatado em FAT32.

## 💻 Setup do Ambiente (Neovim + CLI)

1.  **Instale o PlatformIO Core:**
    ```bash
    pip install platformio
    ```
2.  **Configure o Neovim (LSP):**
    Gere a base de dados do clangd para ter autocomplete perfeito:
    ```bash
    pio run -t compiledb
    ```

## 💾 Preparação do Cartão SD

Antes de ligar o ESP32, você deve copiar o conteúdo da pasta `data/` para a raiz do seu Cartão MicroSD. A estrutura deve ficar assim:

```text
SD_ROOT/
├── images/
│   ├── cura_exu.png
│   ├── festa_tiriri.png
│   └── ... (todas as imagens)
└── templates/
    ├── cura_exu.json
    ├── festa_tiriri.json
    └── ... (todos os arquivos .json)
```

## 🚀 Passo a Passo: Flashing via USB

Siga estes passos para compilar e gravar o firmware no seu ESP32:

### 1. Conexão
Conecte o seu ESP32-WROVER ao computador através de um cabo USB de boa qualidade.

### 2. Permissões de Porta Serial (Linux)
Se for a sua primeira vez usando ESP32 no Linux, garanta que seu usuário tem permissão para acessar a porta USB:
```bash
sudo usermod -a -G dialout $USER
# Reinicie sua sessão (log out/log in) para aplicar.
```

### 3. Compilação e Upload
Dentro da pasta do projeto, execute o comando do PlatformIO:
```bash
# O comando abaixo compila e envia o código via USB automaticamente
pio run -e esp-wrover-kit -t upload
```
*Dica: Se a placa ficar travada em "Connecting...", segure o botão **BOOT** no chip até que o upload comece.*

### 4. Interação via Monitor Serial
Após o upload, abra o console para enviar comandos para o ESP32:
```bash
# O filtro 'colorize' permite ver os logs [SUCCESS] e [ERROR] coloridos
pio device monitor -b 115200 --filter colorize
```

---

## ⌨️ Comandos da CLI Serial
...

Ao abrir o monitor serial, você verá o prompt. Digite `help` para ver os comandos:

*   `list`: Lista todos os templates disponíveis no Cartão SD.
*   `gen <template.json> <texto>`: Gera um card baseado no template e texto fornecidos.
    *   *Exemplo:* `gen cura_exu.json cura=Gira de Pretos Velhos;data=13 de Maio`
*   `calendar <input.txt> <output.json>`: Converte um arquivo de texto de calendário para JSON no SD.

## 🧪 Testes Nativos (Sem Hardware)

Você pode validar a lógica de parsing e comandos diretamente no seu Linux antes de ir para o chip:
```bash
# Compilar para Linux
pio run -e native

# Executar o programa nativo
./.pio/build/native/program -t data/templates/cura_exu.json -x "texto=teste"
```

## 📂 Estrutura do Projeto

*   `lib/AhunCore`: Lógica de templates, calendário e logs.
*   `lib/AhunGraphics`: Motor de renderização LovyanGFX e encoder BMP.
*   `lib/AhunCLI`: Interpretador de comandos para a porta Serial.
*   `data/`: Assets que devem ser enviados para o Cartão SD.

---
*Desenvolvido em C++17 moderno para sistemas embarcados de alta performance.*
