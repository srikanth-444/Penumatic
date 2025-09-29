#include "Network.h"
int sock = -1;
bool socket_ready = false;
struct sockaddr_in dest_addr;
SemaphoreHandle_t network_sem;
extern SemaphoreHandle_t bus1_sem;
extern SemaphoreHandle_t bus2_sem;
extern SemaphoreHandle_t socket_mutex;
esp_netif_t *ap_netif;
esp_netif_ip_info_t ip_info;


static void ip_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    if (event_id == IP_EVENT_ASSIGNED_IP_TO_CLIENT) {
        ip_event_assigned_ip_to_client_t* event = (ip_event_assigned_ip_to_client_t*) event_data;
        ip_info.ip = event->ip;
        xSemaphoreGive(network_sem);
        xSemaphoreGive(bus1_sem);
        xSemaphoreGive(bus2_sem);
        ESP_LOGI("IP_EVENT", "Assigned IP to station: " IPSTR, IP2STR(&event->ip));
    }
}

void init_network(void)
{
    network_sem = xSemaphoreCreateBinary();
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
    xSemaphoreTake(network_sem, portMAX_DELAY); // wait for IP assignment
    xSemaphoreTake(bus1_sem,portMAX_DELAY);
    xSemaphoreTake(bus2_sem,portMAX_DELAY);
    xSemaphoreTake(socket_mutex,portMAX_DELAY);
    if(ip_info.ip.addr == 0) {
        ESP_LOGW(NETWORK, "No device connected to AP");
        xSemaphoreGive(network_sem);
        return false;
    }

    
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
    dest_addr.sin_family = AF_INET;                    // IPv4
    dest_addr.sin_port = htons(8080);                  // Port 8080, converted to network byte order
    dest_addr.sin_addr.s_addr = ip_info.ip.addr; // Server IP
    memset(dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero));

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        ESP_LOGE(NETWORK, "Socket creation failed");
        xSemaphoreGive(network_sem);
        return false;
    }

    if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == 0) {
        socket_ready = true;
        ESP_LOGI(NETWORK, "Connected!");
        xSemaphoreGive(network_sem);
        xSemaphoreGive(bus1_sem);
        xSemaphoreGive(bus2_sem);
        xSemaphoreGive(socket_mutex);
        return true;
    } else {
        ESP_LOGE(NETWORK, "Connect failed");
        socket_ready = false;
        close(sock);
        sock = -1;
        xSemaphoreGive(network_sem);
        return false;
    }
    
}

void send_over_net(const char *data, int len) {
    if (!socket_ready) {
        if (!connect_to_server()) {
            return;
        }
    }
    int ret = send(sock, data, len, 0);
    if (ret < 0) {
        ESP_LOGE(NETWORK, "Send failed, reconnecting...");
        socket_ready=false;
        connect_to_server();  // retry connection immediately
    }
    
}

void receive_over_net(char *data, int len) {
    if (!socket_ready) {
        if (!connect_to_server()) {
            return; // not connected
        }
    }

    int ret = recv(sock, data, len, 0); // 0 = default flags
    if (ret < 0) {
        ESP_LOGE(NETWORK, "Receive failed, reconnecting...");
        socket_ready = false;
        connect_to_server();
    }
}

