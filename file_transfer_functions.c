#include <file_transfer_functions.h>

static size_t size_packet = sizeof(struct packet);

void send_EOT(struct packet* hp, struct packet* data, int sfd)
{
	int x;
	hp->type = EOT;
	data = htonp(hp);
	if((x = send(sfd, data, size_packet, 0)) != size_packet)
		er("send()", x);
}

void send_TERM(struct packet* hp, struct packet* data, int sfd)
{
	int x;
	hp->type = TERM;
	data = htonp(hp);
	if((x = send(sfd, data, size_packet, 0)) != size_packet)
		er("send()", x);
}

void send_file(struct packet* hp, struct packet* data, int sfd, FILE* f)
{
	int x;
	int i = 0, j = 0;
	while(!feof(f))
	{
		memset(hp->buffer, '\0', sizeof(char) * LENBUFFER);
		hp->datalen = fread(hp->buffer, 1, LENBUFFER - 1, f);
		i += hp->datalen;

		data = htonp(hp);
		if((x = send(sfd, data, size_packet, 0)) != size_packet)
			er("send()", x);
		j++;
	}
	fprintf(stderr, "\t%d byte ricevuti.\n", i);
	fprintf(stderr, "\t%d data packet inviati.\n", j);
	fflush(stderr);
}

void receive_file(struct packet* hp, struct packet* data, int sfd, FILE* f)
{
	int x;
	int i = 0, j = 0;
	if((x = recv(sfd, data, size_packet, 0)) <= 0)
		er("recv()", x);
	j++;
	hp = ntohp(data);

	while(hp->type == DATA)
	{
		i += fwrite(hp->buffer, 1, hp->datalen, f);
		if((x = recv(sfd, data, size_packet, 0)) <= 0)
			er("recv()", x);
		j++;
		hp = ntohp(data);
		//printpacket(hp, HP);
	}
	fprintf(stderr, "\t%d data packet ricevuti.\n", --j);	// j decrementa perchè l'ultimo packet è EOT.
	fprintf(stderr, "\t%d byte(s) scritti.\n", i);
	if(hp->type == EOT)
		return;
	else
	{
		fprintf(stderr, "Errore mentre si eseguiva il download dei file remoti.\n");
		exit(2);
	}
	fflush(stderr);
}

