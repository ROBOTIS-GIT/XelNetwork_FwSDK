/*
 * ap_ethernet.h
 *
 *  Created on: 2017. 10. 20.
 *      Author: opus
 */

#ifndef AP_ETHERNET_H_
#define AP_ETHERNET_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hw_def.h"

#ifdef _USE_HW_ETH

#include "hw.h"
#include "lwip/lwip.h"


// DHCP process states
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

#define CLIENT_LOCAL_PORT          (u16_t)   0xFFFF

typedef enum{
  IP_STATIC = 0,
  IP_DHCP
}ip_assign_type_t;

typedef enum {
  TCP_SERVER = 0,
  TCP_CLIENT,
  UDP_PEER,
  RAW_MODE
}socket_op_mode_t;

typedef struct {
  int sockfd;
  socklen_t remote_addr_size;
  struct sockaddr_in local_addr, remote_addr;
}socket_t;

uint8_t ethernetIfIsInit(void);
void ethernetIfBegin(ip_assign_type_t dhcp_en, uint8_t* p_mac_addr, ip_addr_t* ip_addr, ip_addr_t* subnet,
    ip_addr_t* gateway, ip_addr_t* dns_server);

uint8_t* ethernetGetMacAddr(void);
uint32_t ethernetGetIpAddr(void);
uint32_t ethernetGetSubnet(void);
uint32_t ethernetGetGateway(void);

void ethernetGetIpByDNS(const char *domainname, ip_addr_t* recv_ip_addr);
err_t ethernetOpenSocket(socket_t* p_socket_t, u16_t local_port, socket_op_mode_t mode);
int ethernetAccept(socket_t* p_socket_t);
int ethernetConnect(socket_t* p_socket_t, ip_addr_t* p_remote_ip_addr, u16_t remote_port);

int ethernetRecvUDP(socket_t* p_socket_t, uint8_t* p_recv_buf, size_t buf_size);
int ethernetSendUDP(socket_t* p_socket_t, uint8_t* p_send_buf, size_t send_size, ip_addr_t* p_remote_ip_addr, u16_t remote_port);
int ethernetRecvTCP(int socket_num, uint8_t* p_recv_buf, size_t buf_size);
int ethernetSendTCP(int socket_num, uint8_t* p_send_buf, size_t send_size);
err_t ethernetCloseSocket(int socket_num);
void ethernetEnableKeepAlive(int socket_num);

err_code_t ethernetJoinGroup(int socket_num, in_addr_t group_ip_addr, in_addr_t interface_addr);
err_code_t ethernetLeaveGroup(int socket_num, in_addr_t group_ip_addr);

uint8_t ethernetGetDhcpStatus(void);


#ifdef _USE_HW_CMDIF_ETH
void apEthernetCmdifInit(void);
#endif

#endif /* _USE_HW_ETH */

#ifdef __cplusplus
}
#endif

#endif /* AP_ETHERNET_H_ */
