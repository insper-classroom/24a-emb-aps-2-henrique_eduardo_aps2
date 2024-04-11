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
void btn_callback(uint gpio, uint32_t events){
    int button_pressed;
    if(events == GPIO_IRQ_EDGE_FALL){
        if(gpio == A_button){
            button_pressed=1;
            
            xQueueSendFromISR(xQueueBTN,&button_pressed,0);
        }
        if(gpio == S_button){
            button_pressed=2;
            
            xQueueSendFromISR(xQueueBTN,&button_pressed,0);
        }
        if(gpio == J_button){
            button_pressed=3;
            
            xQueueSendFromISR(xQueueBTN,&button_pressed,0);
        }
        if(gpio == K_button){
            button_pressed=4;
            
            xQueueSendFromISR(xQueueBTN,&button_pressed,0);
        }
        if(gpio == L_button){
            button_pressed=5;
            
            xQueueSendFromISR(xQueueBTN,&button_pressed,0);
        }
    }
}
// void y_task(void *p) {
//     adc_init();
//     adc_gpio_init(ADC_y);
//     int envia = 0;
//     while(true) {
//         adc_select_input(ADC_y_ID);
    
//         int result = adc_read();
//         result -= 2047;
//         result = result * 255 / 2047;

//         if (result <= 140 + 30 && result >= 110) {
//             result = 0;
            
//         }
        
         
//         if (result > 200) {                 
//             xSemaphoreGive(xSemaphore_r);
                
//         }
        
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
// }

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("SelberEmbarcados", "1234");
    int btn;
    while (true) {
        // if(xSemaphoreTake(xSemaphore_r, pdMS_TO_TICKS(500)) == pdTRUE){
            if (xQueueReceive(xQueueBTN,&btn,pdMS_TO_TICKS(100))){

                if(btn==1){
                    printf("Enviando o botão A\n");
                    
                    uart_putc_raw(HC06_UART_ID, 'A');
                    vTaskDelay(pdMS_TO_TICKS(100));
                    
                }
                if(btn==2){
                    printf("Enviando o botão S\n");
                    
                    uart_putc_raw(HC06_UART_ID, 'S');
                    vTaskDelay(pdMS_TO_TICKS(100));
                    
                }
                if(btn==3){
                    printf("Enviando o botão J\n");
                    
                    uart_putc_raw(HC06_UART_ID, 'J');
                    vTaskDelay(pdMS_TO_TICKS(100));
                    
                }
                if(btn==4){
                    printf("Enviando o botão K\n");
                    
                    uart_putc_raw(HC06_UART_ID, 'K');
                    vTaskDelay(pdMS_TO_TICKS(100));
                    
                }
                if(btn==5){
                    printf("Enviando o botão L\n");
                    
                    uart_putc_raw(HC06_UART_ID, 'L');
                    vTaskDelay(pdMS_TO_TICKS(100));
                    
                }
            }
             vTaskDelay(pdMS_TO_TICKS(1));
        // }
    }
}

int main() {
    stdio_init_all();
    xQueueBTN = xQueueCreate(32, sizeof(int));
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
    printf("Start bluetooth task\n");

    xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);
    // xTaskCreate(y_task, "y_task", 4095, NULL, 1, NULL);
    gpio_set_irq_enabled_with_callback(A_button, GPIO_IRQ_EDGE_FALL , true, &btn_callback);
    gpio_set_irq_enabled(S_button, GPIO_IRQ_EDGE_FALL , true);
    gpio_set_irq_enabled(J_button, GPIO_IRQ_EDGE_FALL , true);
    gpio_set_irq_enabled(K_button, GPIO_IRQ_EDGE_FALL , true);
    gpio_set_irq_enabled(L_button, GPIO_IRQ_EDGE_FALL , true);
    
    //xSemaphore_r = xSemaphoreCreateBinary();
    vTaskStartScheduler();

    while (true)
        ;
}
