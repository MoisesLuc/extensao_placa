#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

//pinos e módulos controlador i2c selecionado
#define I2C_PORT i2c1
#define PINO_SCL 14
#define PINO_SDA 15

//Inicializa botão B
#define BUTTON_B 6

#define LOAD 28       // Pino de LOAD para o CI
#define CLK 17        // Pino de Clock para o CI
#define SERIALOUT 16  // Saída serial


ssd1306_t disp;


//função para inicialização de todos os recursos do sistema
void inicializa() {
    stdio_init_all();
    adc_init();
    i2c_init(I2C_PORT, 400*1000);// I2C Inicialização. Usando 400Khz.
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);

    //inicialização das funções do CI
    gpio_init(LOAD);
    gpio_init(CLK);
    gpio_init(SERIALOUT);
    gpio_set_dir(LOAD, GPIO_OUT);
    gpio_set_dir(CLK, GPIO_OUT);
    gpio_set_dir(SERIALOUT, GPIO_IN);

    //Inicializa Botão B
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
}


//função escrita no display.
void print_texto(char *msg, uint pos_x, uint pos_y, uint scale){
    ssd1306_draw_string(&disp, pos_x, pos_y, scale, msg); //desenha texto
    ssd1306_show(&disp); //apresenta no Oled
}



// 3.3 // LOAD // CLK // SERIALOUT // GND //



int main() {
    inicializa();
    char *text = ""; //texto do display
    int serial_value[] = {0, 0, 0, 0, 0, 0, 0, 0};
    char* binary[] = {"0", "1"};
   
    while(true) {
        int button_press = gpio_get(BUTTON_B);

        if(button_press == 0) {
            ssd1306_clear(&disp); //Limpa a tela do display

            gpio_put(LOAD, 0);
            sleep_us(20);
            gpio_put(CLK, 0);
            sleep_us(20);
            gpio_put(CLK, 1);
            sleep_us(20);

            gpio_put(LOAD, 1);
            sleep_us(20);
            
            // Primeiro lê todos os bits
            for(int i = 0; i < 8; i++) {
                serial_value[7-i] = gpio_get(SERIALOUT);  // Inverte a ordem durante a leitura
                printf("%d", serial_value[7-i]);
                gpio_put(CLK, 0);
                sleep_us(20);
                gpio_put(CLK, 1);
                sleep_us(20);
            }

            // Calcula o valor decimal
            int valor_decimal = 0;
            for(int i = 0; i < 8; i++) {
                valor_decimal = valor_decimal + (serial_value[i] << (7-i));
            }
            
            // Exibe o valor decimal
            char decimal_str[20];
            sprintf(decimal_str, "Dec: %d", valor_decimal);
            print_texto(text=decimal_str, 0, 20, 1);

            // Exibe na ordem correta (não precisa mais inverter aqui)
            for(int i = 0; i < 8; i++) {
                if(i == 0) print_texto(text=binary[serial_value[0]], 0, 40, 1); // Deep Switch 1
                else if(i == 1) print_texto(text=binary[serial_value[1]], 10, 40, 1); // Deep Switch 2
                else if(i == 2) print_texto(text=binary[serial_value[2]], 20, 40, 1); // Deep Switch 3
                else if(i == 3) print_texto(text=binary[serial_value[3]], 30, 40, 1); // Deep Switch 4
                else if(i == 4) print_texto(text=binary[serial_value[4]], 40, 40, 1); // Deep Switch 5
                else if(i == 5) print_texto(text=binary[serial_value[5]], 50, 40, 1); // Deep Switch 6
                else if(i == 6) print_texto(text=binary[serial_value[6]], 60, 40, 1); // Deep Switch 7
                else if(i == 7) print_texto(text=binary[serial_value[7]], 70, 40, 1); // Deep Switch 8
            }
        }
    }

    return 0;
}
