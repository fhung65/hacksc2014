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
#include "cameralib.h"

void VC0706::VC0706()
{
    frame_length = 0;
    vc_frame_address = 0;
    last_data_length = 0;
}

/*******************************************************************************
 * Function Name  : reset
 * Description    : Reset VC0706
 *
 * Input          : None
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::reset()
{
    tx_buffer[0] = PROTOCOL_SIGN;
    tx_buffer[1] = SERIAL_NUMBER;
    tx_buffer[2] = COMMAND_RESET;
    tx_buffer[3] = 0x00;

    tx_counter = 4;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : get_version
 * Description    : Request version string from VC0706
 *
 * Input          : None
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::get_version()
{
    tx_buffer[0] = PROTOCOL_SIGN;
    tx_buffer[1] = SERIAL_NUMBER;
    tx_buffer[2] = COMMAND_GEN_VERSION;
    tx_buffer[3] = 0x00;

    tx_counter = 4;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : tv_out_control
 * Description    : stop or start TV output from VC0706
 *
 * Input          : on=0 stop tv output
 * ;			:  on=1 start tv output
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::tv_out_control(uint8_t on)
{
    tx_buffer[0] = PROTOCOL_SIGN;
    tx_buffer[1] = SERIAL_NUMBER;
    tx_buffer[2] = COMMAND_TV_OUT_CTRL;
    tx_buffer[3] = 0x01;
    tx_buffer[4] = on;
    tx_counter = 5;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : osd_add_char
 * Description    : ADD OSD CHARACTERS TO CHANNELS(CHANNEL 1)
 *
 * Input          : col : Display column
 *	`		  row: Display Row
 *			  osd_string : display string (max 14 characters)
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::osd_add_char(uint8_t col, uint8_t row, String osd_string)
{
    uint8_t col_row;
    int string_length;
    int i;

    col &= 0x0f;
    row &= 0x0f;
    col_row=(uint8_t)(col << 4 | row);

    string_length = osd_string.length();
    if (string_length > 14)
        string_length = 14;		// max 14 osd characters

    tx_buffer[0] = PROTOCOL_SIGN;
    tx_buffer[1] = SERIAL_NUMBER;
    tx_buffer[2] = COMMAND_OSD_ADD_CHAR;
    tx_buffer[3] = string_length + 2;
    tx_buffer[4] = string_length;		// character number
    tx_buffer[5] = col_row;

    for (i = 0; i < string_length; i++)
    {
        tx_buffer[i+6]=osd_string.charAt(i);
    }

    tx_counter=string_length+6;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : w_h_downsize
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
void VC0706::w_h_downsize(uint8_t scale_width, uint8_t scale_height)
{
    uint8_t scale;

    if (scale_width>=2)	scale_width=2;
    if (scale_height>scale_width)	scale_height=scale_width;
    scale=(uint8_t)(scale_height<<2 | scale_width);

    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_DOWNSIZE_SIZE;
    tx_buffer[3]=0x01;

    tx_buffer[4]=scale;		//bit[1:0] width zooming proportion
    //bit[3:2] height zooming proportion

    tx_counter=5;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : read_frame_buffer
 * Description    : read image data from FBUF
 *
 * Input          : buffer_address(4 bytes); buffer_length(4 bytes)
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::read_frame_buffer(uint64_t buffer_address, uint64_t buffer_length)
{

    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_READ_FBUF;
    tx_buffer[3]=0x0c;
    tx_buffer[4]=FBUF_CURRENT_FRAME;
    tx_buffer[5]=0x0a;		// 0x0a=data transfer by MCU mode; 0x0f=data transfer by SPI interface
    tx_buffer[6]=buffer_address>>24;			//starting address
    tx_buffer[7]=buffer_address>>16;
    tx_buffer[8]=buffer_address>>8;
    tx_buffer[9]=buffer_address&0x0ff;

    tx_buffer[10]=buffer_length>>24;		// data length
    tx_buffer[11]=buffer_length>>16;
    tx_buffer[12]=buffer_length>>8;
    tx_buffer[13]=buffer_length&0x0ff;
    tx_buffer[14]=0x00;		// delay time
    tx_buffer[15]=0x0a;


    tx_counter=16;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : frame_control
 * Description    : control frame buffer register
 *
 * Input          : frame_control=control flag(1byte)
 *			: 		0 = stop current frame ; 1= stop next frame;2=step frame;3 =resume frame;
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::frame_control(uint8_t frame_control)
{
    if(frame_control>3)frame_control=3;
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_FBUF_CTRL;
    tx_buffer[3]=0x01;
    tx_buffer[4]=frame_control;
    tx_counter=5;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : motion_detection
 * Description    : get motion monitoring status in communication interface.
 *
 * Input          : control_flag = 0 stop motion monitoring
 *					      = 1 start motion monitoring
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::motion_detection(uint8_t control_flag)
{
    if(control_flag>1)control_flag=1;
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_COMM_MOTION_CTRL;
    tx_buffer[3]=0x01;
    tx_buffer[4]=control_flag;
    tx_counter=5;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : motion_control
 * Description    : motion control
 *
 * Input          : control_flag = 0 forbid motion monitoring
 *					      = 1 enable motion monitoring
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::motion_control(uint8_t control_flag)
{
    if(control_flag>1)control_flag=1;
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_MOTION_CTRL;
    tx_buffer[3]=0x03;
    tx_buffer[4]=0x00;			//motion control attribute
    tx_buffer[5]=0x01;			//mcu uart control
    tx_buffer[6]=control_flag;
    tx_counter=7;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : get_framebuffer_length
 * Description    : get byte-lengths in FBUF
 *
 * Input          : fbuf_type =current or next frame
 *			            0   =  current frame
 *				     1   =  next frame
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::get_framebuffer_length(uint8_t fbuf_type)
{
    if(fbuf_type>1)fbuf_type=1;
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_GET_FBUF_LEN;
    tx_buffer[3]=0x01;
    tx_buffer[4]=fbuf_type;
    tx_counter=5;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : uart_power_save
 * Description    : stop current frame for reading
 *
 * Input          : power_on =1  start power-save
 *					   = 0  stop power-save
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::uart_power_save(uint8_t power_save_on)
{
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_POWER_SAVE_CTRL;
    tx_buffer[3]=0x03;
    tx_buffer[4]=00;			//power save control mode
    tx_buffer[5]=01;			// control by UART
    tx_buffer[6]=power_save_on;			//start power save
    tx_counter=7;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : uart_color_control
 * Description    : stop current frame for reading
 *
 * Input          : show_mode = 0  automatically step black-white and colour
 *						   1  manually step color, select colour
 *						   2	manually step color, select black-white
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::uart_color_control(uint8_t show_mode)
{
    if(show_mode>2) show_mode=2;
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_COLOR_CTRL;
    tx_buffer[3]=0x02;
    tx_buffer[4]=01;		//control by UART
    tx_buffer[5]=show_mode;		// automatically step black-white and colour
    tx_counter=6;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : compression_ratio
 * Description	  : stop current frame for reading
 *
 * Input		  : ratio		>13(minimum)
 *						<63(max)
 *
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void VC0706::compression_ratio(int16_t ratio)
{
    if(ratio>63)ratio=63;
    if(ratio<13)ratio=13;
    int16_t vc_comp_ratio=(ratio-13)*4+53;
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_WRITE_DATA;
    tx_buffer[3]=0x05;
    tx_buffer[4]=01;		//chip register
    tx_buffer[5]=0x01;	//data num ready to write
    tx_buffer[6]=0x12;	//register address
    tx_buffer[7]=0x04;
    tx_buffer[8]=vc_comp_ratio; //data

    tx_counter=9;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : motion_windows_setting
 * Description	  : motion windows setting
 *
 * Input		  : register_address(2 bytes);
 *				data(4 bytes)= data ready to write
 *
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void VC0706::motion_windows_setting(uint32_t register_address, uint64_t data)
{
    tx_buffer[0]=PROTOCOL_SIGN;
    tx_buffer[1]=SERIAL_NUMBER;
    tx_buffer[2]=COMMAND_WRITE_DATA;
    tx_buffer[3]=0x08;
    tx_buffer[4]=01;		//chip register
    tx_buffer[5]=0x04;	//data num ready to write
    tx_buffer[6]=register_address>>8;	//register address
    tx_buffer[7]=register_address&0x0ff;

    tx_buffer[8]=data>>24;		// data ready to write
    tx_buffer[9]=data>>16;
    tx_buffer[10]=data>>8;
    tx_buffer[11]=data&0x0ff;

    tx_counter=12;

    buffer_send();
}

/*******************************************************************************
 * Function Name  : buffer_send
 * Description    : Transmit buffer to VC0706
 *
 * Input          : tx_vcbuffer
 *
 * Output         : None
 * Return         : None
 *******************************************************************************/
