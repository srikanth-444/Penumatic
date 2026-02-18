#include "Constant.h"



void init_network();
bool connect_to_server();
int send_over_net(const char *data, int len);
int receive_over_net(char *data, int len);