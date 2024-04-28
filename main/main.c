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
#define ADC_y 28
#define ADC_y_ID 2
#define ADC_Sound 27
#define ADC_Sound_ID 1
#define Button_Overload 10
#define Button_Reset 7
#define LED1_R 18
#define LED1_G 17
#define LED1_B 16
#define LED2_R 19
#define LED2_G 20
#define LED2_B 21
#define Bluetooth_State 9

QueueHandle_t xQueueBTN;
QueueHandle_t xQueueState;

volatile int byteArray = 0b0000000000000000;
void btn_callback(uint gpio, uint32_t events){
    if(events == GPIO_IRQ_EDGE_FALL){
        if(gpio == A_button){
            byteArray = 0b0000000000000001 | byteArray;
        }
        if(gpio == S_button){
            byteArray = 0b0000000000000010 | byteArray;
        }
        if(gpio == J_button){
            byteArray = 0b0000000000000100 | byteArray;
        }
        if(gpio == K_button){
            byteArray = 0b0000000000001000 | byteArray;
        }
        if(gpio == L_button){
            byteArray = 0b0000000000010000 | byteArray;
        }
        if (gpio == Button_Overload)
        {
            byteArray = 0b0000000000100000 | byteArray;
        }
        if (gpio == Button_Reset)
        {
            byteArray = 0b0000010000000000 | byteArray;            
        }
        
    }

    if(events == GPIO_IRQ_EDGE_RISE){
        if(gpio == A_button){
            byteArray = 0b111111111111110 & byteArray;
        }
        if(gpio == S_button){
            byteArray = 0b111111111111101 & byteArray;
        }
        if(gpio == J_button){
            byteArray = 0b111111111111011 & byteArray;
        }
        if(gpio == K_button){
            byteArray = 0b111111111110111 & byteArray;
        }
        if(gpio == L_button){
            byteArray = 0b111111111101111 & byteArray;
        }
        if (gpio == Button_Overload)
        {
            byteArray = 0b111111111011111 & byteArray;
        }
        if (gpio == Button_Reset)
        {
            byteArray = 0b1111101111111111 & byteArray;            
        }
        if (gpio == Bluetooth_State)
        {
            int state = 1;
            xQueueSendToFrontFromISR(xQueueState, &state, 0);
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
         
        if (result < -240) {                 
            byteArray = 0b0000000010000000 | byteArray;
        }

        if (result > 240)
        {
            byteArray = 0b0000000001000000 | byteArray;
        }
        

        if ((result < 230 && result > -230)) {          
            byteArray = 0b1111111101111111 & byteArray;
            byteArray = 0b1111111110111111 & byteArray;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void sound_task(void *p) {
    adc_init();
    adc_gpio_init(ADC_Sound);
    while(true) {
        adc_select_input(ADC_Sound_ID);
        int adc_value = adc_read();
        adc_value = adc_value * 100 / 4095;
        if (adc_value < 5 && adc_value > -1)
        {
            byteArray = 0b0000000100000000 | byteArray;             
        }
        else if (adc_value > 90 && adc_value < 110)
        {
            byteArray = 0b0000001000000000 | byteArray; 
        }
        else
        {
            byteArray = 0b1111110111111111 & byteArray;
            byteArray = 0b1111111011111111 & byteArray;
        }
    }
}

bool timer_0_callback(repeating_timer_t *rt) {
    int bytes = byteArray;
    xQueueSendFromISR(xQueueBTN, &bytes, 0);

    return true; // keep repeating
}

void hc06_task(void *p) {
    //gpio_put(LED1_R, 1);
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("Tumas", "12345");
    //gpio_put(LED1_B, 1);

    int received_bytes;
    char d[2];
    int status = 0;
    repeating_timer_t timer_0;

    if (!add_repeating_timer_us(20000, 
                                timer_0_callback,
                                NULL, 
                                &timer_0)) {
        printf("Failed to add timer\n");
    }
    while (true) {
        if(xQueueReceive(xQueueState,&status, 0)) {
            if (status == 1) {
                gpio_put(LED2_G, 0);
                gpio_put(LED2_B, 1);
            }
        }
        if (status == 0) {
            gpio_put(LED2_G, 1);
            vTaskDelay(pdMS_TO_TICKS(500));
            gpio_put(LED2_G, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        if (xQueueReceive(xQueueBTN,&received_bytes,pdMS_TO_TICKS(100))){
            d[0] = (received_bytes >> 8) & 0xFF; // First 8 bytes
            d[1] = received_bytes & 0xFF;        // Last 8 bytes

            // Send the characters over UART
            uart_putc_raw(HC06_UART_ID, d[0]);
            uart_putc_raw(HC06_UART_ID, d[1]);
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
int main() {
    stdio_init_all();

    xQueueBTN = xQueueCreate(16, sizeof(int));
    xQueueState = xQueueCreate(16, sizeof(int));

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
    gpio_init(LED1_R);
    gpio_set_dir(LED1_R,GPIO_OUT);
    gpio_init(LED1_G);
    gpio_set_dir(LED1_G,GPIO_OUT);
    gpio_init(LED1_B);
    gpio_set_dir(LED1_B,GPIO_OUT);
    gpio_init(LED2_R);
    gpio_set_dir(LED2_R,GPIO_OUT);
    gpio_init(LED2_G);
    gpio_set_dir(LED2_G,GPIO_OUT);
    gpio_init(LED2_B);
    gpio_set_dir(LED2_B,GPIO_OUT);
    gpio_init(Button_Overload);
    gpio_set_dir(Button_Overload,GPIO_IN);
    gpio_pull_up(Button_Overload);
    gpio_init(Button_Reset);
    gpio_set_dir(Button_Reset,GPIO_IN);
    gpio_pull_up(Button_Reset);
    gpio_init(Bluetooth_State);
    gpio_set_dir(Bluetooth_State,GPIO_IN);
    

    xTaskCreate(hc06_task, "UART_Task 1", 4095, NULL, 1, NULL);
    xTaskCreate(y_task, "y_task", 4095, NULL, 1, NULL);
    xTaskCreate(sound_task, "sound_task", 4095, NULL, 1, NULL);
    gpio_set_irq_enabled_with_callback(A_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    gpio_set_irq_enabled(S_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(J_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(K_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(L_button, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(Button_Overload, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(Button_Reset, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    gpio_set_irq_enabled(Bluetooth_State, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true);
    
    vTaskStartScheduler();

    while (true)
        ;
}