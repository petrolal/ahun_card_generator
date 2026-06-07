# Plano de Migração: Ahun Card Generator para Sistemas Embarcados (ESP32)

Este documento especifica a transição do **Ahun Card Generator** (atualmente um software Desktop/Linux em C++17) para um firmware capaz de rodar em microcontroladores restritos, focando na plataforma **ESP32**.

## 1. Requisitos de Hardware e Restrições

Dada a natureza do processamento de imagens (manipulação de buffers gráficos para inserção de texto e salvamento de arquivos), o hardware alvo **deve possuir PSRAM** (Pseudo-Static RAM externa).

*   **Alvo Recomendado:** Módulo ESP32-WROVER (mínimo de 4MB de PSRAM). O ESP32 padrão (WROOM) ou o Raspberry Pi Pico padrão possuem ~300KB de RAM, insuficientes para manter o frame de um cartaz de alta resolução na memória.
*   **Armazenamento:** Módulo Leitor de Cartão MicroSD (via interface SPI) para armazenar os templates base, fontes (TTF/GFX) e salvar os cards gerados.

## 2. Toolchain e Ferramentas de Desenvolvimento

O ambiente de compilação C/C++ padrão do Linux (Make/CMake) não atende aos requisitos embarcados. A nova stack será:

*   **IDE:** Visual Studio Code (VS Code).
*   **Sistema de Build e Gestão:** Extensão **PlatformIO**.
*   **Framework:** Arduino Core para ESP32 (escolhido pela farta disponibilidade de bibliotecas gráficas e de SD Card) ou ESP-IDF (para máxima otimização).
*   **Hardware Debugging:** Módulo ESP-Prog (Interface JTAG) para debug de memória e execução passo-a-passo.

## 3. Especificação da Arquitetura Embedded C++ (EC++)

A arquitetura sofrerá um "downsizing" focado em economia de ciclos de CPU e controle rígido da fragmentação de memória (*heap*).

### 3.1 Substituição de Bibliotecas
| Função | Biblioteca Desktop Atual | Nova Biblioteca Embarcada (Proposta) | Motivo da Troca |
| :--- | :--- | :--- | :--- |
| **Gráficos** | `ImageMagick` | `TFT_eSPI` (Sprites) ou `LovyanGFX` | ImageMagick requer dezenas de MBs de RAM e OS completo. As alternativas desenham direto em buffers de memória (PSRAM). |
| **Parsing XML** | `libxml2` | `cJSON` (migrar `.xml` para `.json`) | `libxml2` é muito pesado. Como já usamos `cJSON`, usaremos o formato JSON para configurar os templates. |
| **I/O e Filesystem**| `std::filesystem` e `<fstream>` | `SD.h` e `FS.h` (SPI) | Microcontroladores precisam de drivers específicos (FAT32/LittleFS) para acessar Cartões SD. |

### 3.2 Otimizações de Linguagem (C++)
*   **Zero Exceções:** A flag `-fno-exceptions` será ativada no PlatformIO. As classes do tipo `AhunException` serão removidas. O tratamento de erros usará retornos de enumerações (ex: `enum class Status { OK, ERROR_SD, ERROR_MEM }`).
*   **Controle de Fragmentação:** 
    *   Substituir `std::string` (que faz alocação dinâmica invisível) por estruturas de tamanho fixo, como `std::array<char, MAX_LEN>`.
    *   No lugar de vetores dinâmicos, usar arrays pré-alocados ou aplicar rigorosamente `std::vector::reserve()`.

---

## 4. Roadmap de Execução (Checklist)

### Fase 1: Setup Embarcado e Sistema de Arquivos (VFS)
Nesta fase, o software deve ser capaz de ligar no ESP32, montar o Cartão SD e listar arquivos, sem gerar imagens ainda.
- [ ] Inicializar o projeto via CLI: `pio project init --board esp32-wrover`.
- [ ] Gerar base de dados para o LSP do Neovim: `pio run -t compiledb`.
- [ ] Ativar a flag `-fno-exceptions` no `platformio.ini`.
- [ ] Configurar a montagem do módulo Cartão SD (biblioteca `SD.h`).
- [ ] Refatorar as rotinas de I/O para usar a biblioteca File System do ESP32, removendo `std::filesystem`.

