
#include "drv_rs485.h"
#include "dev_logic.h"



#define RS485_BAUD 19200
#define UART1_BAUD 19200

#define ECHO_TEST_TXD  (17)
#define ECHO_TEST_RXD  (16)

#define ECHO_TEST_RTS  (18)
#define ECHO_TEST_CTS  UART_PIN_NO_CHANGE

#define PACKET_READ_TICS        (10 / portTICK_RATE_MS)

static const char *TAG = "uart_events";

// Buffer decleration, and the last written place in it
volatile uint8_t rs485Buffer[RS485_BUFFER_SIZE];
volatile uint16_t rs485BufferPosition;
volatile uint16_t CopyRs485BufferPosition;
const int uart_num = UART_NUM_2;
static intr_handle_t handle_console;

const int uart_num1 = UART_NUM_1;
#define UART1_TXD  UART_PIN_NO_CHANGE
#define UART1_RXD  (35)

uint8_t send_buff_index = 0;
volatile uint8_t send_tx_index;
uint8_t send_buff[500];

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;



//extern uint8_t protocolBuffer[RS485_BUFFER_SIZE];

static QueueHandle_t uart2_queue;


void stop_send_function()
{

	uart_set_rts(uart_num, 1);
	uart_enable_tx_intr(uart_num, 0, 100);
}

// Send a single character to the RS-485 line
void rs485_send_char(uint8_t * ch)
{

	send_buff[send_buff_index++] = ch;
//	ets_printf("SendChar %d \n",ch);

	/*
	//Serial.write(ch);
    uart_set_rts(uart_num, 0);
    //vTaskDelay(10 / portTICK_PERIOD_MS);
	uart_write_bytes(uart_num, (const char *)&ch, lan);

	vTaskDelay(10 / portTICK_PERIOD_MS);
	uart_set_rts(uart_num, 1);
	*/
}

int rval_led = 0;
void rs485_send_data()
{
	//Serial.write(ch);
	//uart_set_rts(uart_num, 0);
	//uart_disable_rx_intr(uart_num);

	//uart_enable_tx_intr(uart_num, 1, 120);
	rval_led = rval_led == 0 ? 1 : 0;
	WriteToDigital(OUT_LED_R_PIN,rval_led);
	uart_write_bytes(uart_num, &send_buff, send_buff_index);
	memset(send_buff,0,200);
	send_buff_index = 0;
	//send_tx_index = 0;
	//vTaskDelay(30 / portTICK_PERIOD_MS);

	//uart_set_rts(uart_num, 1);


}


/**
Arduino's Serial Event interrupt
@param
empty
@return
void
*/

volatile uint8_t recivedChar;
volatile bool rx_esc;

volatile bool startReceived;
#ifdef DEBUG
extern uint8_t msgLentgh;
#endif







void serialEvent(uint8_t  recivedChar)
{

	if (rs485BufferPosition < RS485_BUFFER_SIZE)
	{
		//recivedChar = Serial.read();  // read pending byte
		switch (recivedChar) 
		{
			case  SLIP_START:
				startReceived = 1;
				rs485BufferPosition = 0;
				rx_esc = 0;
				break;

			case  SLIP_END:
				if(startReceived)
				{
					startReceived = 0;
					//CopyRs485BufferPosition = rs485BufferPosition;
					reciveValidMsg((uint8_t *)rs485Buffer, rs485BufferPosition);
					rs485BufferPosition = 0;
				}

				break;
			
			case SLIP_ESC:
				rx_esc = 1;
				break;
			
			default:
				if(startReceived)
				{
					if(rx_esc)
					{
						rs485Buffer[rs485BufferPosition++] = SLIP_ESC + recivedChar;
						rx_esc = 0;
					}
					else
					{
						rs485Buffer[rs485BufferPosition++] = recivedChar;
					}
				}
		}
	}
	else
	{
		rs485BufferPosition = 0;
	}
}


int gval_led=0;
static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    int i,len;
    while(1) {
		//Read data from UART
		int len = uart_read_bytes(uart_num, dtmp, BUF_SIZE, PACKET_READ_TICS);

		//Write data back to UART
		if (len > 0) {
			for (i = 0; i < len; ++i) {
				//ets_printf("in serial  %d \n",dtmp[i]);
				serialEvent(dtmp[i]);
				gval_led = gval_led == 0 ? 1 : 0;
				WriteToDigital(OUT_LED_G_PIN,gval_led);
			}
		}

		/*
		int len1 = uart_read_bytes(uart_num1, dtmp, BUF_SIZE, PACKET_READ_TICS);

		//Write data back to UART
		if (len1 > 0) {
			for (i = 0; i < len1; ++i) {
				ets_printf("in serial  %d \n",dtmp[i]);
			}

		}
*/
		}



    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}



