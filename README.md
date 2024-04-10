Specifiche del problema

Il progetto in questione è volto ad approfondire le conoscenze acquisite con il linguaggio 
C, unite al Livello di Applicazioni delle Reti di Calcolatori con particolare riferimento al 
File Transfer Protocol (FTP) basato sullo standard RFC959, volto alla trasmissione di dati 
sfruttando un’architettura client-server.

Il software crea una connessione bidirezionale tra client e server. Il funzionamento di 
base consiste nell’avvio del server che rimane in attesa di un client e dopo aver eseguito
i comandi o essersi disconnesso, rende nuovamente disponibile il server per un  altro client. 
Non è stato possibile creare una doppia connessione tra client e server per dividere
il flusso di dati dal flusso di controllo, assente quindi la logica di autenticazione dei client. 

Con l’utilizzo del FTP si prefiggono, principalmente, quattro obbiettivi: 
1) promuovere la condivisione di file
2) promuovere indirettamente l' uso di computer remoti
3) proteggere le risorse di host dalle modifiche ad opera di utenti
4) trasferire dati in modo efficiente e corretto.
