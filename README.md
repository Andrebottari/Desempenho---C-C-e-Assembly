# Análise Comparativa de Desempenho das Linguagens C, C++ e Assembly no Microcontrolador ATmega328P

Este repositório contém os códigos desenvolvidos para o Trabalho de Conclusão de Curso de **André Soares Bottari**, intitulado **“Análise Comparativa de Desempenho das Linguagens C, C++ e Assembly no Microcontrolador ATmega328P”**, realizado na Universidade Federal de Mato Grosso (UFMT), sob orientação do **Prof. Dr. Jésus Franco Bueno**.



🎯 Objetivo

Avaliar o desempenho das linguagens **Assembly**, **C** e **C++** aplicadas a sistemas embarcados, comparando métricas de:
- Tempo de execução (ciclos de clock)
- Tamanho do binário gerado (memória Flash)
- Eficiência no uso de abstrações

Os testes foram conduzidos no **microcontrolador ATmega328P** (arquitetura AVR), executando as mesmas tarefas implementadas em cada linguagem.

---

## ⚙️ Ambiente de Desenvolvimento

- **Plataforma utilizada:** [Microchip Studio](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio)  
- **Toolchain:** AVR-GCC  
- **Microcontrolador:** ATmega328P  
- **Frequência de operação:** 16 MHz  
- **Conexão:** Interface ISP (ou via USBasp/Arduino como ISP)

---

Cada pasta contém o código-fonte equivalente em uma das linguagens testadas.  
Os experimentos incluem rotinas de **PWM**, **manipulação de GPIO** e **operações bit a bit**.

---

## ▶️ Como Executar os Códigos

1. **Abrir o projeto no Microchip Studio**
   - Vá em `File → Open → Project/Solution` e selecione o arquivo do código desejado.

2. **Selecionar o microcontrolador**
   - Certifique-se de que o dispositivo configurado é o **ATmega328P**.

3. **Compilar o código**
   - Clique em **Build → Build Solution** ou pressione `F7`.  
   - O compilador **AVR-GCC** gerará o arquivo `.hex` na pasta `Debug` ou `Release`.

4. **Gravar no microcontrolador**
   - Conecte o ATmega328P via **programador ISP**.
   - Vá em **Tools → Device Programming**, selecione o programador e grave o `.hex`.

5. **Executar o programa**
   - Após a gravação, o código será executado automaticamente no microcontrolador.
   - É possível observar os resultados via osciloscópio (GPIO/PWM) ou comunicação serial (quando disponível).

---

## 📊 Experimentos

Cada implementação realiza:
- **Controle PWM** de motor DC via ponte H (L298N)
- **Alternância de pino (GPIO Toggle)** com medição de ciclos via `Timer1`
- **Operações lógicas bit a bit** sobre blocos de memória (AND, OR, NOT)

Os tempos e tamanhos dos binários foram coletados diretamente no hardware real.

---

## 🧠 Resultados Esperados

- **Assembly:** menor tempo e binário mais compacto.  
- **C:** bom equilíbrio entre desempenho e clareza.  
- **C++:** maior modularidade com custo moderado.  
- **Arduino API:** maior overhead e menor eficiência.

---

## 📄 Licença

Este projeto é de uso **educacional e acadêmico**.  
O conteúdo pode ser reproduzido com a devida citação do autor.

---

**Autor:** André Soares Bottari  
**Curso:** Engenharia de Computação — UFMT  
**Orientador:** Prof. Dr. Jésus Franco Bueno  
**Ano:** 2025