static void IRAM_ATTR uart_intr_handle(void *arg)
{
  uint32_t rx_fifo_len, status,tx_fifo_len;


  status = UART1.int_st.val; // read UART interrupt Status
  rx_fifo_len = UART1.status.rxfifo_cnt; // read number of bytes in UART buffer
  tx_fifo_len = UART1.status.txfifo_cnt;
  //ets_printf("in 1 serial intr_handle  %d -> %d \n",status,tx_fifo_len);

  uint8_t rxbuf;
  while(rx_fifo_len)
  {

		rxbuf = UART1.fifo.rw_byte; // read all bytes
		//uart_write_bytes(uart_num, (const char*) senddata, sizeof(senddata));
		//uart_write_bytes(uart_num, (const char *)rxbuf, 1);
		//ets_printf("in serial event %d \n",rxbuf);
		//serialEvent(rxbuf);
		rx_fifo_len--;
  }
  // after reading bytes from buffer clear UART interrupt status
 // ets_printf("in 1 serial intr_handle  %d \n",status);
  uart_flush_input(uart_num1);
  uart_clear_intr_status(uart_num1, UART_AT_CMD_CHAR_DET_INT_CLR);

 // ets_printf("in 2 serial intr_handle %d \n",status);
}



void UART1_setup(void)
{
	//Serial.begin(RS485_BAUD);


	uart_config_t uart_config = {
	    .baud_rate = UART1_BAUD,
	    .data_bits = UART_DATA_8_BITS,
	    .parity = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_2,
	    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	    .rx_flow_ctrl_thresh = 122,
	};
	// Configure UART parameters
	ESP_ERROR_CHECK(uart_param_config(uart_num1, &uart_config));


	// Set UART pins(TX: (13) (UART0 default), RX: (12) (UART0 default), RTS: IO22, CTS: IO19)
	ESP_ERROR_CHECK(uart_set_pin(uart_num1, -1, UART1_RXD, -1, -1));


	// Setup UART buffered IO with event queue
	const int uart_buffer_size = (1024 * 2);

	// Install UART driver using an event queue here
	ESP_ERROR_CHECK(uart_driver_install(uart_num1, uart_buffer_size, uart_buffer_size, 20,uart0_queue , 0)); //NULL ->&uart2_queue

	//set Interrupts
	// release the pre registered UART handler/subroutine
	//ESP_ERROR_CHECK(uart_isr_free(uart_num1));

	// register new UART subroutine
	//ESP_ERROR_CHECK(uart_isr_register(uart_num1,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));
	//printf("register UART \n");

	// enable RX interrupt
	//ESP_ERROR_CHECK(uart_enable_rx_intr(uart_num1));





}

/**
Initializing the RS-485 driver
@param
empty
@return
void
*/
void rs485_setup(void)
{
	//Serial.begin(RS485_BAUD);


	uart_config_t uart_config = {
	    .baud_rate = RS485_BAUD,
	    .data_bits = UART_DATA_8_BITS,
	    .parity = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_1,
	    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	    .rx_flow_ctrl_thresh = 122,
	};
	// Configure UART parameters
	ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));


	// Set UART pins(TX: (13) (UART0 default), RX: (12) (UART0 default), RTS: IO22, CTS: IO19)
	ESP_ERROR_CHECK(uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));


	// Setup UART buffered IO with event queue
	const int uart_buffer_size = (1024 * 2);

	// Install UART driver using an event queue here
	ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 20,uart0_queue , 0)); //NULL ->&uart2_queue

	//set Interrupts
	// release the pre registered UART handler/subroutine
	//ESP_ERROR_CHECK(uart_isr_free(uart_num));

	// register new UART subroutine
	//ESP_ERROR_CHECK(uart_isr_register(uart_num,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));
	//printf("register UART \n");

	// enable RX interrupt
	//ESP_ERROR_CHECK(uart_enable_rx_intr(uart_num));

	// Set RS485 half duplex mode
	ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));

	//UART1_setup();

	//Create a task to handler UART event from ISR
	xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

	//ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 100)); // wait timeout is 100 RTOS ticks (TickType_t)

	//uart_set_rts(uart_num, 1);

	//uart_enable_tx_intr(uart_num, 1, 2000);
	// uart_set_rts(uart_num, 1);

	//  Serial.begin(RS485_BAUD, SERIAL_8N2);
	rs485BufferPosition = 0;
}





