#include <inoutmsg.h>

#include <wifi.h>
#include <arpa/inet.h>
#include <netdev.h>       /* ����Ҫ���������ǣ���Ҫ����������ͷ�ļ� */

#include <rtthread.h>
#include <sys/socket.h> /* ʹ��BSD socket����Ҫ����socket.hͷ�ļ� */
#include <netdb.h>
#include <string.h>
#include <finsh.h>
#define BUFSZ   1024
char send_data[BUFSZ] = {0}; /* �����õ������� */
char recv_data[BUFSZ] = {0};

struct rt_semaphore sem1;       //����sockt���������߳�
void tcpclient()
{
    //�ȴ���������IP
    struct netdev *netdev = RT_NULL;
    netdev = netdev_get_by_name("esp0");
    while(1){
        if (netdev->ip_addr.addr>0) {
            break;
        }else {
            rt_thread_mdelay(2000);
        }
    }
    //rt_thread_mdelay(2000);
    rt_kprintf("Start connecting to WiFi server...\n");

    int ret;
    struct hostent *host;
    int sock, bytes_received;
    struct sockaddr_in server_addr;
    const char *url;
    int port;

    url = SERVER_IP;
    port = SERVER_PORT;
    /* ͨ��������ڲ���url���host��ַ��������������������������� */
    host = gethostbyname(url);

    /* ����һ��socket��������SOCKET_STREAM��TCP���� */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* ����socketʧ�� */
        rt_kprintf("Socket error\n");
        goto __end;
        return;
    }
    /* ��ʼ��Ԥ���ӵķ���˵�ַ */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    /* ���ӵ������ */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* ����ʧ�� */
        rt_kprintf("WIFI server Connect fail!\n");
        closesocket(sock);
        goto __end;
        return;
    }
    rt_kprintf("WIFI server Connect success!\n");
    while (1)
    {
        /* ��sock�����н������BUFSZ - 1�ֽ����� */
        bytes_received = recv(sock, recv_data, BUFSZ - 1, 0);
        if (bytes_received < 0)
        {
            /* ����ʧ�ܣ��ر�������� */
            closesocket(sock);
            rt_kprintf("\nreceived error,close the socket.\r\n");

            break;
        }
        else if (bytes_received == 0)
        {
            /* Ĭ�� recv Ϊ����ģʽ����ʱ�յ�0��Ϊ���ӳ����ر�������� */
            closesocket(sock);
            rt_kprintf("\nreceived error,close the socket.\r\n");
            break;
        }
        /* �н��յ����ݣ���ĩ������ */
        recv_data[bytes_received] = '\0';
        /* �ڿ����ն���ʾ�յ������� */
        rt_kprintf("\nReceived data =%s;\n", recv_data);
        iom1(recv_data,BUFSZ,send_data,BUFSZ);
        /* �������ݵ�sock���� */
        ret = send(sock, send_data, strlen(send_data), 0);
        if (ret < 0)
        {
            /* ����ʧ�ܣ��ر�������� */
            closesocket(sock);
            rt_kprintf("\nsend error,close the socket.\r\n");
            break;
        }
        else if (ret == 0)
        {
            /* ��ӡsend��������ֵΪ0�ľ�����Ϣ */
            rt_kprintf("\n Send warning,send function return 0.\r\n");
        }
    }
    goto __end;
    __end:
    rt_hw_cpu_reset();
    rt_sem_release(&sem1);

    return;
}
void wific2();
rt_thread_t twific = RT_NULL;
void my_wifi_up(void){
    rt_sem_init(&sem1,"sem1",0,RT_IPC_FLAG_FIFO);

    rt_thread_mdelay(2000);

    twific = rt_thread_create("wific", tcpclient, RT_NULL, 2048, 10, 20);
    if (twific != RT_NULL)
    {
        rt_thread_startup(twific);
        rt_kprintf("tcpclient up!\n");
    }else {
        rt_kprintf("tcpclient ERROR!\n");
    }

    rt_thread_t twific2 = RT_NULL;

    twific2 = rt_thread_create("wific2", wific2, RT_NULL, 1024, 10, 20);
    if (twific2 != RT_NULL)
    {
        rt_thread_startup(twific2);
        rt_kprintf("wific2 up!\n");
    }else {
        rt_kprintf("wific2 ERROR!\n");
    }
}

int errorcount=0;
void wific2(){
    while(1){
        rt_sem_take(&sem1, RT_WAITING_FOREVER);
        errorcount++;
        if (errorcount==5) {
            rt_hw_cpu_reset();
        }
        //ɾ���߳�
        rt_thread_delete(twific);

        //����ǰ��
        twific = rt_thread_create("wific", tcpclient, RT_NULL, 2048, 10, 20);
        if (twific != RT_NULL)
        {
            rt_thread_startup(twific);
            rt_kprintf("tcpclient up!\n");
        }else {
            rt_kprintf("tcpclient ERROR!\n");
        }

        rt_thread_mdelay(2000);
    }
}


