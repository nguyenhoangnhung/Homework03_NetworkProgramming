

#include "stdafx.h"
#include "winsock2.h"

void RemoveClient(SOCKET);
SOCKET clients[64];
int numClients ;
int main(int argc,char *argv[])
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
	char *ids[64];
	SOCKET clientsConnected[64];
	int numClientsConnected = 0;
	char buf[256];

	char sendBuf[512];
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
						for (int k = 0; k < numClients; k++) {
							if (clients[k] != clients[i]) {
								
								sprintf(sendBuf, "%s : %s", ids[k], mess);
								send(clients[k], sendBuf, strlen(sendBuf), 0);
							}
						}
					}
					continue;
				}

				if (FD_ISSET(clients[i], &fdread) && !connected)
				{
					ret = recv(clients[i], buf, sizeof(buf), 0);

					if (ret <= 0)
					{
						// Ket noi bi huy
						RemoveClient(clients[i]);
						continue;
					}

					char cmd[64];
					char id[64];
					char tmp[64];
					char *errorMsg = "Loi cu phap. Hay nhap lai\n";
					buf[ret] = 0;
					printf("Received from %d: %s\n", clients[i], buf);
					// Kiem tra cu phap client_id: [id]
					ret = sscanf(buf, "%s %s %s", cmd, id, tmp);
					if (ret == 2)
					{
						if (strcmp(cmd, "client_id:") == 0)
						{
							char *okMsg = "Dung cu phap. Hay nhap thong diep muon gui.\n";
							send(clients[i], okMsg, strlen(okMsg), 0);

							// Luu client dang nhap thanh cong vao mang
							clientsConnected[numClientsConnected] = clients[i];
							ids[numClientsConnected] = id;
							numClientsConnected++;

							break;
						}
						else
							send(clients[i], errorMsg, strlen(errorMsg), 0);
					}
					else
						send(clients[i], errorMsg, strlen(errorMsg), 0);
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