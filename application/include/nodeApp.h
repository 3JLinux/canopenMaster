#ifndef NODEAPP_H
#define NODEAPP_H
#include <unistd.h>


#define FIRE_NODE_MAX_NUM		200   //mark mark

typedef enum
{
    false   = 0,
    true    = 1
} bool;

void fire_node_info_init(void);
int8_t update_fire_node_info(uint8_t* write_dec_mac,uint8_t dec_mac_total_num);
void node_printf(void);
extern uint8_t get_node_mac_total_num(void);
extern uint8_t get_node_mac(uint8_t* dec_mac,uint8_t dec_num);
extern bool nodeBelognToMeByMac(const uint8_t *pcMac);
extern void node_dec_online(const uint8_t *pcMac);
extern void node_dec_outline(const uint8_t *pcMac);
void fillNotNetNodeInfo(uint8_t* data);
uint8_t node_getID(uint8_t* pcMac);
void node_addMAC(uint8_t* pcMac);
void node_check_in_out_net(void);
uint8_t* node_get_mac(const uint8_t nodeID);
uint8_t node_check_id(const uint8_t nodeID);
#endif
