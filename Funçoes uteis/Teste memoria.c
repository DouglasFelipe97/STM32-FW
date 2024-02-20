	
// Função para apagar uma página na EEPROM
void apagar_pagina(EE24_HandleTypeDef *handle, uint32_t pagina) {
    // Cria um buffer com 64 bytes, todos com valor 0xFF
    uint8_t buffer[64];
    for (size_t i = 0; i < sizeof(buffer); i++) {
        buffer[i] = 0xFF;
    }
    // Escreve o buffer na página da EEPROM
    EE24_Write(handle, pagina, buffer, sizeof(buffer), 10); // Use o timeout apropriado
}
// Função para apagar 1K de memória na EEPROM
void apagar_1K_eeprom(uint32_t endereco) {
    uint16_t pagina;	
    // Apagar 16 páginas (1K / 64 bytes por página = 16 páginas)
    for (pagina = 0; pagina < 16; pagina++, endereco+=64) {
        apagar_pagina(&e2prom, endereco);
    }
}

void verifica_manutencao(void){
	uint16_t aux1, aux2, aux3, aux4, aux5 = 0x00;

	aux1 = inter.primeira_man[1];
	aux2 = inter.intervalo_man[1];
	aux3 = inter.primeira_man[0];
	aux4 = inter.intervalo_man[0];
	aux5 = inter.limpeza[0];
	if(aux1<5)inter.primeira_man[1] = 30;
	if(aux2<10)inter.intervalo_man[1] = 100;
	if(aux3<5)inter.primeira_man[0] = 30;
	if(aux4<10)inter.intervalo_man[0] = 100;
	if(aux5<5)inter.limpeza[0] = 100;

	/**********************************************************************************/
	/*                      TRATA MANUTENÇÃO 100 HORAS BARRA                          */
	/**********************************************************************************/
	if(hori.hr1==(aux1-5) && aux.manaux[0]==(aux1-5) && !aux.flagman[0]){ 		//HORIMETRO COM DE 30 HORAS E MANUTENÇÃO NAO FEITA PREVIAMENTE
		aux.flagicone[0] = 1;					//HABILITA ICONE E AVISO MANUTENÇÃO
	}

	if(aux.manaux[0]>=(aux2-10) && !aux.flagman[0]){   //HORIMETRO COM DE 100 HORAS E MANUTENÇÃO NAO FEITA PREVIAMENTE
		aux.flagicone[0] = 1;					//HABILITA ICONE E AVISO MANUTENÇÃO
	}
	else if(aux.flagman[0]){
		aux.flagicone[0] = 0;					//MANTEM O ICONE DE MANUTENÇÃO DESLIGADO
		aux.flagman[0] = 0;						//ZERA A FLAG DE MANUTENÇÃO
		aux.flagad[0] = 0;						//ZERA A FLAG DE MANUTENÇÃO ADIANTADA
		aux.horas[0] +=aux.manaux[0]; 			//TOTAL DE HORAS QUANDO FOI FEITA A MANUTENÇÃO
		aux.manaux[0]=0x0;						//ZERA O CONTADOR DE MANUTENÇÃO QUE ACIONA A FLAG DE AVISO
		for(uint8_t aux=0x01; aux!=0x00/*aux!=0xFF*/; indexbr+=1){
			EE24_Read_8(&e2prom, (memobr+indexbr), &aux, sizeof(aux), 1);
		}
		if(indexbr==1)indexbr-=1;
		else {
			indexbr-=1;
			EE24_Read_8(&e2prom, (memobr+indexbr), &aux.cont[0], sizeof(aux.cont[0]), 1);
		}

		/*=============================================================================================================================*/
		if(indexbr < 0x400){		//---PAGINA INTEIRA---//
			EE24_Write_32(&e2prom, (memobr+indexbr), aux.horas[0], sizeof(aux.horas[0]), 5);
			indexbr+=4;

			EE24_Write_8(&e2prom, (memobr+indexbr), dia, sizeof(dia), 5);
			indexbr+=1;

			EE24_Write_8(&e2prom, (memobr+indexbr), mes, sizeof(mes), 5);
			indexbr+=1;

			EE24_Write_8(&e2prom, (memobr+indexbr), ano, sizeof(ano), 5);
			indexbr+=1;

			aux.cont[0]+=1;
			EE24_Write_8(&e2prom, (memobr+indexbr), aux.cont[0], sizeof(aux.cont[0]), 5);
			indexbr+=1;
		}
		else{
			apagar_1K_eeprom(&e2prom, memobr);		//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
			indexbr = 0x0;

			EE24_Write_32(&e2prom, (memobr+indexbr), aux.horas[0], sizeof(aux.horas[0]), 5);
			indexbr+=4;

			EE24_Write_8(&e2prom, (memobr+indexbr), dia, sizeof(dia), 5);
			indexbr+=1;

			EE24_Write_8(&e2prom, (memobr+indexbr), mes, sizeof(mes), 5);
			indexbr+=1;

			EE24_Write_8(&e2prom, (memobr+indexbr), ano, sizeof(ano), 5);
			indexbr+=1;

			aux.cont[0]+=1;
			EE24_Write_8(&e2prom, (memobr+indexbr), aux.cont[0], sizeof(aux.cont[0]), 5);
			indexbr+=1;
		}
		/*=============================================================================================================================*/
		aux.horas[0] = 0x00;
	}

	/**********************************************************************************/
	/*                      TRATA MANUTENÇÃO 100 HORAS BOMBA D'AGUA                   */
	/**********************************************************************************/

	if(hori.hr1==(aux3-5) && aux.manaux[1]==(aux3-5) && !aux.flagman[1]){ 		//HORIMETRO COM DE 30 HORAS E MANUTENÇÃO NAO FEITA PREVIAMENTE
		aux.flagicone[1] = 1;					//HABILITA ICONE E AVISO MANUTENÇÃO
	}
	if(aux.manaux[1]>=aux4 && !aux.flagman[1]){   //HORIMETRO COM DE 100 HORAS E MANUTENÇÃO NAO FEITA PREVIAMENTE
		aux.flagicone[1] = 1;					//HABILITA ICONE E AVISO MANUTENÇÃO
	}
	else if(aux.flagman[1]){
		aux.flagicone[1] = 0;					//MANTEM O ICONE DE MANUTENÇÃO DESLIGADO
		aux.flagman[1] = 0;						//ZERA A FLAG DE MANUTENÇÃO
		aux.flagad[1] = 0;						//ZERA A FLAG DE MANUTENÇÃO ADIANTADA
		aux.horas[1] +=aux.manaux[1]; 			//TOTAL DE HORAS QUANDO FOI FEITA A MANUTENÇÃO
		aux.manaux[1]=0x0;						//ZERA O CONTADOR DE MANUTENÇÃO QUE ACIONA A FLAG DE AVISO
		for(uint8_t aux=0x01; aux!=0x00/*aux!=0xFF*/; indexba+=1){
			EE24_Read_8(&e2prom,(memoba+indexba), &aux, sizeof(aux), 1);
		}
		if(indexba==1)indexba-=1;
		else {
			indexba-=1;
			EE24_Read_8(&e2prom,(memoba+indexba), &aux.cont[1], sizeof(aux.cont[1]), 1);
		}

		if(indexba < 0x400){			//---PAGINA INTEIRA---//
			EE24_Write_32(&e2prom, (memoba+indexba), aux.horas[1], sizeof(aux.horas[1]), 5);
			indexba+=4;

			EE24_Write_8(&e2prom, (memoba+indexba), dia, sizeof(dia), 5);
			indexba+=1;

			EE24_Write_8(&e2prom, (memoba+indexba), mes, sizeof(mes), 5);
			indexba+=1;

			EE24_Write_8(&e2prom, (memoba+indexba), ano, sizeof(ano), 5);
			indexba+=1;

			aux.cont[1]+=1;
			EE24_Write_8(&e2prom, (memoba+indexba), aux.cont[1], sizeof(aux.cont[1]), 5);
			indexba+=1;
		}
		else{
			apagar_1K_eeprom(&e2prom, memoba);		//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
			indexba = 0x0;

			EE24_Write_32(&e2prom, (memoba+indexba), aux.horas[1], sizeof(aux.horas[1]), 5);
			indexba+=4;

			EE24_Write_8(&e2prom, (memoba+indexba), dia, sizeof(dia), 5);
			indexba+=1;

			EE24_Write_8(&e2prom, (memoba+indexba), mes, sizeof(mes), 5);
			indexba+=1;

			EE24_Write_8(&e2prom, (memoba+indexba), ano, sizeof(ano), 5);
			indexba+=1;

			aux.cont[1]+=1;
			EE24_Write_8(&e2prom, (memoba+indexba), aux.cont[1], sizeof(aux.cont[1]), 5);
			indexba+=1;
		}
		/*=============================================================================================================================*/
		aux.horas[1] = 0x00;
	}
	if(aux.manaux[2]>=(aux5-10) && !aux.flagman[2]){   //HORIMETRO COM DE 100 HORAS E MANUTENÇÃO NAO FEITA PREVIAMENTE
		aux.flagicone[2] = 1;					//HABILITA ICONE E AVISO MANUTENÇÃO
	}
	else if (aux.flagman[2])aux.flagicone[2] = 1;
}

