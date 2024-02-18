/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Server/Src/udp_echoserver.c
  * @author  MCD Application Team
  * @brief   UDP echo server
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UDP_SERVER_PORT    7   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    7   /* define the UDP remote connection port */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//--------- UDP start ------------
#define UDP_SERVER_PORT    9999   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    9999   /* define the UDP remote connection port */
uint8_t udp_flag = 0;     // Add
uint8_t led_flag[4] = {0,0,0,0};     // Add
struct udp_pcb *upcb1;    // Add
ip_addr_t *addr1;   // Add
uint8_t udp_data[100];    // Add

extern UART_HandleTypeDef huart3;   //Add

//--------- UDP end ------------
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_init(void)
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive_callback, NULL);
      }
   }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
struct udp_pcb *upcb1;    // Add
uint8_t udp_data[100];    // Add

extern UART_HandleTypeDef huart3;   //Add
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{

  /* Connect to the remote client */
  udp_connect(upcb, addr, UDP_CLIENT_PORT);
    
  MEMCPY(udp_data, p->payload, sizeof(udp_data));    // UDP data를 1 byte 읽어온다
  if (strncmp(udp_data, "SERVO:", 6) == 0)
  {
	  /* Tell the client that we have accepted it */

	  udp_flag = 1;
  }

  if (strncmp(udp_data, "LED001", 6) == 0){
	  led_flag[1] = 1;
  }
  else if (strncmp(udp_data, "LED002", 6) == 0){
	  led_flag[2] = 1;
  }
  else if (strncmp(udp_data, "LED003", 6) == 0){
	  led_flag[3] = 1;
  }
  /* Tell the client that we have accepted it */
  udp_send(upcb, p);

  /* free the UDP connection, so we can accept new clients */
  udp_disconnect(upcb);
	
  /* Free the p buffer */
  pbuf_free(p);
   
}
