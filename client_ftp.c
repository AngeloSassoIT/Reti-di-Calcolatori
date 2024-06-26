#include <client_ftp.h>

int main(int argc, char* argv[])
{
	//INIZIO: inizializzazione
	struct sockaddr_in sin_server;
	int sfd_client, x;
	size_t size_sockaddr = sizeof(struct sockaddr), size_packet = sizeof(struct packet);
	short int connection_id;
	struct packet* chp = (struct packet*) malloc(size_packet);		// client host packet
	set0(chp);
	struct packet* data;							// network packet
	
	if((x = sfd_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		er("socket()", x);
	
	memset((char*) &sin_server, 0, sizeof(struct sockaddr_in));
	sin_server.sin_family = AF_INET;
	sin_server.sin_addr.s_addr = inet_addr(IPSERVER);
	sin_server.sin_port = htons(PORTSERVER);
	
	if((x = connect(sfd_client, (struct sockaddr*) &sin_server, size_sockaddr)) < 0)
		er("connect()", x);
			
	printf(ID "FTP Client avviato. Comunicazione con server @ %s:%d...\n\n", IPSERVER, PORTSERVER);
	//FINE: inizializzazione

	
	struct command* cmd;
	char lpwd[LENBUFFER], pwd[LENBUFFER];
	char userinput[LENUSERINPUT];
	while(1)
	{
		printf("\t> ");
		fgets(userinput, LENUSERINPUT, stdin);	//  per inserire \
							un filename con spazi, inserire ':' \
							invece di ' '.
		cmd = userinputtocommand(userinput);
		if(!cmd)
			continue;

		switch(cmd->id)
		{
			case GET:
				if(cmd->npaths)
					command_get(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "Nessun percorso per il file indicato.\n");
				break;
			case PUT:
				if(cmd->npaths)
					command_put(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "Nessun percorso per il file indicato.\n");
				break;
			case MGET:
				if(cmd->npaths)
					command_mget(chp, data, sfd_client, cmd->npaths, cmd->paths);
				else
					fprintf(stderr, "Nessun percorso per il file indicato.\n");
				break;
			case MPUT:
				if(cmd->npaths)
					command_mput(chp, data, sfd_client, cmd->npaths, cmd->paths);
				else
					fprintf(stderr, "Nessun percorso per il file indicato.\n");
				break;
			case MGETWILD:
				command_mgetwild(chp, data, sfd_client);
				break;
			case MPUTWILD:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_mputwild(chp, data, sfd_client, lpwd);
				break;
			case CD:
				if(cmd->npaths)
					command_cd(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "Nessun percorso indicato.\n");
				break;
			case LCD:
				if(cmd->npaths)
					command_lcd(*cmd->paths);
				else
					fprintf(stderr, "Nessun percorso indicato.\n");
				break;
			case PWD:
				command_pwd(chp, data, sfd_client);
				break;
			case LPWD:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				printf("\t%s\n", lpwd);
				break;
			case DIR_:
			case LS:
				command_ls(chp, data, sfd_client);
				break;
			case LDIR:
			case LLS:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_lls(lpwd);
				break;
			case MKDIR:
				if(cmd->npaths)
					command_mkdir(chp, data, sfd_client, *cmd->paths);
				else
					fprintf(stderr, "Nessun percorso per la directory indicato.\n");
				break;
			case LMKDIR:
				if(cmd->npaths)
					command_lmkdir(*cmd->paths);
				else
					fprintf(stderr, "Nessun percorso per la directory indicato.\n");
				break;
			case RGET:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_rget(chp, data, sfd_client);
				if((x = chdir(lpwd)) == -1)
					fprintf(stderr, "Percorso errato.\n");
				break;
			case RPUT:
				if(!getcwd(lpwd, sizeof lpwd))
					er("getcwd()", 0);
				command_rput(chp, data, sfd_client);
				if((x = chdir(lpwd)) == -1)
					fprintf(stderr, "Percorso errato.\n");
				break;
			case EXIT:
				goto outside_client_command_loop;
			default:
				// stampa errore
				break;
		}
	}
	outside_client_command_loop:
	
	close(sfd_client);
	printf(ID "Completato.\n");
	fflush(stdout);
	
	return 0;
}

