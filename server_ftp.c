#include <server_ftp.h>

size_t size_sockaddr = sizeof(struct sockaddr), size_packet = sizeof(struct packet);

void* serve_client(void*);

int main(void)
{
	//INIZIO: inizializzazione
	struct sockaddr_in sin_server, sin_client;
	int sfd_server, sfd_client, x;
	short int connection_id = 0;
	
	if((x = sfd_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		er("socket()", x);
	
	memset((char*) &sin_server, 0, sizeof(struct sockaddr_in));
	sin_server.sin_family = AF_INET;
	sin_server.sin_port = htons(PORTSERVER);
	sin_server.sin_addr.s_addr = htonl(INADDR_ANY);

	if((x = bind(sfd_server, (struct sockaddr*) &sin_server, size_sockaddr)) < 0)
		er("bind()", x);
	
	if((x = listen(sfd_server, 1)) < 0)
		er("listen()", x);
	
	printf(ID "FTP Server avviato su @ local:%d. Attendo il client...\n\n", PORTSERVER);
	//FINE: inizializzazione

	
	while(1)
	{
		if((x = sfd_client = accept(sfd_server, (struct sockaddr*) &sin_client, &size_sockaddr)) < 0)
			er("accept()", x);
		printf(ID "Comunicazione avviata con %s:%d\n", inet_ntoa(sin_client.sin_addr), ntohs(sin_client.sin_port));
		fflush(stdout);
		
		struct client_info* ci = client_info_alloc(sfd_client, connection_id++);
		serve_client(ci);
	}
	
	close(sfd_server);
	printf(ID "Completato.\n");
	fflush(stdout);
	
	return 0;
}

void* serve_client(void* info)
{
	int sfd_client, connection_id, x;
	struct packet* data = (struct packet*) malloc(size_packet);
	struct packet* shp;
	char lpwd[LENBUFFER];
	struct client_info* ci = (struct client_info*) info;
	sfd_client = ci->sfd;
	connection_id = ci->cid;
	
	while(1)
	{
		if((x = recv(sfd_client, data, size_packet, 0)) == 0)
		{
			fprintf(stderr, "client chiuso/terminato. Chiudo la connessione.\n");
			break;
		}
		
		shp = ntohp(data);
		
		if(shp->type == TERM)
			break;
		
		if(shp->conid == -1)
			shp->conid = connection_id;
		
		if(shp->type == REQU)
		{
			switch(shp->comid)
			{
				case PWD:
					if(!getcwd(lpwd, sizeof lpwd))
						er("getcwd()", 0);
					command_pwd(shp, data, sfd_client, lpwd);
					break;
				case CD:
					if((x = chdir(shp->buffer)) == -1)
						fprintf(stderr, "Percorso errato.\n");
					command_cd(shp, data, sfd_client, x == -1 ? "fail" : "success");
					break;
				case MKDIR:
					command_mkdir(shp, data, sfd_client);
					break;
				case LS:
					if(!getcwd(lpwd, sizeof lpwd))
						er("getcwd()", 0);
					command_ls(shp, data, sfd_client, lpwd);
					break;
				case GET:
					command_get(shp, data, sfd_client);
					break;
				case PUT:
					command_put(shp, data, sfd_client);
					break;
				case RGET:
					if(!getcwd(lpwd, sizeof lpwd))
						er("getcwd()", 0);
					command_rget(shp, data, sfd_client);
					send_EOT(shp, data, sfd_client);
					if((x = chdir(lpwd)) == -1)
						fprintf(stderr, "Percorso errato.\n");
				default:
					// stampa errore
					break;
			}
				}
		else
		{
			//mostra errore, invia TERM e ferma
			fprintf(stderr, "packet incomprensibile. Chiudo la connessione.\n");
			send_TERM(shp, data, sfd_client);
			break;
		}
	}

	close(sfd_client);
	fflush(stdout);
}

