# APS 2 Computação Embarcada: Controle de Guitar Hero

Este projeto demonstra a interação entre botões, sensores analógicos e um módulo Bluetooth HC06 utilizando o sistema operacional FreeRTOS em uma placa Pico. O código é organizado em tasks e utiliza filas para comunicação entre elas.

### Funcionalidades

- Leitura do estado de 5 botões (A, S, J, K, L)
- Leitura de 2 sensores analógicos (ADC_y e ADC_Sound)
- Detecção de pressão longa e curta nos botões e sensores
- Envio dos dados combinados dos botões e sensores através do módulo Bluetooth HC06
- Indicação visual do estado da conexão Bluetooth (LED azul aceso)

### Componentes Utilizados

- **Microcontrolador:** Pico (RP2040)
- **Botões:** 5 botões (A, S, J, K, L)
- **Sensores Analógicos:** 2 sensores analógicos (ADC_y e ADC_Sound)
- **Módulo Bluetooth:** HC06
- **LEDs:** 2 LEDs (vermelho e azul)

### Descrição dos Componentes

- **Botões:** Os botões são conectados como entradas digitais (GPIO) com resistores pull-up para garantir nível lógico alto na ausência de pressão. A interrupção por borda é utilizada para detectar o pressionamento e liberação dos botões.
- **Sensores Analógicos:** Os sensores analógicos são conectados a pinos ADC (conversor analógico-digital) do microcontrolador. A função `adc_read` converte a tensão lida do sensor em um valor digital.
- **Módulo Bluetooth HC06:** O módulo Bluetooth HC06 é configurado para comunicação serial com a placa Pico. A task `hc06_task` é responsável por enviar os dados combinados dos botões e sensores através da UART.
- **LEDs:** Um LED vermelho é utilizado para fins de debugging e os LEDs vermelho e verde são utilizados para indicar o estado da conexão Bluetooth (azul aceso quando conectado).

### Descrição das Tasks

- **`hc06_task`:** Esta task inicializa a comunicação serial com o módulo Bluetooth HC06, recebe dados da fila `xQueueState` para atualizar o LED de status da conexão e lê as filas `xQueueBTNSet` e `xQueueBTNClear` para empacotar os dados dos botões e sensores. Por fim, envia os dados empacotados através da UART.
- **`y_task`:** Esta task lê o sensor analógico `ADC_y`, detecta pressão longa e curta e envia o dado codificado para a fila `xQueueBTNSet`.
- **`sound_task`:** Esta task lê o sensor analógico `ADC_Sound`, detecta pressão longa e curta e envia o dado codificado para a fila `xQueueBTNSet`.
- **`btn_callback`:** Esta função de callback é chamada por interrupção sempre que ocorre uma mudança de estado (pressão ou liberação) em algum dos botões. O estado do botão pressionado é codificado em um byte e enviado para a fila adequada (`xQueueBTNSet` para pressão ou `xQueueBTNClear` para liberação).

### Descrição das Filas

- **`xQueueBTNSet`:** Esta fila é utilizada para enviar o código do botão pressionado indicando pressão longa ou curta.
- **`xQueueBTNClear`:** Esta fila é utilizada para enviar o código do botão liberado, indicando o fim da pressão.
- **`xQueueState`:** Esta fila é utilizada para enviar o estado da conexão Bluetooth (conectado ou desconectado) para a task `hc06_task` atualizar o LED de status.
