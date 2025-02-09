# Projeto BitDogLab - Controle de LEDs, Display e Interação via UART

## Descrição do Projeto

Este projeto utiliza a placa BitDogLab para demonstrar o controle de LEDs, matriz WS2812, botões e exibição de informações em um display SSD1306. O projeto também implementa comunicação via UART para entrada de caracteres e controle interativo.

## Componentes Utilizados

- **Matriz 5x5 de LEDs WS2812** (GPIO 7)
- **LED RGB** (GPIOs 11, 12 e 13)
- **Botão A** (GPIO 5)
- **Botão B** (GPIO 6)
- **Display SSD1306** via I2C (GPIOs 14 e 15)

## Funcionalidades Implementadas

### 1. Modificação da Biblioteca `font.h`

- Adicionados caracteres minúsculos personalizados para exibição no display SSD1306.

### 2. Entrada de Caracteres via Serial Monitor

- Usuário digita caracteres pelo Serial Monitor do VS Code.
- Cada caractere é exibido no display SSD1306.
- Caso um número (0 a 9) seja digitado, um símbolo correspondente é mostrado na matriz 5x5 de LEDs WS2812.

### 3. Interação com o Botão A

- Pressionar o botão A alterna o estado do LED RGB Verde.
- O estado do LED é exibido no display SSD1306 e registrado no Serial Monitor.

### 4. Interação com o Botão B

- Pressionar o botão B alterna o estado do LED RGB Azul.
- O estado do LED é exibido no display SSD1306 e registrado no Serial Monitor.

## Requisitos Técnicos Implementados

- **Uso de interrupções (IRQ)** para as funcionalidades dos botões.
- **Debouncing via software** para evitar múltiplas detecções em um único clique.
- **Controle de LEDs comuns e LEDs WS2812**, garantindo domínio de diferentes tipos de controle.
- **Utilização do Display SSD1306** para exibição de fontes maiúsculas e minúsculas via I2C.
- **Envio de informações pela UART** demonstrando comunicação serial eficaz.
- **Código bem estruturado e comentado** para facilitar a manutenção e entendimento.

## Demonstração

Um vídeo de demonstração está disponível no seguinte link: 

