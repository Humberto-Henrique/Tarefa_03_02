#include <stdlib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/uart.h" 
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C


#define BUTTON_A 5
#define BUTTON_B 6
#define LED_VERDE 11
#define LED_AZUL 12
#define WS2812_PIN 7


volatile bool led_estado_V = false;
volatile bool led_estado_A = false;
static uint32_t ultimo_tempo_A = 0;
static uint32_t ultimo_tempo_B = 0;
#define DEBOUNCE_DELAY 200 // Tempo de debounce em ms

#define IS_RGBW false

ssd1306_t ssd; // Estrutura do display

const uint8_t numeros[10][5] = {
  {0b01110, 0b01010, 0b01010, 0b01010, 0b01110}, // 0
  {0b01110, 0b00100, 0b00100, 0b01100, 0b00100}, // 1
  {0b01110, 0b01000, 0b01110, 0b00010, 0b01110}, // 2
  {0b01110, 0b00010, 0b01110, 0b00010, 0b01110}, // 3
  {0b01000, 0b00010, 0b01110, 0b01010, 0b01010}, // 4
  {0b01110, 0b00010, 0b01110, 0b01000, 0b01110}, // 5
  {0b01110, 0b01010, 0b01110, 0b01000, 0b01110}, // 6
  {0b01000, 0b00010, 0b01000, 0b00010, 0b01110}, // 7
  {0b01110, 0b01010, 0b01110, 0b01010, 0b01110}, // 8
  {0b01110, 0b00010, 0b01110, 0b01010, 0b01110}  // 9
};

// Função para exibir o número na matriz WS2812 5x5
void exibir_numero_ws2812(uint8_t numero) {
    if (numero < 0 || numero > 9) return;  // Se o número for inválido, retorna

    // Para cada linha da matriz, acende os LEDs correspondentes
    for (int linha = 0; linha < 5; linha++) {
        uint8_t leds = numeros[numero][linha];  // Padrão da linha

        for (int coluna = 0; coluna < 5; coluna++) {
            // Verifica se o bit correspondente à coluna está aceso
            bool led_aceso = (leds & (1 << (4 - coluna))) != 0;
            
            // Aqui você pode definir a cor dos LEDs. Usamos 0xFF0000 (vermelho) como exemplo
            uint32_t cor = led_aceso ? 0xFF0000 : 0x000000; // LED aceso em vermelho, apagado em preto
            pio_sm_put_blocking(pio0, 0, cor);  // Envia o valor para o PIO (cor do LED)
        }
    }
}


void atualizar_display(const char *led, bool estado) {
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_draw_string(&ssd, led, 4, 0); // Nome do LED na linha 0
    ssd1306_draw_string(&ssd, estado ? "LIGADO" : "DESLIGADO", 4, 16); // Estado na linha 1
    ssd1306_send_data(&ssd); // Atualiza o display
}


void __isr_button_pressed(uint gpio, uint32_t eventos) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());


    if (gpio == BUTTON_A && (tempo_atual - ultimo_tempo_A >= DEBOUNCE_DELAY)) {
        ultimo_tempo_A = tempo_atual;
        led_estado_V = !led_estado_V;
        gpio_put(LED_VERDE, led_estado_V);
        atualizar_display("LED VERDE", led_estado_V);
    }


    if (gpio == BUTTON_B && (tempo_atual - ultimo_tempo_B >= DEBOUNCE_DELAY)) {
        ultimo_tempo_B = tempo_atual;
        led_estado_A = !led_estado_A;
        gpio_put(LED_AZUL, led_estado_A);
        atualizar_display("LED AZUL", led_estado_A);
    }
}


int ler_caractere_uart() {
    if (stdio_usb_connected()) {  // Verifica se há dados disponíveis no UART0
        return getchar_timeout_us(0);  // Lê o caractere do UART0
    }
    return -1;  // Retorna -1 se não houver dados
}


int main() {
    stdio_init_all();

    // Inicializa a interface I2C
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);


    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);


    // Configura os botões e LEDs
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);


    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);


    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);


    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);


    // Configura interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &__isr_button_pressed);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);


    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    while (true) {
        int caractere = getchar_timeout_us(0);  // Lê o caractere do UART

     
        if (caractere != PICO_ERROR_TIMEOUT) {  // Se houver um caractere válido
            ssd1306_fill(&ssd, false); // Limpa o display
            char buffer[2] = {(char)caractere, '\0'};  // Cria uma string com o caractere lido
            ssd1306_draw_string(&ssd, "CARACTERE", 30, 0); 
            ssd1306_draw_string(&ssd, buffer, 63, 16);  // Exibe o caractere no display
            ssd1306_send_data(&ssd); // Atualiza o display

            if (caractere >= '0' && caractere <= '9') {
              int numero = caractere - '0';  // Converte o caractere para o número correspondente
              exibir_numero_ws2812(numero);  // Exibe o número na matriz WS2812
        }
      }
        
        sleep_ms(100); // Pequeno atraso para eficiência
      }
  
    }
  
