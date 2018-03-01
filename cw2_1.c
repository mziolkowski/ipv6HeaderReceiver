/*
 ============================================================================
 Name        : cw2.c
 Author      : maciej
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/*
 ============================================================================
 Name        : EthRecv.c
 Author      : Krzysztof Maslanka
 Version     :
 Copyright   :
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include "ipv6hdr.h"

int main(void) {
	struct IPv6hdr *pierwszyElement = NULL;
	struct IPv6hdr *poprzedniEle, *biezacyEle;

	int i = 0;

	printf("Uruchamiam odbieranie ramek Ethernet.\n"); /* prints  */

	//Utworzenie bufora dla odbieranych ramek Ethernet
	char* buffer = (void*) malloc(ETH_FRAME_LEN);
	//Otwarcie gniazda pozwalającego na odbiór wszystkich ramek Ethernet
	int iEthSockHandl = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	//Kontrola czy gniazdo zostało otwarte poprawnie, w przypadku bledu wyświetlenie komunikatu.
	if (iEthSockHandl < 0)
		printf("Problem z otwarciem gniazda : %s!\n", strerror(errno));

	//Zmienna do przechowywania rozmiaru odebranych danych
	int iDataLen = 0;

	//Pętla nieskończona do odbierania ramek Ethernet
	while (1) {

		//Odebranie ramki z utworzonego wcześniej gniazda i zapisanie jej do bufora
		iDataLen = recvfrom(iEthSockHandl, buffer, ETH_FRAME_LEN, 0, NULL,
		NULL);

		//Kontrola czy nie było bledu podczas odbierania ramki
		if (iDataLen == -1)
			printf("Nie moge odebrac ramki: %s! \n", strerror(errno));
		else { //jeśli ramka odebrana poprawnie wyświetlenie jej zawartości
			printf("\nOdebrano ramke Ethernet o rozmiarze: %d [B]\n", iDataLen);
			printf("\n");
			//utworzenie struktury dla ramek eth
			struct ethhdr *eth = (struct ethhdr *) (buffer);
			//utworzenie struktury dla ramek ipv6
			struct IPv6hdr *ipv6 = (struct IPv6hdr *) (buffer
					+ sizeof(struct ethhdr));
			// Sprawdzenie czy odebrany protokol to nasz poszukiwany
			if (eth->h_proto == htons(ETH_P_IPV6)) {
				while (i < 20) {

					// Przydzielenie bloku o rozmiarze struktury. Biezacy element to wsk. na poczatek blok
					biezacyEle = (struct IPv6hdr*) malloc(
							sizeof(struct IPv6hdr));
					if (pierwszyElement == NULL)
						pierwszyElement = biezacyEle;
					else
						poprzedniEle->nowyEle = biezacyEle;
					// przypisanie poszczegolnym polom wartosci z ramki ipv6
					biezacyEle->nowyEle = NULL;
					biezacyEle->ver = ipv6->ver;
					biezacyEle->traf_class = ipv6->traf_class;
					biezacyEle->flow_lab = ipv6->flow_lab;
					biezacyEle->length = ipv6->length;
					biezacyEle->next_header = ipv6->next_header;
					biezacyEle->hop_limit = ipv6->hop_limit;
					for (int j = 0; j <= 15; j++) {
						biezacyEle->src_addr[j] = ipv6->src_addr[j];
						biezacyEle->dest_addr[j] = ipv6->dest_addr[j];
					}
					poprzedniEle = biezacyEle;
					i++;
					break;
				}
				biezacyEle = pierwszyElement;

				// Wyswietlanie danych z ramki
				if (i == 20) {
					while (i > 0 && biezacyEle != NULL) {
						printf("\n");
						printf("\n#IPv6 Header\n");
						printf("\t|-Version: %d\n", biezacyEle->ver);
						printf("\t|-Traffic Class: %d\n",
								biezacyEle->traf_class);
						printf("\t|-Flow Label: %d\n", biezacyEle->flow_lab);
						printf("\t|-Payload Length: %d\n", biezacyEle->length);
						printf("\t|-Next Header: %d\n",
								biezacyEle->next_header);
						printf("\t|-Hop Limit: %d\n", biezacyEle->hop_limit);
						printf("\t|-Source Address      : ");
						for (int j = 0; j <= 15; j += 2) {
							if (j == 14) {
								printf("%x%x", biezacyEle->src_addr[j],
										(int) biezacyEle->src_addr[j + 1]);
							} else
								printf("%x%x-", biezacyEle->src_addr[j],
										(int) biezacyEle->src_addr[j + 1]);
						}
						printf("\n");
						printf("\t|-Destination Address : ");
						for (int j = 0; j <= 15; j += 2) {
							if (j == 14) {
								printf("%x%x", biezacyEle->dest_addr[j],
										biezacyEle->dest_addr[j + 1]);
							} else
								printf("%x%x-", biezacyEle->dest_addr[j],
										biezacyEle->dest_addr[j + 1]);
						}
						i--;
						biezacyEle = biezacyEle->nowyEle;
					}

					printf("\n");
					i = 0;

					// Kasownie elementow z listy wiazanej
					while (biezacyEle != NULL) {
						biezacyEle = pierwszyElement;
						poprzedniEle = biezacyEle;
						biezacyEle = biezacyEle->nowyEle;
						free(poprzedniEle);
					}
				}

			} else {
				printf("\nNie znaleziono pakietu IPv6!!!\n");
			}
		}
	}

	return EXIT_SUCCESS;
}
