typedef struct{
    uint8_t buff_len;
    uint8_t* tx_buffer;
    uint8_t* rx_buffer;
    uint32_t clock_speed_hz;
    uint8_t mode;
    uint8_t cs_pin;
    uint8_t queue_size;
    spi_host_device_t bus;
    spi_device_handle_t devhandler;
    spi_transaction_t t;
    void (*init)(Serif* device);
    void (*send)(Serif* device);
    void (*free_t)(Serif* device);
}Serif;

Serif* create_serif(spi_host_device_t bus, uint32_t clk, uint8_t mode, uint8_t qsize, uint8_t buff_len)


