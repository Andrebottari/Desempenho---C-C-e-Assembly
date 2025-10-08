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
   - Vá em `File → Open → Project/Solution` e selecione o arquivo desejado.

2. **Selecionar o microcontrolador**
   - Certifique-se de que o dispositivo configurado é o **ATmega328P**.

3. **Compilar o código**
   - Clique em **Build → Build Solution** ou pressione `F7`.
   - O compilador **AVR-GCC** gerará o arquivo `.hex` na pasta `Debug` ou `Release`.

4. **Gravar no microcontrolador**
   - Conecte o ATmega328P via **programador ISP**.
   - Vá em **Tools → Device Programming**, selecione o programador e grave o arquivo `.hex`.

5. **Executar o programa**
   - Após a gravação, o código será executado automaticamente.
   - Os resultados podem ser observados por osciloscópio (PWM/GPIO) ou via comunicação serial, dependendo do experimento.

---

## 🤖 Uso de Inteligência Artificial

Durante o desenvolvimento deste trabalho, foi utilizada **Inteligência Artificial (IA)** como ferramenta auxiliar para:
- **Correção de sintaxe** e **formatação** dos códigos em Assembly, C e C++;
  
O uso da IA não substituiu o entendimento dos conceitos, mas serviu como apoio técnico e didático no processo de revisão e estruturação.

---

## ⚠️ Limitações e Propósito Científico

Por se tratar de um **trabalho acadêmico**, este projeto **não é isento de falhas**.  
Os códigos podem conter **problemas de implementação, otimizações ineficientes ou abordagens incompletas**.  
Essa imperfeição é **intencionalmente reconhecida** como parte do propósito científico do repositório.

> O objetivo de disponibilizar os códigos, é, justamente, transformar estes em **parâmetros de benchmark** para estudos futuros — permitindo que a comunidade contribua com **correções, otimizações e comparações adicionais**.  

Desse modo, o projeto busca evoluir de um TCC individual para um **repositório colaborativo de pesquisa aplicada em sistemas embarcados**, contribuindo para o avanço da ciência e da engenharia.

---

## 🧠 Resultados Gerais

- **Assembly:** maior desempenho e menor uso de memória.  
- **C:** equilíbrio entre eficiência e clareza.  
- **C++:** modularidade e abstração com custo previsível.  
- **Arduino API:** simplicidade extrema, porém com alto overhead.

Esses resultados reforçam o impacto das abstrações e a importância de compreender o custo de cada linguagem no contexto embarcado.

---



## 📄 Licença

Este repositório é disponibilizado para fins **acadêmicos e científicos**.  
O conteúdo pode ser reproduzido com a devida citação do autor e da instituição.

---

**Autor:** André Soares Bottari  
**Curso:** Engenharia de Computação — UFMT  
**Orientador:** Prof. Dr. Jésus Franco Bueno  
**Ano:** 2025  
