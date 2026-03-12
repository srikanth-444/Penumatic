#include "Network.h"
int send_sock = -1;
int recv_sock = -1;
bool send_socket_ready = false;
bool recv_socket_ready = false;
struct sockaddr_in send_dest_addr;
struct sockaddr_in recv_dest_addr;
esp_netif_t *ap_netif;
esp_netif_ip_info_t ip_info;


static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    if (event_id == IP_EVENT_ASSIGNED_IP_TO_CLIENT) {
        ip_event_assigned_ip_to_client_t* event = (ip_event_assigned_ip_to_client_t*) event_data;
        ip_info.ip = event->ip;
        ESP_LOGI("IP_EVENT", "Assigned IP to station: " IPSTR, IP2STR(&event->ip));
    }
}

void init_network(void)
{
    
        // System initialization
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
 

    // 2. Create default Wi-Fi AP interface
    ap_netif = esp_netif_create_default_wifi_ap();
    assert(ap_netif);

    // 3. Init Wi-Fi driver
    wifi_init_config_t cfg_init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg_init));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_ASSIGNED_IP_TO_CLIENT,
                                                        &ip_event_handler,
                                                        NULL,
                                                        NULL));

    // 4. Configure Wi-Fi AP
    wifi_config_t cfg = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .channel = ESP_WIFI_CHANNEL,
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen((char *)cfg.ap.password) == 0) {
        cfg.ap.authmode = WIFI_AUTH_OPEN;
    }

    // 5. Set mode and config
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &cfg));

    // 6. Start Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WIFI", "Wi-Fi AP started. SSID:%s password:%s channel:%d",
             ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);   
}

bool connect_to_server() {
    if(ip_info.ip.addr == 0) {
        printf("\rNo device connected to AP");
        fflush(stdout);
        return false;
    }
    if (send_sock >= 0) {
        close(send_sock);
        send_sock = -1;
    }
    if (recv_sock >= 0) {
        close(recv_sock);
        recv_sock = -1;
    }
    send_dest_addr.sin_family = AF_INET;                    // IPv4
    send_dest_addr.sin_port = htons(8080);                  // Port 8080, converted to network byte order
    send_dest_addr.sin_addr.s_addr = ip_info.ip.addr;       // Server IP
    memset(send_dest_addr.sin_zero, 0, sizeof(send_dest_addr.sin_zero));
    send_sock = socket(AF_INET, SOCK_STREAM, 0);
    recv_dest_addr.sin_family = AF_INET;                    // IPv4
    recv_dest_addr.sin_port = htons(5000);                  // Port 8081, converted to network byte order
    recv_dest_addr.sin_addr.s_addr = ip_info.ip.addr; // Server IP
    memset(recv_dest_addr.sin_zero, 0, sizeof(recv_dest_addr.sin_zero));
    recv_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (send_sock < 0) {
        ESP_LOGE(NETWORK, "Send Socket creation failed");
        return false;
    }
    if (recv_sock < 0){
        ESP_LOGE(NETWORK, "Receive Socket creation failed");
        return false;
    }
    if (connect(send_sock, (struct sockaddr *)&send_dest_addr, sizeof(send_dest_addr)) == 0) {
        send_socket_ready = true;
        ESP_LOGI(NETWORK, "Sending socket Connected!");
    } else {
        ESP_LOGE(NETWORK, "sending socket Connect failed");
        send_socket_ready = false;
        close(send_sock);
        send_sock = -1;
        return false;
    }
    if (connect(recv_sock, (struct sockaddr *)&recv_dest_addr, sizeof(recv_dest_addr)) == 0) {
        recv_socket_ready = true;
        ESP_LOGI(NETWORK, "Receiving socket Connected!");
    } else {
        ESP_LOGE(NETWORK, "receiving socket Connect failed");
        recv_socket_ready = false;
        close(recv_sock);
        recv_sock = -1;
        return false;
    }
    return true;
    
}

int send_over_net(const char *data, int len) {
    if (!send_socket_ready) {
        if (!connect_to_server()) {
            return -1;
        }
    }
    int ret = send(send_sock, data, len, 0);
    if (ret < 0) {
        ESP_LOGE(NETWORK, "Send failed, reconnecting...");
        send_socket_ready=false;   
    }
    return ret;
    
}

int receive_over_net(char *data, int len) {
    if (!recv_socket_ready) {
        if (!connect_to_server()) {
            return -1; 
        }
    }

    int ret = recv(recv_sock, data, len, 0); // 0 = default flags
    if (ret < 0) {
        ESP_LOGE(NETWORK, "Receive failed, reconnecting...");
        recv_socket_ready = false;  
    }
    return ret;
}