void VC0706::buffer_send()
{
    uint8_t i=0;

    for (i=0; i<tx_counter; i++)
        Serial.write(tx_buffer[i]);

    tx_ready=true;
}

/*******************************************************************************
 * Function Name  : buffer_read
 * Description    : Receive buffer from VC0706
 *
 * Input          : None
 *
 * Output         : rx_buffer, rx_ready
 * Return         : None
 *******************************************************************************/
void VC0706::buffer_read(uint8_t * buffer)
{
    bool validity=true;

    if (rx_ready)			// if something unread in buffer, just quit
        return;

    rx_counter=0;
    buffer[0]=0;
    while (Serial.available() > 0)
    {
        buffer[rx_counter++]= Serial.read();
        //delay(1);
    }

    if (buffer[0]!=0x76)
        validity=false;
    if (buffer[1]!=SERIAL_NUMBER)
        validity=false;

    if (validity) rx_ready=true;
}

/*******************************************************************************
 * Function Name  : capture_photo_to_buffer
 * Description	  : capture a photo and repeatedly call a handler with a buffer containing the data
 *
 * Input		  : dataBuf: a buffer to store the data in; dataBufSize: the size of dataBuf; callback: a handler to call when the buffer is filled
 *
 * Output		  : None
 * Return		  : None
 *******************************************************************************/
