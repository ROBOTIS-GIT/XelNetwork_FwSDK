/*
 * ap_ethernet.c
 *
 *  Created on: 2017. 10. 20.
 *      Author: opus
 */

#include <stdlib.h>
#include <stdio.h>
#include "ap_ethernet.h"


#ifdef _USE_HW_ETH
#ifdef _USE_HW_RTOS

#define MAX_DHCP_TRIES  4
uint8_t DHCP_state = DHCP_OFF;

struct netif gnetif; /* network interface structure */
static ip_addr_t local_ip_addr;
static ip_addr_t local_subnet;
static ip_addr_t local_gateway;
static ip_addr_t eth_dns_server;
static uint8_t ethernet_init_state = FALSE;

/* Private function prototypes -----------------------------------------------*/
static void ethernetHwInit();
static void ethernetDHCPThread(void const * argument);
static void ethernetDHCPMaintain(void);
#ifdef _USE_HW_CMDIF_ETH
static int apEthernetCmdif(int argc, char **argv);
#endif

/* Private function definition -----------------------------------------------*/
uint8_t ethernetIfIsInit(void){
  return ethernet_init_state;
}

uint8_t ethernetGetDhcpStatus(void)
{
  return DHCP_state;
}

void ethernetIfBegin(ip_assign_type_t dhcp_en, uint8_t* p_mac_addr, ip_addr_t* ip_addr, ip_addr_t* subnet,
    ip_addr_t* gateway, ip_addr_t* dns_server)
{
  osThreadId ret;
  uint8_t iptxt[20];

  memcpy(gnetif.hwaddr, p_mac_addr, ETH_HWADDR_LEN);
  memcpy(&local_ip_addr, ip_addr, sizeof(ip_addr_t));
  memcpy(&local_subnet, subnet, sizeof(ip_addr_t));
  memcpy(&local_gateway, gateway, sizeof(ip_addr_t));
  memcpy(&eth_dns_server, dns_server, sizeof(ip_addr_t));

  // Create tcp_ip stack thread with RTOS
  tcpip_init( NULL, NULL);

  netif_add(&gnetif, ip_addr, subnet, gateway, NULL, &lwipHwInit, &tcpip_input);
  dns_setserver(0, &eth_dns_server);

  // lwip hardware initialize
  ethernetHwInit();

  if (dhcp_en == IP_DHCP)
  {
    if (netif_is_up(&gnetif))
    {
      DHCP_state = DHCP_START;
    }
    else
    {
      DHCP_state = DHCP_LINK_DOWN;
      USBD_UsrLog("[ETH] Cable is not connected");
    }

    osThreadDef(DHCP, ethernetDHCPThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
    ret = osThreadCreate(osThread(DHCP), &gnetif);
    if (ret == NULL)
    {
      USBD_UsrLog("osThreadCreate : threadEthernet fail\n");
    }
  }
  else{
    sprintf((char *) iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *) &gnetif.ip_addr));
    USBD_UsrLog("[ETH] Static IP address: %s\n", iptxt);
  }
  ethernet_init_state = TRUE;
}

uint8_t* ethernetGetMacAddr(void)
{
  return gnetif.hwaddr;
}

uint32_t ethernetGetIpAddr(void)
{
  return gnetif.ip_addr.addr;
}

char* ethernetGetIpAddrAsString(void)
{
  return ip4addr_ntoa((const ip4_addr_t *) &gnetif.ip_addr);
}

uint32_t ethernetGetSubnet(void)
{
  return gnetif.netmask.addr;
}

uint32_t ethernetGetGateway(void)
{
  return gnetif.gw.addr;
}

static void ethernetHwInit(void)
{
  /*  Registers the default network interface. */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }
}

/**
 * @brief  DHCP Process
 * @param  argument: network interface
 * @retval None
 */
