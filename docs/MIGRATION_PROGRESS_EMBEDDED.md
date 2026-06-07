# Ahun Card Generator: Status da Migração Embarcada (ESP32)

Este documento rastreia o progresso da portabilidade para o ESP32.

## ✅ Checklist de Progresso

### Fase 1: Setup Embarcado e Sistema de Arquivos (VFS)
- [x] **Tarefa 1.1:** Inicializar projeto PlatformIO (`esp-wrover-kit`).
- [x] **Tarefa 1.2:** Gerar `compile_commands.json` para o Neovim.
- [x] **Tarefa 1.3:** Configurar `platformio.ini` (PSRAM, No-Exceptions, Native Env).
- [x] **Tarefa 1.4:** Portar estruturas core (`Status`, `CardConfig`, `TextElement`) com tamanhos fixos.
- [x] **Tarefa 1.5:** Criar `Logger` abstraído (Serial vs Stdout).
- [x] **Tarefa 1.6:** Validar compilação "Double Targeting" (Native e ESP32).

### Fase 2: Adaptação de Dados (Fim do XML)
- [x] **Tarefa 2.1:** Implementar carregamento de templates via JSON (cJSON).
- [x] **Tarefa 2.2:** Abstrair leitura de arquivos (SD Card no ESP32 vs Filesystem no Native).
- [x] **Tarefa 2.3:** Converter arquivos `.xml` originais para `.json`. (Todos os templates convertidos em `data/templates`)

### Fase 3: EC++ Core (Refatoração de Erros)
- [x] **Tarefa 3.1:** Substituir `AhunException` por retornos de `Status`.
- [x] **Tarefa 3.2:** Eliminar blocos `try/catch` e habilitar `-fno-exceptions`.

### Fase 4: Motor Gráfico (LovyanGFX)
- [x] **Tarefa 4.1:** Implementar renderização de BMP/PNG na PSRAM. (Via `drawBmpFile`/`drawPngFile`)
- [x] **Tarefa 4.2:** Implementar escrita de texto via LovyanGFX. (Suporte básico a `drawString`)
- [x] **Tarefa 4.3:** Salvar resultado final no Cartão SD. (Encoder BMP customizado implementado)

### Fase 5: Interface CLI via Serial
- [x] **Tarefa 5.1:** Implementar interpretador de comandos Serial (`help`, `list`, `calendar`, `gen`).
- [x] **Tarefa 5.2:** Integrar CLI no `loop()` principal do ESP32.

### Fase 6: Excelência Industrial (Refinamentos)
- [x] **Tarefa 6.1:** Implementar Mapeador de Cores (String JSON -> Hexadecimal LovyanGFX). (Suporte a nomes básicos e Hex #RRGGBB)
- [x] **Tarefa 6.2:** Suporte a Resolução Dinâmica (Detectar tamanho da imagem ou ler do JSON). (Implementado para BMP)
- [ ] **Tarefa 6.3:** Gestão de Fontes Customizadas (Carregar arquivos `.vlw` ou binários do SD Card).

### Validação final
- [x] **Integração Nativa:** Rodar teste completo no ambiente `native` para validar lógica de templates JSON e mapeamento de texto. (Sucesso!)

---
*Última atualização: 07 de Junho de 2026*
