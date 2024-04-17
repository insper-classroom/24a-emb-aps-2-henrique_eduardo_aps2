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
#define A_button 11
#define S_button 12
#define J_button 13
#define K_button 14
#define L_button 15
#define ADC_y 26
#define ADC_y_ID 0

QueueHandle_t xQueueBTN;
SemaphoreHandle_t xSemaphore_r;

typedef struct {
    int button;
    int level;
} data;

void btn_callback(uint gpio, uint32_t events){
    int button_pressed;
    if(events == GPIO_IRQ_EDGE_FALL){
        if(gpio == A_button){
            button_pressed=1;
            data d;
            d.button = 1;
            d.level = 1;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == S_button){
            button_pressed=2;
            data d;
            d.button = 2;
            d.level = 1;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == J_button){
            button_pressed=3;
            data d;
            d.button = 3;
            d.level = 1;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == K_button){
            button_pressed=4;
            data d;
            d.button = 4;
            d.level = 1;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == L_button){
            button_pressed=5;
            data d;
            d.button = 5;
            d.level = 1;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
    }

    if(events == GPIO_IRQ_EDGE_RISE){
        if(gpio == A_button){
            button_pressed=1;
            data d;
            d.button = 1;
            d.level = 0;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == S_button){
            button_pressed=2;
            data d;
            d.button = 2;
            d.level = 0;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == J_button){
            button_pressed=3;
            data d;
            d.button = 3;
            d.level = 0;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == K_button){
            button_pressed=4;
            data d;
            d.button = 4;
            d.level = 0;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
        if(gpio == L_button){
            button_pressed=5;
            data d;
            d.button = 5;
            d.level = 0;
            
            xQueueSendFromISR(xQueueBTN,&d,0);
        }
    }
}

void y_task(void *p) {
    adc_init();
    adc_gpio_init(ADC_y);
    int envia = 0;
    while(true) {
        adc_select_input(ADC_y_ID);
    
        int result = adc_read();
        result -= 2047;
        result = result * 255 / 2047;
         
        if ((result > 240 || result < -240) && envia == 0) {                 
            xSemaphoreGive(xSemaphore_r);
            envia = 1;   
        }

        if ((result < 200 && result > -200) && envia == 1) {                 
            envia = 0;  
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("SelberEmbarcados", "1234");
    data d;
    while (true) {
        if (xQueueReceive(xQueueBTN,&d,pdMS_TO_TICKS(100))){
            if (d.level == 0)
            {
                if(d.button==1){                    
                    uart_putc_raw(HC06_UART_ID, 'A');
                    uart_putc_raw(HC06_UART_ID, d.level);

                }
                if(d.button==2){
                    uart_putc_raw(HC06_UART_ID, 'S');
                    uart_putc_raw(HC06_UART_ID, d.level);

                }
                if(d.button==3){                    
                    uart_putc_raw(HC06_UART_ID, 'J');
                    uart_putc_raw(HC06_UART_ID, d.level);

                }
                if(d.button==4){                    
                    uart_putc_raw(HC06_UART_ID, 'K');
                    uart_putc_raw(HC06_UART_ID, d.level);

                }
                if(d.button==5){                    
                    uart_putc_raw(HC06_UART_ID, 'L');
                    uart_putc_raw(HC06_UART_ID, d.level);
                }
            }
            
            else
            {
                if(xSemaphoreTake(xSemaphore_r, pdMS_TO_TICKS(500)) == pdTRUE){
                    if(d.button==1){                    
                        uart_putc_raw(HC06_UART_ID, 'A');
                        uart_putc_raw(HC06_UART_ID, d.level);
                    }
                    if(d.button==2){
                        uart_putc_raw(HC06_UART_ID, 'S');
                        uart_putc_raw(HC06_UART_ID, d.level);
                    }
                    if(d.button==3){                    
                        uart_putc_raw(HC06_UART_ID, 'J');
                        uart_putc_raw(HC06_UART_ID, d.level);

                    }
                    if(d.button==4){                    
                        uart_putc_raw(HC06_UART_ID, 'K');
                        uart_putc_raw(HC06_UART_ID, d.level);
                    }
                    if(d.button==5){                    
                        uart_putc_raw(HC06_UART_ID, 'L');
                        uart_putc_raw(HC06_UART_ID, d.level);
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
int main() {
    stdio_init_all();
    xQueueBTN = xQueueCreate(32, sizeof(data));
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

    xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(y_task, "y_task", 4095, NULL, 1, NULL);
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