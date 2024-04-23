/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>
#include "hc06.h"
#define A_button 15
#define S_button 14
#define J_button 13
#define K_button 12
#define L_button 11
#define ADC_y 26
#define ADC_y_ID 0
#define ADC_Sound 27
#define ADC_Sound_ID 1

QueueHandle_t xQueueBTN;
SemaphoreHandle_t xSemaphore_r;

volatile char byteArray = 0b00000000;
void btn_callback(uint gpio, uint32_t events){
    int button_pressed;
    if(events == GPIO_IRQ_EDGE_FALL){
        if(gpio == A_button){
            byteArray = 0b00000001 | byteArray;
        }
        if(gpio == S_button){
            byteArray = 0b00000010 | byteArray;
        }
        if(gpio == J_button){
            byteArray = 0b00000100 | byteArray;
        }
        if(gpio == K_button){
            byteArray = 0b00001000 | byteArray;
        }
        if(gpio == L_button){
            byteArray = 0b00010000 | byteArray;
        }
    }

    if(events == GPIO_IRQ_EDGE_RISE){
        if(gpio == A_button){
            byteArray = 0b11111110 & byteArray;
        }
        if(gpio == S_button){
            byteArray = 0b11111101 & byteArray;
        }
        if(gpio == J_button){
            byteArray = 0b11111011 & byteArray;
        }
        if(gpio == K_button){
            byteArray = 0b11110111 & byteArray;
        }
        if(gpio == L_button){
            byteArray = 0b11101111 & byteArray;
        }
    }
}

void y_task(void *p) {
    adc_init();
    adc_gpio_init(ADC_y);
    while(true) {
        adc_select_input(ADC_y_ID);
    
        int result = adc_read();
        result -= 2047;
        result = result * 255 / 2047;
         
        if ((result > 240 || result < -240)) {                 
            byteArray = 0b10000000 | byteArray;
        }

        if ((result < 200 && result > -200)) {                 
            byteArray = 0b01111111 & byteArray;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void sound_task(void *p) {
    adc_init();
    adc_gpio_init(ADC_Sound);
    int envia = 0;
    while(true) {
        adc_select_input(ADC_Sound_ID);
    
        int adc_value = adc_read();
        adc_value = adc_value * 100 / 4095;

        if (adc_value < 10)
        {
            byteArray = 0b00100000 | byteArray;
        }
        else if (adc_value > 90)
        {
            byteArray = 0b01000000 | byteArray;
        }
        else
        {
            byteArray = 0b11011111 & byteArray;
            byteArray = 0b10111111 & byteArray;
        }
    }
}

bool timer_0_callback(repeating_timer_t *rt) {
    char bytes = byteArray;
    
    xQueueSendFromISR(xQueueBTN, &bytes, 0);

    return true; // keep repeating
}

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("SelberEmbarcados", "1234");
    char d;

    repeating_timer_t timer_0;

    if (!add_repeating_timer_us(40000, 
                                timer_0_callback,
                                NULL, 
                                &timer_0)) {
        printf("Failed to add timer\n");
    }
    while (true) {
        if (xQueueReceive(xQueueBTN,&d,pdMS_TO_TICKS(100))){ 
            uart_putc_raw(HC06_UART_ID, d);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
int main() {
    stdio_init_all();
    xQueueBTN = xQueueCreate(32, sizeof(char));
    gpio_init(A_button);
    gpio_set_dir(A_button,GPIO_IN);
    gpio_pull_up(A_button);
    gpio_init(S_button);
    gpio_set_dir(S_button,GPIO_IN);
    gpio_pull_up(S_button);
    gpio_init(J_button);
    gpio_set_dir(J_button,GPIO_IN);
    gpio_pull_up(J_button);
    gpio_init(K_button);
    gpio_set_dir(K_button,GPIO_IN);
    gpio_pull_up(K_button);
    gpio_init(L_button);
    gpio_set_dir(L_button,GPIO_IN);
    gpio_pull_up(L_button);

    xTaskCreate(hc06_task, "UART_Task 1", 4095, NULL, 1, NULL);
    xTaskCreate(y_task, "y_task", 4095, NULL, 1, NULL);
    xTaskCreate(sound_task, "sound_task", 4095, NULL, 1, NULL);
    gpio_set_irq_enabled_with_callback(A_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    gpio_set_irq_enabled(S_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(J_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(K_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(L_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    
    xSemaphore_r = xSemaphoreCreateBinary();
    vTaskStartScheduler();

    while (true)
        ;
}