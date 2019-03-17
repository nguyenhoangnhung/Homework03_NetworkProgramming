

#include "stdafx.h"
#include "winsock2.h"

void RemoveClient(SOCKET);
SOCKET clients[64];
int numClients;
int main(int argc, char *argv[])
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(atoi(argv[1]));

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	fd_set fdread;
	int ret;
	numClients = 0;
	SOCKET clientsConnected[64];
	int numClientsConnected = 0;
	char buf[256];
	char fileBuf[256];
	char sendBuf[512];
	char cmdBuf[256];
	while (true)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		ret = select(0, &fdread, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR)
			break;

		if (ret > 0)
		{
			// Xu ly su kien co ket noi moi
			if (FD_ISSET(listener, &fdread))
			{
				SOCKET client = accept(listener, NULL, NULL);
				printf("New client accepted: %d\n", client);

				clients[numClients] = client;
				numClients++;
			}

			// Xu ly su kien khi co du lieu den cac client
			for (int i = 0; i < numClients; i++) {
				bool connected = false;
				//neu da dang nhap
				for (int j = 0; j < numClientsConnected; j++) {
					if (clients[i] == clientsConnected[j] && FD_ISSET(clients[i], &fdread)) {
						char mess[256];
						connected = true;
						ret = recv(clients[i], mess, sizeof(mess), 0);
						mess[ret] = 0;
						if (ret <= 0) {
							RemoveClient(clients[i]);
							continue;
						}
						//xu ly mess voi lenh system

						if (mess[ret - 1] == '\n')
							mess[ret - 1] = 0;

						printf("Received: %s\n", mess);

						sprintf(cmdBuf, "%s > c:\\test_server\\out.txt", mess);
						system(cmdBuf);

						FILE *f = fopen("C:\\test_server\\out.txt", "r");
						while (fgets(fileBuf, sizeof(fileBuf), f))
						{
							send(clients[i], fileBuf, strlen(fileBuf), 0);
						}
						fclose(f);
					}
				}
				// chua dang nhap dung
				if (FD_ISSET(clients[i], &fdread) && !connected)
				{
					ret = recv(clients[i], buf, sizeof(buf), 0);

					if (ret <= 0)
					{
						// Ket noi bi huy
						RemoveClient(clients[i]);
						continue;
					}
					buf[ret] = 0;
					printf("Received: %s\n", buf);

					int found = 0;
					FILE *f = fopen("UserData.txt", "r");
					while (fgets(fileBuf, sizeof(fileBuf), f))
					{
						if (strcmp(buf, fileBuf) == 0)
						{
							found = 1;
							break;
						}
					}
					fclose(f);

					if (found == 1)
					{
						char *msg = "Dang nhap thanh cong. Hay nhap lenh.\n";
						send(clients[i], msg, strlen(msg), 0);
						clientsConnected[numClientsConnected] = clients[i];
						numClientsConnected++;
						break;
					}
					else
					{
						char *msg = "Dang nhap that bai. Hay thu lai.\n";
						send(clients[i], msg, strlen(msg), 0);
					}
					
				}
			}
		}
	}

	return 0;
}


void RemoveClient(SOCKET client) {

	int i = 0;

	for (; i < numClients; i++)

		if (clients[i] == client)

			break;

	//xoa phan tu

	if (i < numClients - 1)

		clients[i] = clients[numClients - 1];

	numClients--;

}