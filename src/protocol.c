#include "protocol.h"
#include "options.h"
#include <stdbool.h>


char get_protocol_command(char *command_name) {
    if (strcasecmp(command_name, "EXECCMD") == 0) {
        return COMMAND_EXECCMD;
    } else if (strcasecmp(command_name, "EXECBIN") == 0) {
        return COMMAND_EXECBIN;
    } else if (strcasecmp(command_name, "EXECASM") == 0) {
        return COMMAND_EXECASM;
    } else if (strcasecmp(command_name, "DEVREAD") == 0) {
        return COMMAND_DEVREAD;
    } else if (strcasecmp(command_name, "DEVWRITE") == 0) {
        return COMMAND_DEVWRITE;
    } else if (strcasecmp(command_name, "DEVSHOW") == 0) {
        return COMMAND_DEVSHOW;
    } else if (strcasecmp(command_name, "FILEREAD") == 0) {
        return COMMAND_FILEREAD;
    } else if (strcasecmp(command_name, "FILEWRITE") == 0) {
        return COMMAND_FILEWRITE;
    } else if (strcasecmp(command_name, "SENDING") == 0) {
        return COMMAND_SENDINGFROMADMIN;
    } else if (strcasecmp(command_name, "RECEIVING") == 0) {
        return COMMAND_RECEIVINGFROMCLIENT;
    } else {
        return 0;
    }
}


void encapsulate_run_cmd_admin(char *buffer, char command, uint32_t ip, uint32_t mask, uint16_t port, bool output, uint64_t data_size) {
    buffer[0] = command;
    memcpy(&buffer[1], &ip, sizeof(uint32_t));
    memcpy(&buffer[5], &mask, sizeof(uint32_t));
    memcpy(&buffer[9], &port, sizeof(uint16_t));
    buffer[11] = output;
    memcpy(&buffer[12], &data_size, sizeof(uint64_t));
    // memcpy(&buffer[17], data, BUFFER_SIZE-16); // remaining space
}

void decapsulate_run_cmd_admin(char *buffer, uint32_t *ip, uint32_t *mask, uint16_t *port, bool *output, uint64_t *data_size) {
    // *command = buffer[0];
    memcpy(ip, &buffer[1], sizeof(uint32_t));
    memcpy(mask, &buffer[5], sizeof(uint32_t));
    memcpy(port, &buffer[9], sizeof(uint16_t));
    *output = buffer[11];
    memcpy(data_size, &buffer[12], sizeof(uint64_t));
    // memcpy(data, &buffer[17], BUFFER_SIZE-16);
}

void encapsulate_run_cmd_client(char *buffer, char command, uint32_t data_stream_id, bool output, uint64_t data_size) {
    buffer[0] = command;
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t));
    buffer[5] = output;
    memcpy(&buffer[6], &data_size, sizeof(uint64_t));
}

void decapsulate_run_cmd_client(char *buffer, uint32_t *data_stream_id, bool *output, uint64_t *data_size) {
    // *command = buffer[0];
    memcpy(data_stream_id, &buffer[1], sizeof(uint32_t));
    *output = buffer[5];
    memcpy(&data_size, &buffer[6], sizeof(uint64_t));
}


void encapsulate_transfer_cmd(char *buffer, char command, uint32_t data_stream_id) {
    buffer[0] = command; // 1 byte
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t)); // 4 bytes
}

// void decapsulate_transfer_cmd(char *buffer, uint32_t data_stream_id) {
//     // command = buffer[0]; // 1 byte
//     memcpy(&data_stream_id, &buffer[1], sizeof(uint32_t)); // 4 bytes
// }
uint32_t get_data_stream_id(char *buffer) {
    uint32_t data_stream_id;
    memcpy(&data_stream_id, &buffer[1], sizeof(uint32_t));
    return data_stream_id;
}

void encapsulate_transfer_cmd_larger(char *buffer, char command, uint64_t total_client_count) {
    buffer[0] = command; // 1 byte
    memcpy(&buffer[1], &total_client_count, sizeof(uint64_t)); // 4 bytes
}
uint64_t get_total_client_count(char *buffer) {
    uint64_t total_client_count;
    memcpy(&total_client_count, &buffer[1], sizeof(uint64_t));
    return total_client_count;
}



void encapsulate_transfer_data_cmd(char *buffer, char command, uint32_t data_stream_id, char *data, int offset) {
    buffer[0] = command; // 1 byte
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t)); // 4 bytes
    memcpy(&buffer[5], &data[offset], BUFFER_SIZE-5); // chunk of data
}

void decapsulate_transfer_data_cmd(char *buffer, uint32_t *data_stream_id, char *data) {
    // command = buffer[0]; // 1 byte
    memcpy(data_stream_id, &buffer[1], sizeof(uint32_t)); // 4 bytes
    memcpy(data, &buffer[5], BUFFER_SIZE-5); // chunk of data
}

void encapsulate_transfer_data_tid_cmd(char *buffer, char command, uint32_t data_stream_id, uint16_t thread_id, char *data) {
    buffer[0] = command; // 1 byte
    memcpy(&buffer[1], &data_stream_id, sizeof(uint32_t)); // 4 bytes
    memcpy(&buffer[5], &data_stream_id, sizeof(uint16_t)); // 2 bytes
    memcpy(&buffer[7], data, BUFFER_SIZE-7); // chunk of data
}
void decapsulate_transfer_data_tid_cmd(char *buffer, uint32_t *data_stream_id, uint16_t *thread_id, char *data) {
    // command = buffer[0]; // 1 byte
    memcpy(data_stream_id, &buffer[1], sizeof(uint32_t)); // 4 bytes
    memcpy(data_stream_id, &buffer[5], sizeof(uint16_t)); // 2 bytes
    memcpy(data, &buffer[7], BUFFER_SIZE-7); // chunk of data
}


bool validate_mask(uint32_t mask) {
    if (mask == UINT32_MAX || mask == 0){
        return true;
    }
    uint32_t inverted_mask = ~mask;
    uint32_t incremented_inverted_mask = inverted_mask + 1;
    return (incremented_inverted_mask & (incremented_inverted_mask - 1)) == 0;
}

