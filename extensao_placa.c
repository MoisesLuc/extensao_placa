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
            gpio_put(CLK, 0);
            sleep_us(20);
            gpio_put(CLK, 1);
            
            for(int i = 0; i < 8; i++) {
                printf("%d\n", gpio_get(SERIALOUT));

                serial_value[i] = gpio_get(SERIALOUT);
                print_texto(text=binary[serial_value[i]], (10*i), 2, 1);

                gpio_put(CLK, 0);
                sleep_us(20);
                gpio_put(CLK, 1);
                sleep_us(20);

                print_texto(text=binary[serial_value[i]], (10*i), 2, 1);
            }
        }
    }

    return 0;
}