static void ethernetDHCPThread(void const * argument)
{
  struct netif *netif = (struct netif *) argument;
  struct dhcp *dhcp;
  uint8_t iptxt[20];

  for (;;)
  {
    switch (DHCP_state)
    {
      case DHCP_START :
      {
        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);
        dhcp_start(netif);
        DHCP_state = DHCP_WAIT_ADDRESS;
        USBD_UsrLog("[DHCP] State: Looking for DHCP server ...\n");
      }
        break;

      case DHCP_WAIT_ADDRESS :
      {
        if (dhcp_supplied_address(netif))
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;

          sprintf((char *) iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *) &netif->ip_addr));
          USBD_UsrLog("[DHCP] IP address assigned by a DHCP server: %s\n", iptxt);
        }
        else
        {
          dhcp = (struct dhcp *) netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

          /* DHCP timeout */
          if (dhcp->tries > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;

            /* Stop DHCP */
            dhcp_stop(netif);

            /* Static address used */
            netif_set_addr(netif, ip_2_ip4(&local_ip_addr), ip_2_ip4(&local_subnet), ip_2_ip4(&local_gateway));

            sprintf((char *) iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *) &netif->ip_addr));
            USBD_UsrLog("[DHCP] DHCP Timeout !! \n");
            USBD_UsrLog("[ETH] Static IP address: %s\n", iptxt);
          }
        }
      }
        break;
      case DHCP_LINK_DOWN :
      {
        /* Stop DHCP */
        dhcp_stop(netif);
        DHCP_state = DHCP_OFF;
      }
        break;
      default:
        break;
    }
    delay(250);
    ethernetDHCPMaintain();
  }
}

static void ethernetDHCPMaintain(void)
{
  if (netif_is_up(&gnetif))
  {
    switch (DHCP_state)
    {
      case DHCP_TIMEOUT :
      case DHCP_OFF :
      case DHCP_LINK_DOWN :
        DHCP_state = DHCP_START;
        break;
      default:
        break;
    }
  } else
  {
    DHCP_state = DHCP_LINK_DOWN;
  }
}

void ethernetGetIpByDNS(const char *domainname, ip_addr_t* recv_ip_addr){
  struct hostent* p_domain_info;

  p_domain_info = gethostbyname(domainname);
  memcpy(recv_ip_addr, p_domain_info->h_addr_list[0], p_domain_info->h_length);
}