void historico_bagua(void){				//---PAGINA INTEIRA---//
	typedef struct{
		uint16_t	i;
		uint16_t 	aux;
		uint32_t 	hora;
		uint16_t 	dia;
		uint16_t 	mes;
		uint16_t 	ano;
	}hist;

	hist historico = {.aux = 0x0, .hora = 0x0, .dia = 0x0, .mes = 0x0, .ano = 0x0};

	char man1[20] = "Proxima manutencao";
	char man2[20] = "Proxima manutencao";
	char man3[20] = "Proxima manutencao";
	char man4[20] = "Proxima manutencao";

	if(!flaghistba){
		indexba=0x00;
		auxindex = 0X00;
		for(uint16_t aux=0x01; aux!=0x0000 /*aux!=0xFFFF*/; indexba+=1){
			
			EE24_Read_8(&e2prom,(memoba+indexba), &aux, sizeof(aux), 1);
		}
		indexba-=1;
		auxindex += indexba;
		indexba=0x0;
		flaghistba = 1;
	}
	if(!point){
		for(uint16_t aux = 0x0; aux<=4; aux+=1){
			switch (aux){
			case 1:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memoba+indexba), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man1,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 2:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memoba+indexba), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man2,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 3:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memoba+indexba), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man3,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 4:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memoba+indexba), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memoba+indexba), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man4,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}

				break;
			}
		}
		point+=1;
		OLED_Clear(0);
		FontSet(Segoe_UI_Eng_10);
		OLED_DrawStr(man1, 1, 0, 1);
		OLED_DrawStr(man2, 1, 16, 1);
		OLED_DrawStr(man3, 1, 33, 1);
		OLED_DrawStr(man4, 1, 49, 1);
		OLED_UpdateScreen();
	}
}
		/*=============================================================================================================================*/
		void aviso_manutencao(void){
			typedef struct{
				uint16_t	i;
				uint16_t 	aux;
				uint32_t 	hora;
				uint16_t 	dia;
				uint16_t 	mes;
				uint16_t 	ano;
			}hist;

			hist historico = {.aux = 0x0, .hora = 0x0, .dia = 0x0, .mes = 0x0, .ano = 0x0};

			char man1[20] = "Proxima manutencao";
			char man2[20] = "Proxima manutencao";
			char man3[20] = "Proxima manutencao";
			char man4[20] = "Proxima manutencao";

			if(!flaghistb){
				indexbr=0x00;
				auxindex1=0X00;
				for(uint16_t aux=0x01; aux!=0x0000 /*aux!=0xFFFF*/; indexba+=1){
					aux = *(uint32_t*)(memoba+indexba);
				}
				indexbr-=2;
				auxindex1 += indexbr;
				indexbr=0x0;
				flaghistb = 1;
			}
			if(!point){
				for(uint16_t aux = 0x0; aux<=4; aux+=1){
					switch (aux){
					case 1:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memobr+indexbr), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man1,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 2:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memobr+indexbr), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man2,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 3:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memobr+indexbr), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man3,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 4:
				if(auxindex>=indexba+8){
					EE24_Read_32(&e2prom,(memobr+indexbr), &historico.hora, sizeof(historico.hora), 1);
					indexba+=4;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.dia, sizeof(historico.dia), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.mes, sizeof(historico.mes), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.ano, sizeof(historico.ano), 1);
					indexba+=1;
					EE24_Read_8(&e2prom,(memobr+indexbr), &historico.aux, sizeof(historico.aux), 1);
					indexba+=1;
					sprintf(man4,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}

				break;
					}
				}
				point+=1;
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_10);
				OLED_DrawStr(man1, 1, 0, 1);
				OLED_DrawStr(man2, 1, 16, 1);
				OLED_DrawStr(man3, 1, 33, 1);
				OLED_DrawStr(man4, 1, 49, 1);
				OLED_UpdateScreen();
			}
		}
		void aviso_manutencao(void){
			if(aux.flagicone[1] && !aux.flagicone[0]){
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_10);
				OLED_DrawStr("1 manutencao", CENTER, 1, 1);
				OLED_DrawStr("pendentes", CENTER, 14, 1);
				OLED_DrawStr("Bomba d'agua", CENTER, 32, 1);
				OLED_UpdateScreen();
				HAL_Delay(1200);
			}
			else if(!aux.flagicone[1] && aux.flagicone[0]){
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_10);
				OLED_DrawStr("1 manutencao", CENTER, 1, 1);
				OLED_DrawStr("pendentes", CENTER, 14, 1);
				OLED_DrawStr("Oleo barra", CENTER, 32, 1);
				OLED_UpdateScreen();
				HAL_Delay(1200);
			}
			else if(aux.flagicone[1] && aux.flagicone[0]){
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_10);
				OLED_DrawStr("2 manutencoes", CENTER, 1, 1);
				OLED_DrawStr("pendentes", CENTER, 14, 1);
				OLED_DrawStr("Bomba d'agua", CENTER, 32, 1);
				OLED_DrawStr("Oleo barra", CENTER, 47, 1);
				OLED_UpdateScreen();
				HAL_Delay(1200);
			}
			else if(!aux.flagicone[1] && !aux.flagicone[0]){
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_10);
				OLED_DrawStr("Sem manutencoes", CENTER, 1, 1);
				OLED_DrawStr("pendentes", CENTER, 14, 1);
				OLED_UpdateScreen();
				HAL_Delay(1200);
			}
			if(aux.flagicone[2] == 1){
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_10);
				OLED_DrawStr("Limpeza do sensor", CENTER, 1, 1);
				OLED_DrawStr("pendente", CENTER, 14, 1);
				OLED_UpdateScreen();
				HAL_Delay(1200);
			}
		}
		