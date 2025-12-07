#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "ssd1306.h"
#include "font.h"
#include "pico/bootrom.h"

#define botaoB 6  

// Sensor GY-33
#define GY33_I2C_ADDR 0x29
#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

// Display SSD1306
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define endereco 0x3C

// LED RGB
const uint RED_PIN = 13;
const uint GREEN_PIN = 11;
const uint BLUE_PIN = 12;

void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == botaoB) {
        reset_usb_boot(0, 0);
    }
}

// Registradores
#define ENABLE_REG 0x80
#define ATIME_REG 0x81
#define CONTROL_REG 0x8F
#define CDATA_REG 0x94
#define RDATA_REG 0x96
#define GDATA_REG 0x98
#define BDATA_REG 0x9A

void gy33_write_register(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(I2C_PORT, GY33_I2C_ADDR, buffer, 2, false);
}

uint16_t gy33_read_register(uint8_t reg) {
    uint8_t buffer[2];
    i2c_write_blocking(I2C_PORT, GY33_I2C_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, GY33_I2C_ADDR, buffer, 2, false);
    return (buffer[1] << 8) | buffer[0];
}

void gy33_init() {
    gy33_write_register(ENABLE_REG, 0x03);
    gy33_write_register(ATIME_REG, 0xD5);   // integração maior (~200ms)
    gy33_write_register(CONTROL_REG, 0x02); // ganho 16x
}

void gy33_read_color(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {
    *c = gy33_read_register(CDATA_REG);
    *r = gy33_read_register(RDATA_REG);
    *g = gy33_read_register(GDATA_REG);
    *b = gy33_read_register(BDATA_REG);
}

// Classificação ajustada com base nos parâmetros
const char* identifica_cor(uint16_t r, uint16_t g, uint16_t b) {
    if (r > 800 && r < 2100 && g > 700 && g < 1300 && b > 600 && b < 1100) {
        return "Vermelho";
    } else if (r > 2700 && g > 3500 && b < 1900) {
        return "Amarelo";
    } else if (r >= 400 && r <= 1300 && g >= 950 && g <= 2600 && b >= 850 && b <= 2150) {
        return "Verde";
    } else if (b > 2500 && b < 3100 && r >= 800 && r <= 1700 && g >= 1700 && g <= 2700) {
        return "Azul";
    } else {
        return "Indefinido";
    }
}

int main() {
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    stdio_init_all();
    sleep_ms(2000);

    // Display
    i2c_init(I2C_PORT_DISP, 400 * 1000);
    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISP);
    gpio_pull_up(I2C_SCL_DISP);
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT_DISP);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // LEDs
    gpio_init(RED_PIN); gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_init(GREEN_PIN); gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_init(BLUE_PIN); gpio_set_dir(BLUE_PIN, GPIO_OUT);

    // Sensor
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    gy33_init();

    while (1) {
        uint16_t r, g, b, c;
        gy33_read_color(&r, &g, &b, &c);

        printf("R:%d G:%d B:%d C:%d\n", r, g, b, c);

        const char* cor_nome = identifica_cor(r,g,b);

        char str[32];
        snprintf(str,sizeof(str),"R:%d G:%d B:%d",r,g,b);

        ssd1306_fill(&ssd,false);
        ssd1306_draw_string(&ssd, cor_nome, 10, 10);   // só nome da cor
        ssd1306_draw_string(&ssd, str, 10, 30);        // valores RGB
        ssd1306_send_data(&ssd);

        // LED correspondente
        if (strcmp(cor_nome,"Vermelho")==0) {
            gpio_put(RED_PIN,1); gpio_put(GREEN_PIN,0); gpio_put(BLUE_PIN,0);
        } else if (strcmp(cor_nome,"Amarelo")==0) {
            gpio_put(RED_PIN,1); gpio_put(GREEN_PIN,1); gpio_put(BLUE_PIN,0);
        } else if (strcmp(cor_nome,"Verde")==0) {
            gpio_put(RED_PIN,0); gpio_put(GREEN_PIN,1); gpio_put(BLUE_PIN,0);
        } else if (strcmp(cor_nome,"Azul")==0) {
            gpio_put(RED_PIN,0); gpio_put(GREEN_PIN,0); gpio_put(BLUE_PIN,1);
        } else {
            gpio_put(RED_PIN,0); gpio_put(GREEN_PIN,0); gpio_put(BLUE_PIN,0);
        }

        sleep_ms(500);
    }
}