err_t ethernetOpenSocket(socket_t* p_socket_t, u16_t local_port, socket_op_mode_t mode)
{
  socket_t* p = p_socket_t;

  if (p == NULL)
    return ERR_ARG;

  switch (mode)
  {
    case TCP_CLIENT:
      if ((p->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
      {
        return ERR_ARG;
      }
      break;

    case TCP_SERVER:
      if ((p->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
      {
        return ERR_ARG;
      }
      p->local_addr.sin_family = AF_INET;
      p->local_addr.sin_port = htons(local_port);
      p->local_addr.sin_addr.s_addr = INADDR_ANY; //or (in_addr_t)gnetif.ip_addr.addr;

      if (bind(p->sockfd, (struct sockaddr*)&p->local_addr, sizeof(p->local_addr)) < 0)
      {
        return ERR_ARG;
      }
      listen(p->sockfd, 5);
      break;

    case UDP_PEER:
      if ((p->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0)
      {
        return ERR_ARG;
      }
      p->local_addr.sin_family = AF_INET;
      p->local_addr.sin_port = htons(local_port);
      p->local_addr.sin_addr.s_addr = INADDR_ANY;

      if (bind(p->sockfd, (struct sockaddr*)&p->local_addr, sizeof(p->local_addr)) < 0)
      {
        return ERR_ARG;
      }
      break;

    case RAW_MODE:
      if ((p->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_RAW)) < 0)
      {
        return ERR_ARG;
      }
      break;

    default:
      return ERR_ARG;
  }

  return ERR_OK;
}

int ethernetAccept(socket_t* p_socket_t)
{
  socket_t* p = p_socket_t;
  int sock_state, sock_state_size;

  if (p == NULL)
    return ERR_ARG;

  getsockopt(p->sockfd, SOL_SOCKET, SO_ACCEPTCONN, &sock_state, (socklen_t* )&sock_state_size);
  if (sock_state == 0)
    return -1;

  return accept(p->sockfd, (struct sockaddr* )&p->remote_addr, &p->remote_addr_size);
}

int ethernetConnect(socket_t* p_socket_t, ip_addr_t* p_remote_ip_addr, u16_t remote_port)
{
  socket_t* p = p_socket_t;
  int err, err_size;

  if (p == NULL)
    return ERR_ARG;

  p->remote_addr.sin_family = AF_INET;
  p->remote_addr.sin_port = (in_port_t) htons(remote_port);
  p->remote_addr.sin_addr.s_addr = (in_addr_t) p_remote_ip_addr->addr;

  err = connect(p->sockfd, (struct sockaddr* )&p->remote_addr, sizeof(p->remote_addr));
  getsockopt(p->sockfd, SOL_SOCKET, SO_ERROR, &err, (socklen_t* )&err_size); //for detail err check

  return err;
}

int ethernetRecvTCP(int socket_num, uint8_t* p_recv_buf, size_t buf_size)
{
  if (p_recv_buf == NULL)
    return ERR_ARG;
  return recv(socket_num, p_recv_buf, buf_size-1, 0); //return recv size or error
}

int ethernetSendTCP(int socket_num, uint8_t* p_send_buf, size_t send_size)
{
  if (p_send_buf == NULL)
    return ERR_ARG;
  return write(socket_num, p_send_buf, send_size); // If fail, return -1
}

int ethernetRecvUDP(socket_t* p_socket_t, uint8_t* p_recv_buf, size_t buf_size)
{
  socket_t* p = p_socket_t;
  if (p_recv_buf == NULL)
  {
    return ERR_ARG;
  }
  return recvfrom(p->sockfd, p_recv_buf, buf_size-1, 0, (struct sockaddr* )&p->remote_addr, &p->remote_addr_size); //return recv size or error
}

int ethernetSendUDP(socket_t* p_socket_t, uint8_t* p_send_buf, size_t send_size, ip_addr_t* p_remote_ip_addr,
    u16_t remote_port)
{
  struct sockaddr_in remote_ip_addr;
  socket_t* p = p_socket_t;
  if (p_send_buf == NULL)
    return ERR_ARG;

  remote_ip_addr.sin_family = AF_INET;
  remote_ip_addr.sin_port = (in_port_t) htons(remote_port);
  remote_ip_addr.sin_addr.s_addr = (in_addr_t) p_remote_ip_addr->addr;

  return sendto(p->sockfd, p_send_buf, send_size, 0, (struct sockaddr* )&remote_ip_addr, sizeof(remote_ip_addr)); // If fail, return -1
}

err_t ethernetCloseSocket(int socket_num)
{
  if (close(socket_num) < 0)
    return ERR_ARG;
  return ERR_OK;
}

#if LWIP_TCP
void ethernetEnableKeepAlive(int socket_num)
{
  int enable = 1;
  setsockopt(socket_num, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(int));

  int idle = 1;
  setsockopt(socket_num, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

  int interval = 10;
  setsockopt(socket_num, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

  int maxpkt = 10;
  setsockopt(socket_num, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));
}
#endif /* LWIP_TCP */


err_code_t ethernetJoinGroup(int socket_num, in_addr_t group_ip_addr, in_addr_t interface_addr)
{
  err_code_t err = OK;
  ip_mreq optval;

  optval.imr_multiaddr.s_addr = group_ip_addr;
  optval.imr_interface.s_addr = interface_addr;

  if(setsockopt(socket_num, IPPROTO_IP, IP_ADD_MEMBERSHIP, &optval, sizeof(optval)) == -1)
  {
    err = ERR_ETHERNET_JOIN_GROUP;
  }

  return err;
}

err_code_t ethernetLeaveGroup(int socket_num, in_addr_t group_ip_addr)
{
  err_code_t err = OK;
  ip_mreq optval;

  optval.imr_multiaddr.s_addr = group_ip_addr;
  optval.imr_interface.s_addr = INADDR_ANY;

  if(setsockopt(socket_num, IPPROTO_IP, IP_DROP_MEMBERSHIP, &optval, sizeof(int)) == -1)
  {
    err = ERR_ETHERNET_LEAVE_GROUP;
  }

  return err;
}


#ifdef _USE_HW_CMDIF_ETH

void apEthernetCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd((char *) "eth", apEthernetCmdif);
}

static int apEthernetCmdif(int argc, char **argv)
{
  bool ret = true;

  if (argc == 2)
  {
    if (strcmp("server", argv[1]) == 0)
    {
      cmdifPrintf("This is loopback test for Ethernet\r\n");
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf("eth [arg1] [arg2] [arg3] [arg4] [arg5]\n");
    cmdifPrintf("[arg1] : 2 ~ FE \n");
    cmdifPrintf("[arg2] : 2 ~ 253 \n");
    cmdifPrintf("[arg3] : \n");
    cmdifPrintf("[arg4] : \n");
    cmdifPrintf("[arg5] : \n");

  }
  return 0;
}

#endif /* _USE_HW_CMDIF_ETH */
#endif /* _USE_HW_RTOS */
#endif /* _USE_HW_ETH */

