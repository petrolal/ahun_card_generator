# Ahun Card Generator: Status da Migração (C para C++ Moderno)

Este documento rastreia o progresso da migração do projeto de C para C++17, seguindo o Plano de Migração Estruturado.

## ✅ Checklist de Progresso

### Fase 1: Estrutura (Preparação e Wrappers)
- [x] **Tarefa 1.1:** Adicionar guardas `extern "C"` nos headers em `include/`.
- [x] **Tarefa 1.2:** Ajustar `CMakeLists.txt` para suporte `C` e `CXX` (C++17).
- [x] **Tarefa 1.3:** Migrar `main.c` para `main.cpp` e validar compilação mista.
- [x] **Tarefa 1.4:** Construir wrappers RAII (`MagickWandWrapper.hpp`, `XmlWrapper.hpp`, `JsonWrapper.hpp`).

### Fase 2: Lógica (Processamento e Parser)
- [x] **Tarefa 2.1:** Migrar `cli.c` para `cli.cpp` (uso de `std::string`, `CliParser`).
- [x] **Tarefa 2.2:** Migrar `parse_calendar.c` para `parse_calendar.cpp` (uso de `std::ifstream`, `CalendarParser`).
- [x] **Tarefa 2.3:** Migrar `template.c` e `list_templates.c` para `template.cpp` (uso de `std::filesystem`, `CardTemplate`).

### Fase 3: Main, Build & Finalização
- [x] **Tarefa 3.1:** Migrar `card.c` para `card.cpp` (adoção dos wrappers RAII da MagickWand).
- [x] **Tarefa 3.2:** Refatorar `main.cpp` para arquitetura puramente C++ (uso de `std::unique_ptr`, `std::stringstream`, `std::filesystem`).
- [x] **Tarefa 3.3:** Finalizar `CMakeLists.txt` como projeto puramente CXX.

---

## 🚀 Próximas Etapas (Garantia de Qualidade & Polimento)

### Validação e Testes
- [x] **Memory Check:** Validar ausência de leaks de memória. (Confirmado via AddressSanitizer/ASan)
- [x] **Regression Check:** Comparar binariamente o JSON gerado com o exemplo original. (Validado via `jq`)

### Fase 4 (Melhorias Futuras - Opcional)
- [x] **Custom Exceptions:** Substituir retornos `int` por erros tipados em C++. (Implementado via `AhunException`)
- [x] **Logging System:** Integrar uma biblioteca de logs para substituir o `std::cout`. (Implementado via classe `Logger`)
- [ ] **Unit Testing:** Implementar testes unitários (ex: GTest) para as classes `CliParser` e `CalendarParser`.

---
*Última atualização: 07 de Junho de 2026*
