#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CENG435_IP "172.24.0.10"
#define CENG435_PORT 2468
#define CENG435_BUFFER_SIZE 1024

#define CENG435_PAYLOAD_LEN 16
#define CENG435_FIN 0x0001
#define CENG435_SYN 0x0002
#define CENG435_ACK 0x0010


struct my_dgram {
    uint32_t seq_no;
    uint32_t ack_no;
    uint16_t flags;
    uint16_t checksum;
    char payload[CENG435_PAYLOAD_LEN];
};

// sequence no of last packet sent to the server
int server_ack = 456;
// sequence no of last packet received from the server
int client_seq = 123;

int my_receive(int client_socket, char* message_buffer, int message_buffer_size, struct sockaddr_in* server_addr_ptr, socklen_t* server_addr_len_ptr) {

    // message_len includes the '\0' character
    int message_len = 0;
    int in_len = 0;
    bool last_fragment = false;

    while (last_fragment == false) {

        // prepare temporary server address structure
        struct sockaddr_in temp_server_addr;
        socklen_t temp_server_addr_len = sizeof(temp_server_addr);

        // receive packet from the server
        my_dgram in_packet;
        if (recvfrom(client_socket, (char*) &in_packet, sizeof(my_dgram), MSG_WAITALL, (struct sockaddr*) &temp_server_addr, &temp_server_addr_len) < 0) {
            perror("ERROR: while receiving packet");
            exit(EXIT_FAILURE);
        }

        // ignore if ACK packet
        if (!(in_packet.flags & CENG435_ACK)) {

            // prepare ACK packet
            my_dgram out_packet;
            out_packet.seq_no = 0;
            // out_packet.ack_no = 0;
            out_packet.flags = CENG435_ACK;
            out_packet.checksum = 0;
            strncpy(out_packet.payload, "", CENG435_PAYLOAD_LEN);

            // check if sequence is valid
            if (in_packet.seq_no == server_ack + 1) {

                // copy payload to message buffer
                strncpy(message_buffer + in_len, in_packet.payload, CENG435_PAYLOAD_LEN);
                if (in_packet.payload[CENG435_PAYLOAD_LEN - 1] == '\0') {
                    in_len += strlen(in_packet.payload) + 1;
                    // break
                    last_fragment = true;
                } else {
                    in_len += CENG435_PAYLOAD_LEN;
                }

                // increment acknowledgement number
                server_ack++;

                // send ACK packet
                out_packet.ack_no = server_ack;
                if (sendto(client_socket, (char*) &out_packet, sizeof(my_dgram), MSG_CONFIRM, (struct sockaddr*) server_addr_ptr, *server_addr_len_ptr) < 0) {
                    perror("ERROR: while sending message");
                    exit(EXIT_FAILURE);
                }
            } else {
                // send previous ACK packet (NACK packet)
                out_packet.ack_no = server_ack;
                if (sendto(client_socket, (char*) &out_packet, sizeof(my_dgram), MSG_CONFIRM, (struct sockaddr*) server_addr_ptr, *server_addr_len_ptr) < 0) {
                    perror("ERROR: while sending message");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    message_len = in_len;
    return message_len;
}

void my_send(int client_socket, const char* message, int message_len, struct sockaddr_in* server_addr_ptr, socklen_t* server_addr_len_ptr) {

    for (int out_len = 0; message_len - out_len > 0; out_len += CENG435_PAYLOAD_LEN) {

        // increment sequence number
        client_seq++;

        // send first 16 bytes of the message to the server
        my_dgram out_packet;
        out_packet.seq_no = client_seq;
        out_packet.ack_no = 0;
        out_packet.flags = 0;
        out_packet.checksum = 0;
        strncpy(out_packet.payload, message + out_len, CENG435_PAYLOAD_LEN);
        if (sendto(client_socket, (char*) &out_packet, sizeof(my_dgram), MSG_CONFIRM, (struct sockaddr*) server_addr_ptr, *server_addr_len_ptr) < 0) {
            perror("ERROR: while sending message");
            exit(EXIT_FAILURE);
        }

        while (true) {
            // prepare temporary server address structure
            struct sockaddr_in temp_server_addr;
            socklen_t temp_server_addr_len = sizeof(temp_server_addr);

            // receive packet from the server
            my_dgram in_packet;
            if (recvfrom(client_socket, (char*) &in_packet, sizeof(my_dgram), MSG_WAITALL, (struct sockaddr*) &temp_server_addr, &temp_server_addr_len) < 0) {
                perror("ERROR: while receiving packet");
                exit(EXIT_FAILURE);
            }

            // ignore if not ACK packet
            if (in_packet.flags & CENG435_ACK) {

                // resend the packet if ACK is not valid
                if (in_packet.ack_no != client_seq) {
                    if (sendto(client_socket, (char*) &out_packet, sizeof(my_dgram), MSG_CONFIRM, (struct sockaddr*) server_addr_ptr, *server_addr_len_ptr) < 0) {
                        perror("ERROR: while sending message");
                        exit(EXIT_FAILURE);
                    }
                    continue;
                }

                // ACK is valid
                break;
            }
        }
    }
}

int main() {

    // create a socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("ERROR: while creating socket");
        exit(EXIT_FAILURE);
    }

    // prepare server address structure
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    memset(&server_addr, 0, server_addr_len);
    server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    if (inet_pton(AF_INET, CENG435_IP, &server_addr.sin_addr) < 0) {
        perror("ERROR: while setting address");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_port = htons(CENG435_PORT);


    // prepare ACK packet
    my_dgram out_packet;
    out_packet.seq_no = 0;
    out_packet.ack_no = 0;
    out_packet.flags = 0;
    out_packet.checksum = 0;
    strncpy(out_packet.payload, "", CENG435_PAYLOAD_LEN);
    if (sendto(client_socket, (char*) &out_packet, sizeof(my_dgram), MSG_CONFIRM, (struct sockaddr*) &server_addr, server_addr_len) < 0) {
        perror("ERROR: while sending message");
        exit(EXIT_FAILURE);
    }

    bool keep_on = true;
    while (keep_on) {

        // send to client from stdin
        std::string out_buffer;
        std::getline(std::cin, out_buffer);
        int out_buffer_len = out_buffer.size() + 1;
        my_send(client_socket, out_buffer.c_str(), out_buffer_len, &server_addr, &server_addr_len);

        // receive from client
        char in_buffer[CENG435_BUFFER_SIZE] = { 0 };
        size_t in_len = my_receive(client_socket, in_buffer, CENG435_BUFFER_SIZE, &server_addr, &server_addr_len);
        std::cout << in_buffer << std::endl;
    }

    // close the socket
    close(client_socket);
}
