#define MAX_BUFFERSIZE 128 //自定义的缓冲buff长度
char uart_Receive_Buff [MAX_BUFFERSIZE]; //缓冲buff
unsigned int uart_Receive_Length = 0; //接收字节长度
unsigned long preUartTime = 0; //记录读取最好一个字节的时间点
unsigned int frame_Length = 5; //数据长度
unsigned int frame_Check_Begin = 1; //校验起始位
unsigned int frame_Inst_One = 2; //指令1
unsigned int frame_Inst_Two = 3; //指令2
unsigned int frame_Check_Length = 3; //校验长度
unsigned int frame_state = 0; //校验指令
unsigned int frame_inst1 = 0; //校验指令1
unsigned int frame_inst2 = 0; //校验指令2
bool task1_flag = false; //任务1标志位
bool task2_flag = false; //任务2标志位
bool task3_flag = false; //任务3标志位
 
/*
  校验函数
*/
char checksum(char *p, int len)
{
  char sum = 0;
  while(len--)
  {
    sum += *p;
    p++;
  }
  return sum; 
}
/*
  串口发送函数
*/
void sendUART(const char *p, size_t len) {
  Serial.write(p, len);
}
/*
  预处理串口数据函数
*/
void uartDataProcess(char *p , int len) 
{
  if ((len != frame_Length)&&(p != 0x34))    //不是目标长度
  {
    char testframebuf[1] = {0xFF};
    sendUART(testframebuf, 1);
  }
  else
  {
    if(checksum(&p[frame_Check_Begin],frame_Check_Length)!=p[len-1])
    { 
      char testframebuf1[2] = {0xFF ,0xFF};
      sendUART(testframebuf1, 2);
    }
    else
    {
      Serial.write(p, len);
      instructDataProcess(p , len);
    }
  }
}
/*
  指令处理函数
*/
void instructDataProcess(char *p , int len) 
{
  frame_state = p[frame_Check_Begin];
  frame_inst1 = p[frame_Inst_One];
  frame_inst2 = p[frame_Inst_Two];
  if(frame_state == 1)
  {
    if(frame_inst1 == 1)//0x34,0x01,0x01
    {
        char testframebuf2[3] = {0x01,0x01,0x00};
        sendUART(testframebuf2, 3);
        task1_flag = false;
    }
    else if(frame_inst1 == 2)//0x34,0x01,0x02
    {
        char testframebuf3[3] = {0x01,0x02,0x00};
        sendUART(testframebuf3, 3);
        task2_flag = false;
    }
    else if(frame_inst1 == 3)//0x34,0x01,0x03
    {
        char testframebuf4[3] = {0x01,0x03,0x00};
        sendUART(testframebuf4, 3);
        task3_flag = false;
    }
  }
  else if(frame_state == 2)//0x34,0x02,0x01
  {
    if(frame_inst1 == 1)
    {
        char testframebuf5[3] = {0x02,0x01,0x01};
        sendUART(testframebuf5, 3);
        task1_flag = true;
    }
    else if(frame_inst1 == 2)//0x34,0x02,0x02
    {
        char testframebuf6[3] = {0x02,0x02,0x01};
        sendUART(testframebuf6, 3);
        task2_flag = true;
    }
    else if(frame_inst1 == 3)//0x34,0x02,0x03
    {
        char testframebuf7[3] = {0x02,0x03,0x01};
        sendUART(testframebuf7, 3);
        task3_flag = true;
    } 
  }  
}
/*
  指令处理函数
*/
void task1(void)
{
  char testframebuf8[3] = {0x11,0x11,0x11};
  sendUART(testframebuf8, 3);
}
 
/*
  指令处理函数
*/
void task2(void)
{
  char testframebuf9[3] = {0x22,0x22,0x22};
  sendUART(testframebuf9, 3);
}
 
/*
  指令处理函数
*/
void task3(void)
{
  char testframebuf10[3] = {0x33,0x33,0x33};
  sendUART(testframebuf10, 3);
}
 
/*
  串口数据接受进程
*/
void doUartReceiveInit()
{
  if (Serial.available())
  { 
    uart_Receive_Buff [uart_Receive_Length++] = Serial.read();//读取串口数据
    preUartTime = millis(); //读取接收数据时间
    if (uart_Receive_Length  >= MAX_BUFFERSIZE-1) 
    { 
      uart_Receive_Length  = MAX_BUFFERSIZE-2;
      preUartTime = preUartTime - 200;
    }
  }
  if (uart_Receive_Length  > 0 && (millis() - preUartTime >= 100))  //最后一个字节后超过100ms数据处理
  { 
    uart_Receive_Buff [uart_Receive_Length] = 0x00;//处理串口数据
    Serial.flush();
    uartDataProcess(uart_Receive_Buff, uart_Receive_Length);//执行串口数据处理函数
    uart_Receive_Length = 0;
  }
}
/*
  处理串口数据函数，可以在函数内编写处理函数数据的程序
*/
void setup() {
  Serial.begin(115200);
}
/*
  处理串口数据函数，可以在函数内编写处理函数数据的程序
*/
void loop()
{
  doUartReceiveInit();  //调用串口数据处理进程
  if(task1_flag == true)
  {
    task1();
  }
  else if(task2_flag == true)
  {
    task2();
  }
  else if(task3_flag == true)
  {
    task3();
  }
}
