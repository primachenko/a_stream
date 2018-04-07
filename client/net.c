#include "net.h"

int open_socket(net_t * net)
{
    int rc;
    rc = socket(AF_INET, SOCK_DGRAM, 0);
    if (!rc) return -1;
    net->fd = rc;

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr(TX_ADDR);
    my_addr.sin_port = htons(TX_PORT);

    socklen_t addr_len = sizeof(my_addr);
    rc = bind(net->fd, (const struct sockaddr *)&my_addr, addr_len);
    if (0 != rc) return -2;

    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_addr.s_addr = inet_addr(RX_ADDR);
    target_addr.sin_port = htons(RX_PORT);

    memcpy(&net->addr, &target_addr, sizeof(target_addr));

    return 0;
}

int close_socket(net_t * net)
{
    if (net->fd)
    {
        close(net->fd);
    }
    else
    {
        return -1;
    }

    memset(&net->addr, 0, sizeof(net->addr));

    return 0;
}

int send_message(net_t * net, uint8_t flags, void * data, uint16_t data_len)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    header.length = data_len;
    header.flags = flags;

    uint16_t addr_len = sizeof(net->addr);

    rc = sendto(net->fd, (void *) &header, sizeof(header), 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;
    net->byte_counter += sizeof(header);

    if (header.length == 0) return 0;

    rc = sendto(net->fd, (void *) data, header.length, 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;
    net->byte_counter += header.length;
    printf("counter = %lld\n", net->byte_counter);

    return 0;
}

int recv_message(net_t * net, void ** data)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    uint16_t addr_len = sizeof(net->addr);

    rc = recvfrom(net->fd, (void *) &header, sizeof(header_t), 0, &net->addr, (socklen_t *) &addr_len);
    if (!rc) return -1;

    if (header.length == 0) return 0;

    uint8_t * payload = (uint8_t *) malloc(sizeof(message_t) + header.length);
    if (payload == NULL) return -1;

    *data = payload;
    memcpy((void *) payload, (void *) &header, sizeof(header));
    payload += sizeof(header);
    rc = recvfrom(net->fd, payload, header.length, 0, &net->addr, (socklen_t *) &addr_len);
    if (!rc) return -1;


    return 0;
}

/*void *udp_listener(void *arg)
{
  int bytes_recv, mysocket, client_addr_size;
  char buf[ALDENLEN];
  char *addr;
  struct sockaddr_in sockaddr_in;
  struct sockaddr_in client_addr;
  struct sockaddr_in local_addr;
  struct hostent *hst;
  socklen_t fromlen;
  pthread_t thread;

  char *all = ALLOW_SEND;
  char *den = DENY_SEND;
  
  if ((mysocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    error("socket() failed");
  }

  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(MYPORT);
  local_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(mysocket, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
  {
    printf("CLIENT 1 type: search for a free port\n");
    int i = 0;
    while(i < PORTRANGE)
    {
      i++;
      local_addr.sin_port = htons(MYPORT + i);
      if (!bind(mysocket, (struct sockaddr *)&local_addr, sizeof(local_addr)))
      {
           break;
      }
    }
  }

  client_addr_size = sizeof(struct sockaddr_in);

  printf("CLIENT 1 type: listening broadcast\n");

  tcp_send.allow = 0;

  while (1) 
  {
    if ((bytes_recv = recvfrom(mysocket, &buf[0], sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &client_addr_size)) < 0)
        {
            error("recvfrom");
        }
    hst = gethostbyaddr((char *)&client_addr.sin_addr, 4, AF_INET);
    addr = (char*)inet_ntoa(client_addr.sin_addr);
    buf[bytes_recv] = 0;
   
    if(!strcmp(buf, all))
    {
        if(tcp_send.allow == 0)
        {
            pthread_mutex_lock(&tcp_send.mutex);
            tcp_send.allow = 1;
            pthread_mutex_unlock(&tcp_send.mutex);
            pthread_create(&thread, NULL, tcp_sender, (void*)addr);
        }
    }
    if (!strcmp(buf, den))
    { 
        pthread_mutex_lock(&tcp_send.mutex);
        tcp_send.allow = 0;
        pthread_mutex_unlock(&tcp_send.mutex);
        pthread_cancel(thread);
    }
  }

void *udp_broadcast(void *arg)
{
    int sock, n, port, K;
    char *msgf, *msgs, *ip;
    unsigned int length;
    struct sockaddr_in server;
    struct hostent *hp;

    ip = ((char**)arg)[1];
    K = atoi(((char**)arg)[2]);

    printf("SERVER: start with arg: IP = %s, K = %d\n", ip, K);
    fflush(stdout);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        error("socket() failed");
    }
    server.sin_family = AF_INET;
    hp = gethostbyname(ip);

    if (hp == 0) 
    {
        error("gethostbyname() failed");
    }

    bcopy((char *)hp->h_addr_list[0], (char *)&server.sin_addr, hp->h_length);
    length = sizeof(struct sockaddr_in);

    while(1)
    {   
        //вещание для клиенов первого типа
        port = CLIFTPORT;
        if(qpacket.elements < QUEUELEN)
        {
            msgf = ALLOW_SEND;
        } else {
            msgf = DENY_SEND;
        }
        server.sin_port = htons(port);
        for(int i = 0; i < PORTRANGE; i++)
        {
            if ((n = sendto(sock,msgf,ALDENLEN,0,(const struct sockaddr *)&server,length)) < 0) 
            {
                error("sendto() failed");
            }
            server.sin_port = htons(port + i);
        }
        //вещание для клиентов второго типа
        port = CLISTPORT;
        if(qpacket.elements > 0)
        {
            msgs = ALLOW_RECIEV;
        } else {
            msgs = DENY_RECIEV;
        }
        server.sin_port = htons(port);
        for(int i = 0; i < PORTRANGE; i++)
        {
            if ((n = sendto(sock,msgs,ALDENLEN,0,(const struct sockaddr *)&server,length)) < 0) 
            {
                error("sendto() failed");
            }
            server.sin_port = htons(port+i);
        }   
        sleep(K);
    }
}
}*/