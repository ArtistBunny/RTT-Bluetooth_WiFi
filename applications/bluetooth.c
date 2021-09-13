
#include <bluetooth.h>
#include <inoutmsg.h>


#define YGP_BLUETOOTH_DEBUG

static rt_device_t serial;
#define UART_NAME           "uart2"
#define UART_BAUD_RATE      BAUD_RATE_9600
#define UART_DATA_BITS      DATA_BITS_8
#define UART_STOP_BITS      STOP_BITS_1
#define UART_PARITY         PARITY_NONE
#define UART_BUFSZ          512                     //HC-08�ֲ��Ϲ涨һ�����ݰ����ܳ���500���ֽ�

static struct rt_timer  timer1;                     //����֡����ж϶�ʱ��
#define TIMER1_NAME         "T0927"                 //�ڲ���ϵͳ�в�������
#define TIMER1_TIMING       60                      //��ʱ����ʱ

static int charsize = 0;                            //���ڽ��յ����ַ�����ͳ��
#define MSGSIZE              2048                    //512
static char receive_msg[MSGSIZE];
static char send_msg[MSGSIZE];




/* �� ʱ ʱ �� �� �� �� �� ���� */
static void timer1_timeout(void* parameter){
    //������Ҫ�Ż��Ļ���ʹ����Ϣ�ж���+�����߳�
    rt_device_read(serial, 0, receive_msg, charsize);
#ifdef YGP_BLUETOOTH_DEBUG
    rt_kprintf("serial read receive_msg=%s;\n",receive_msg);
#endif /* YGP_BLUETOOTH_DEBUG */

    iom1(receive_msg,MSGSIZE,send_msg,MSGSIZE);
    rt_kprintf("send_msg=%s;\n",send_msg);
    rt_device_write(serial, 0, send_msg, strlen(send_msg));

    //��λ�ַ�ͳ�ƺͽ��ջ�����
    charsize = 0;
    memset(receive_msg,0,MSGSIZE);
}

/* �������ݻص����� */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    charsize++;
    //������ʱ��
    rt_timer_start(&timer1);
    return RT_EOK;
}


int my_bluetooth_up(void){
    /* ����ϵͳ�еĴ����豸 */
    serial = rt_device_find(UART_NAME);
    if (!serial)
    {
#ifdef YGP_BLUETOOTH_DEBUG
        rt_kprintf("find %s failed!\n", UART_NAME);
#endif /* #ifdef YGP_BLUETOOTH_DEBUG */
        return RT_ERROR;
    }
    //���Ĵ��ڲ���
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* ��ʼ�����ò��� */
    //�޸Ĵ������ò���
    config.baud_rate = UART_BAUD_RATE;              //�޸Ĳ�����Ϊ 9600
    config.data_bits = UART_DATA_BITS;              //����λ 8
    config.stop_bits = UART_STOP_BITS;              //ֹͣλ 1
    config.bufsz     = UART_BUFSZ;                  //�޸Ļ����� buff size
    config.parity    = UART_PARITY;                 //����żУ��λ
    //step3�����ƴ����豸��ͨ�����ƽӿڴ�����������֣�����Ʋ���
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    //ʹ�þ�̬�Ķ�ʱ��
    rt_timer_init(&timer1, TIMER1_NAME,                 /* �� ʱ �� �� ���� timer1 */
                  timer1_timeout,                       /* �� ʱ ʱ �� �� �� �� �� ���� */
                    RT_NULL,                            /* �� ʱ �� �� �� �� �� �� �� */
                    TIMER1_TIMING,                      /* �� ʱ �� �ȣ� �� OS Tick Ϊ�� λ�� */ //�����ڼ�10sһ��
                    RT_TIMER_FLAG_ONE_SHOT);            /* �� �� �� ʱ �� */

    /* ���жϽ��ռ���ѯ����ģʽ�򿪴����豸 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* ���ý��ջص����� */
    rt_device_set_rx_indicate(serial, uart_input);

    return RT_EOK;
}
MSH_CMD_EXPORT(my_bluetooth_up, my_bluetooth_up);


void H001(){
    rt_device_write(serial, 0, "ABCDEF", 6);
}
MSH_CMD_EXPORT(H001, H001);
