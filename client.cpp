#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include "project.pb.h"

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void str_overwrite_stdout() {
	printf("%s", "> ");
	fflush(stdout);
}

void str_trim_lf (char* arr, int length) {
	int i;
	for (i = 0; i < length; i++) { // trim \n
		if (arr[i] == '\n') {
			arr[i] = '\0';
			break;
		}
	}
}

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void* prueba(char* message){
    char recipient_name[LENGTH];
    size_t start, end;

    // Buscamos el primer '<' y el primer '>'
    start = strcspn(message, "<") + 1;
    end = strcspn(message + start, ">");

    // Si encontramos ambos caracteres, extraemos el nombre
    if (start != 0 && end != 0) {
        strncpy(recipient_name, message + start, end);
        recipient_name[end] = '\0';
    }

    // Imprimimos el resultado
    printf("Nombre del destinatario: %s\n", recipient_name);
    printf("Mensaje: %s\n", message + start + end + 2);

    return 0;
}



/**

void* send_msg_handler(void* arg){
	char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};

	while(1) {
		str_overwrite_stdout();
		fgets(message, LENGTH, stdin);
		str_trim_lf(message, LENGTH);

		if (strcmp(message, "exit") == 0) {
				break;
		} else {
		sprintf(buffer, "%s: %s\n", name, message);
		send(sockfd, buffer, strlen(buffer), 0);
		}
		prueba(message);
		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);
	}
	catch_ctrl_c_and_exit(2);
}
**/


void* send_msg_handler(void* arg){
	
//Variable Menú
	int option = 0;
	char optionBuffer[10];

	printf("=== Bienvenidos al Proyecto Chat ===\n");

	while(option != 6) {
		int validOption = 0;
		while(validOption == 0) {
			printf("1. Chat.\n2. Cambiar de status.\n3. Listar los usuarios conectados al sistema de chat.\n4. Desplegar información de un usuario en particular.\n5. Ayuda.\n6. Salir.\nElige una opción: ");
			fgets(optionBuffer, 10, stdin);
			option = atoi(optionBuffer);
			if(option < 7 && option > 0) {
				validOption = 1;
			} else {
				printf("Opcion no válida");
			}
		}

		if(option == 1) {
			printf("=== Bienvenidos al Chat ===\n");
			while(1) {

				char message[LENGTH] = {};
				char buffer[LENGTH + 32] = {};

				str_overwrite_stdout();
				fgets(message, LENGTH, stdin);
				str_trim_lf(message, LENGTH);

				if (strcmp(message, "exit") == 0) {
					break;
				} else {
					sprintf(buffer, "%s: %s\n", name, message);
					chat::UserRequest userRequest;
					chat::newMessage newMessage1;

					newMessage1.set_message_type(1);
					newMessage1.set_sender(name);
					newMessage1.set_message(message);
					userRequest.set_option(4);
					userRequest.set_allocated_message(newMessage1);

					std::string serialized_request;
					userRequest.SerializeToString(&serialized_request);

					// Preparar los datos para ser enviados
					const char* data = serialized_request.c_str();
					size_t data_len = serialized_request.length();

					send(sockfd, data, data_len, 0);
				}

				bzero(message, LENGTH);
				bzero(buffer, LENGTH + 32);
			}
			catch_ctrl_c_and_exit(2);
		} else if(option == 2) {
			printf("Cambiar de status\n");	 
		} else if(option == 3) {
			
		} else if(option == 4) {
			printf("Funciona");
		} else if(option == 5) {
			printf("Ayuda del sistema.");
		} else if(option == 6) {
			printf("\nGracias por usar nuestro chat.\n");
			//exit(0);
		}
	}
}

void* recv_msg_handler(void* arg){
	char message[LENGTH] = {};
	while (1) {
		int receive = recv(sockfd, message, LENGTH, 0);
		if (receive > 0) {
			printf("%s", message);
			str_overwrite_stdout();
		} else if (receive == 0) {
			break;
		} else {
			// -1
		}
		memset(message, 0, sizeof(message));
	}
}

int main(int argc, char **argv){
	if(argc != 4){
		printf("Uso correcto: ./client Nombre <ip> <puerto>\n");
		return EXIT_FAILURE;
	}

	snprintf(name, sizeof(name), "%s", argv[1]);
	char *ip = argv[2];
	int port = atoi(argv[3]);

	signal(SIGINT, catch_ctrl_c_and_exit);

	struct sockaddr_in server_addr;

	// Socket settings 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	// Connect to Server
	int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1) {
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// Send name
	send(sockfd, name, 32, 0);

	pthread_t send_msg_thread;
	if(pthread_create(&send_msg_thread, NULL, send_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	pthread_t recv_msg_thread;
	if(pthread_create(&recv_msg_thread, NULL, recv_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}	
	
	while (1){
		if(flag){
			printf("\nChat cerrado\n");
			break;
		}
	}

	close(sockfd);
	return EXIT_SUCCESS;
}
