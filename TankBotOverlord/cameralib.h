/******************** (C) COPYRIGHT 2012 RadioSHack ********************
 * File Name          : camsd.pde
 * Author             : TRS
 * Version            : V1.0
 * Date               : 28/05/2012
 * Description        : Main program body
 ********************************************************************************
 * THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 * AS A RESULT, RS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *******************************************************************************/

#include "Arduino.h"

#define NORMAL_USE	1

#define VC0706_PROTOCOL_SIGN 			0x56
#define VC0706_SERIAL_NUMBER 			0x00

#define VC0706_COMMAND_RESET			0x26
#define VC0706_COMMAND_GEN_VERSION		0x11
#define VC0706_COMMAND_TV_OUT_CTRL		0x44
#define VC0706_COMMAND_OSD_ADD_CHAR		0x45
#define VC0706_COMMAND_DOWNSIZE_SIZE		0x53
#define VC0706_COMMAND_READ_FBUF		0x32
#define FBUF_CURRENT_FRAME			0
#define FBUF_NEXT_FRAME				0

#define VC0706_COMMAND_FBUF_CTRL		0x36
#define VC0706_COMMAND_COMM_MOTION_CTRL		0x37
#define VC0706_COMMAND_COMM_MOTION_DETECTED	0x39
#define VC0706_COMMAND_POWER_SAVE_CTRL		0x3E
#define VC0706_COMMAND_COLOR_CTRL		0x3C
#define VC0706_COMMAND_MOTION_CTRL		0x42


#define VC0706_COMMAND_WRITE_DATA		0x31
#define VC0706_COMMAND_GET_FBUF_LEN		0x34

#define READ_DATA_BLOCK_NO			56

/*******************************************************************************
 * Function Name  : VC0706_reset
 * Description    : Reset VC0706
 *
 * Input          : None
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_reset();

/*******************************************************************************
 * Function Name  : VC0706_get_version
 * Description    : Request version string from VC0706
 *
 * Input          : None
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_get_version();

/*******************************************************************************
 * Function Name  : VC0706_tv_out_control
 * Description    : stop or start TV output from VC0706
 *
 * Input          : on=0 stop tv output
 * ;			:  on=1 start tv output
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_tv_out_control(int on);

/*******************************************************************************
 * Function Name  : VC0706_osd_add_char
 * Description    : ADD OSD CHARACTERS TO CHANNELS(CHANNEL 1)
 *
 * Input          : col : Display column
 *	`		  row: Display Row
 *			  osd_string : display string (max 14 characters)
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_osd_add_char(int col, int row, String osd_string);

/*******************************************************************************
 * Function Name  : VC0706_w_h_downsize
 * Description    : control width and height downsize attribute
 *
 * Input          : scale_width = 0 1:1
 *                  			      = 1 1:2
 *                  			      = 2 1:4
 * 			  scale_height= 0 1:1
 *                  			      = 1 1:2
 *                  			      = 2 1:4
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_w_h_downsize(int scale_width, int scale_height);

/*******************************************************************************
 * Function Name  : VC0706_read_frame_buffer
 * Description    : read image data from FBUF
 *
 * Input          : buffer_address(4 bytes); buffer_length(4 bytes)
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_read_frame_buffer(unsigned long buffer_address, unsigned long buffer_length);

/*******************************************************************************
 * Function Name  : VC0706_frame_control
 * Description    : control frame buffer register
 *
 * Input          : frame_control=control flag(1byte)
 *			: 		0 = stop current frame ; 1= stop next frame;2=step frame;3 =resume frame;
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_frame_control(byte frame_control);

/*******************************************************************************
 * Function Name  : VC0706_motion_detection
 * Description    : get motion monitoring status in communication interface.
 *
 * Input          : control_flag = 0 stop motion monitoring
 *					      = 1 start motion monitoring
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_motion_detection(int control_flag);

/*******************************************************************************
 * Function Name  : VC0706_motion_control
 * Description    : motion control
 *
 * Input          : control_flag = 0 forbid motion monitoring
 *					      = 1 enable motion monitoring
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_motion_control(int control_flag);

/*******************************************************************************
 * Function Name  : VC0706_get_framebuffer_length
 * Description    : get byte-lengths in FBUF
 *
 * Input          : fbuf_type =current or next frame
 *			            0   =  current frame
 *				     1   =  next frame
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_get_framebuffer_length(byte fbuf_type);

/*******************************************************************************
 * Function Name  : VC0706_uart_power_save
 * Description    : stop current frame for reading
 *
 * Input          : power_on =1  start power-save
 *					   = 0  stop power-save
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_uart_power_save(byte power_save_on);

/*******************************************************************************
 * Function Name  : VC0706_uart_color_control
 * Description    : stop current frame for reading
 *
 * Input          : show_mode = 0  automatically step black-white and colour
 *						   1  manually step color, select colour
 *						   2	manually step color, select black-white
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706_uart_color_control(byte show_mode);

/*******************************************************************************
 * Function Name  : VC0706_compression_ratio
 * Description	  : stop current frame for reading
 *
 * Input		  : ratio		>13(minimum)
 *						<63(max)
 *
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void VC0706_compression_ratio(int ratio);

/*******************************************************************************
 * Function Name  : VC0706_motion_windows_setting
 * Description	  : motion windows setting
 *
 * Input		  : register_address(2 bytes);
 *				data(4 bytes)= data ready to write
 *
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void VC0706_motion_windows_setting(unsigned int register_address, unsigned long data);

/*******************************************************************************
 * Function Name  : buffer_send
 * Description    : Transmit buffer to VC0706
 *
 * Input          : tx_vcbuffer
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void buffer_send();

/*******************************************************************************
 * Function Name  : buffer_read
 * Description    : Receive buffer from VC0706
 *
 * Input          : None
 *
 * Output         : rx_buffer, rx_ready
 * Return         : None
 *******************************************************************************/
void buffer_read();

/*******************************************************************************
 * Function Name  : capture_photo_to_buffer
 * Description	  : capture a photo and repeatedly call a handler with a buffer containing the data
 *
 * Input		  : dataBuf: a buffer to store the data in; dataBufSize: the size of dataBuf; callback: a handler to call when the buffer is filled
 *
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void capture_photo_to_buffer(uint8_t * dataBuf, uint32_t dataBufSize, void(callback*)(uint8_t*));

#endif



