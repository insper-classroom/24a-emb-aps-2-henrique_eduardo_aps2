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
#define A_button 14
#define ADC_y 26
#define ADC_y_ID 0

QueueHandle_t xQueueBTN;
SemaphoreHandle_t xSemaphore_r;
void btn_callback(uint gpio, uint32_t events){
    int button_pressed;
    if(events=0x04){
        if(gpio == A_button){
            button_pressed=1;
            
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
                sleep_ms(500);
                if(btn==1){
                    printf("Enviando o botão A\n");
                    
                    uart_puts(HC06_UART_ID, "A");
                    vTaskDelay(pdMS_TO_TICKS(100));
                    
                }
            }
        // }
    }
}

int main() {
    stdio_init_all();
    xQueueBTN = xQueueCreate(32, sizeof(int));
    gpio_init(A_button);
    gpio_set_dir(A_button,GPIO_IN);
    gpio_pull_up(A_button);
    printf("Start bluetooth task\n");

    xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);
    // xTaskCreate(y_task, "y_task", 4095, NULL, 1, NULL);
    gpio_set_irq_enabled_with_callback(A_button, GPIO_IRQ_EDGE_FALL , true, &btn_callback);
    xSemaphore_r = xSemaphoreCreateBinary();
    vTaskStartScheduler();

    while (true)
        ;
}