void VC0706::capture_photo_to_buffer(uint8_t * dataBuf, uint32_t dataBufSize, void(callback*)(uint8_t*, uint32_t))
{
    // Set compression ratio
    compression_ratio(63);
    delay(100);

    frame_control(3);
    delay(10);

    frame_control(0);
    delay(10);

    // Update state machine
    rx_ready=false;
    rx_counter=0;

    //get frame buffer length
    get_framebuffer_length(0);
    delay(10);
    buffer_read();

    // store frame buffer length for coming reading
    frame_length=(rx_buffer[5]<<8)+rx_buffer[6];
    frame_length<<=16;
    frame_length+=(0x0ff00&(rx_buffer[7]<<8))+rx_buffer[8];

    vc_frame_address =dataBufSize;

    while(vc_frame_address<frame_length)
    {
        read_frame_buffer(vc_frame_address-dataBufSize, dataBufSize);
        delay(9);

        //get the data with length=READ_DATA_BLOCK_NObytes
        rx_ready=false;
        rx_counter=0;
        buffer_read();

        // write data to temp.jpg
        memcpy(rx_buffer+5, dataBuf, dataBufSize);
        callback(dataBuf, dataBufSize);

        //read next READ_DATA_BLOCK_NO bytes from frame buffer
        vc_frame_address+=dataBufSize;

    }

    // get the last data
    vc_frame_address-=dataBufSize;

    last_data_length=frame_length-vc_frame_address;

    read_frame_buffer(vc_frame_address,last_data_length);
    delay(9);
    //get the data
    rx_ready=false;
    rx_counter=0;
    buffer_read();

    memcpy(rx_buffer+5, dataBuf, last_data_length);
    callback(dataBuf, last_data_length);
}