### Fase 2: Adaptação de Dados (Fim do XML)
- [ ] Converter fisicamente os templates de `.xml` para formato `.json`.
- [ ] Substituir o uso do wrapper `XmlDocument` pelo `JsonDocument` na classe `CardTemplate`.
- [ ] Refatorar a classe `CalendarParser` para usar as rotinas de arquivo da `SD.h` e gerenciar possíveis faltas de memória durante o uso do `cJSON`.

### Fase 3: EC++ Core (Remoção da Standard Library Pesada)
- [ ] Remover ou adaptar `std::string` nas `structs` como `TextElement` e classes de configuração.
- [ ] Substituir o sistema `try/catch` implementado na refatoração Desktop por fluxos de controle de erro estritos baseados em status (`if (result != Status::OK)`).

### Fase 4: Novo Motor Gráfico (O Desafio da PSRAM)
- [ ] Integrar a biblioteca gráfica escolhida (ex: `LovyanGFX` ou `TFT_eSPI`).
- [ ] Modificar os PNGs de *background* para formato BMP de 16 ou 24 bits, que são mais rápidos de decodificar num microcontrolador.
- [ ] Refatorar a classe `CardGenerator`:
    - Alocar um buffer gráfico (*Sprite*) obrigatoriamente na PSRAM (`heap_caps_malloc(..., MALLOC_CAP_SPIRAM)`).
    - Desenhar o arquivo BMP no buffer.
    - Inserir o texto usando fontes convertidas para o padrão da biblioteca gráfica (arquivos `.vlw` ou headers C).
    - Salvar o buffer resultante de volta para o Cartão SD.

### Fase 5: Integração de Interface (HMI)
Como não há "Linha de Comando" em um chip:
- [ ] Substituir o CLI Parser (`cli.cpp`) por um controle de hardware (botões e display TFT) **OU** por um Servidor Web embutido (WebUI acessada via Wi-Fi) para que o usuário escolha o template e digite o texto do Card através do celular.

### Fase 6: Excelência Industrial (Refinamentos Técnicos)
Esta fase visa transformar o protótipo funcional em um sistema de nível profissional, garantindo fidelidade visual e flexibilidade.
- **Mapeamento de Cores:** Criar uma função utilitária `color_lookup()` para traduzir nomes de cores amigáveis (ex: "yellow") para o formato RGB888/RGB565.
- **Resolução Adaptativa:** Remover o tamanho fixo (800x1200) e permitir que o motor gráfico se ajuste ao arquivo de background carregado.
- **Tipografia:** Implementar o suporte a fontes customizadas para manter a identidade visual dos temas originais.

---

## 5. Estratégia de Testes (Double Targeting)

Para garantir produtividade e evitar o ciclo lento de flash constante no chip, adotaremos a seguinte estratégia:

1.  **Ambiente Nativo (Linux/Neovim):** 
    *   Desenvolver e testar a lógica de parsing (JSON, Calendário) e processamento de templates diretamente no PC.
    *   Usar Mocks para abstrair o sistema de arquivos (mapear o SD Card para uma pasta local).
2.  **Simulação (Wokwi):**
    *   Validar o comportamento do firmware em um ambiente virtual de ESP32 antes do deploy físico.
3.  **Hardware Real (ESP32-WROVER):**
    *   Focar exclusivamente em testes de performance gráfica, latência de I/O do cartão SD e consumo de PSRAM.

---
*Este plano especifica a rota arquitetural para transformar o Ahun Card Generator em um dispositivo hardware (Stand-alone Firmware).*
rser (`cli.cpp`) por um controle de hardware (botões e display TFT) **OU** por um Servidor Web embutido (WebUI acessada via Wi-Fi) para que o usuário escolha o template e digite o texto do Card através do celular.

---
*Este plano especifica a rota arquitetural para transformar o Ahun Card Generator em um dispositivo hardware (Stand-alone Firmware).*