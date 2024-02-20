/**********************************************************************************/
/*          ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO DA SEÇÃO DO MENU          */
/**********************************************************************************/
typedef enum{ //ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO DA SEÇÃO DO MENU
	M1 = 1,
	M1_1,
	M1_2
}menu;
/**********************************************************************************/
/*          ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO DE TECLAS                 */
/**********************************************************************************/
typedef enum{
	OK = 1,
	BCK,
	UP,
	DW,
}tecla;
/**********************************************************************************/
/*             ESTRUTURAS DE DE VARIAVEIS PARA MANIPULAÇÃO DO MENU                */
/**********************************************************************************/
typedef struct{
	uint8_t 	trava;
	uint8_t 	menu;
	uint8_t 	menu_flag[5];
	uint8_t 	menu_aux[5];
	uint8_t 	menu_aux1;
	uint8_t 	bt;
}men;
/*
->FLAG.MENU = PERTINENTE AS POSIÇOES DO MENU PRINCIPAL
->FLAG.TRAVA = INTERTRAVAMENTO DOS SUBMENUS PARA O MENU PRINCIPAL
->FLAG.MENU_FLAG[0] = INDICA SE HÁ UM SUBMENU ATIVO (EVITA QUE ENTRE NUMA CONDICIONAL ERRADA)
->FLAG.MENU_FLAG[1] = INDICA SE HÁ UM SUBSUBMENU ATIVO (EVITA QUE ENTRE NUMA CONDICIONAL ERRADA)
->FLAG.MENU_AUX1 = AUXILIAR PRA SUBTELAS DA TELA DE DESCANSO
->FLAG.MENU_AUX[1] = VARIAVEL QUE ITERA SOBRE AS OPÇOES DOS SUBMENUS E EVITA QUE ENTRE NUMA CONDICIONAL ERRADA
->FLAG.MENU_AUX[2] = VARIAVEL QUE ITERA SOBRE AS OPÇOES DOS SUBSUBMENUS E EVITA QUE ENTRE NUMA CONDICIONAL ERRADA
->FLAG.MENU_AUX[3] = VARIAVEL QUE ITERA SOBRE AS OPÇOES DOS SUBSUBSUBMENUS E EVITA QUE ENTRE NUMA CONDICIONAL ERRADA
*/
uint8_t
/*----*/sem,
/*----*/dia,
/*----*/diasem,
/*----*/mes,
/*----*/ano,
/*----*/hr,
/*----*/min,
/*----*/tela4,
/*----*/sec = 0x00;

men flag= {.trava = 0x0, .menu = 0x1, .menu_aux = 0x0, .menu_aux1 = 0x1, .menu_flag = 0x0, .bt = 0x0};
void trata_teclas(void){
	/**********************************************************************************/
	/*                           TRATA EVENTO TECLA CONF                              */
	/**********************************************************************************/
	if(!HAL_GPIO_ReadPin(TCF_GPIO_Port, TCF_Pin)){
		deboucing = 50;										//CONTADOR DEBOUCING
		cont[1]=0x00;
		while(deboucing>0){
			deboucing--;									//DECREMENTO DO DEBOUCING
		}
		if(!flagespera)
			flag.bt = OK;									//SETA FLAG DE BOTÃO ACIONADO
	}
	while(!HAL_GPIO_ReadPin(TCF_GPIO_Port, TCF_Pin)){		//SEGURA O PROGRAMA ENQUANTO O BOTÃO
		//asm("nop");										//CONTINUAR PRESSIONADO
	}
	if(flag.bt)beep();
	/**********************************************************************************/
	/*                              TRATA TECLA CONF                                  */
	/**********************************************************************************/
	/*---SAI DA TELA DE DESCANSO PRO MENU 1---*/
	if(flag.bt==OK && flag.menu==0){
		flag.bt = 0;
		flag.menu = 1;
		flag.trava = 0;
		tela4 = 0;
	}
	/*--ENTRA NA PRIMERA OPÇÃO DO MENU 1---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==0){		 /*/FLAG.MENU INDICA A POSIÇÃO DO MENU 1/*/
		flag.bt= 0;
		flag.trava = 1;
		flag.menu_aux[1] = 1;				/*CAI NA PRIMEIRA OPÇÃO DO SUBMENU 1*/
		flag.menu_flag[0] = M1; 			/*SINALIZA EM QUAL MENU ESTA, PARA LOGICA DOS BOTOES*/
		flag.menu_flag[1] = 0;				/*NENHUM SUBSUBMENU SELECIONADO*/
	}
	/*---ENTRA NO SUBMENU 1-1 PRIMEIRA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==0 && flag.menu_aux[1]==1){		/*/FLAG.MENU AUX[1] INDICA A PRIMEIRA OPÇÃO DO SUBMENU M1(FLAG.MENU_FLAG[0])/*/
		flag.bt = 0;
		flag.menu_aux[2] = 1;				/*CAI NA PRIMEIRA OPÇÃO DO SUBSUBMENU*/
		flag.menu_flag[1] = M1_1;			/*SINALIZA EM QUAL MENU ESTA, PARA LOGICA DOS BOTOES*/
	}
	/*---CONF SUBMENU 1-1---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1){			/*/FLAG.MENU_FLAG[1] INDICA O SUBSUBMENU M1_1 ATIVO/*/
		flag.bt = 0;
		flag.menu_aux[2]++;					/*A CADA OK PRESSIONADO ITERA NAS OPÇOES DO MENU ATUAL*/
		if(flag.menu_aux[2]>4){				/*AO CHEGAR NA ULTIMA OPÇÃO VOLTA AO MENU ANTERIOR*/
			flag.menu_aux[1]=1;				/*VOLTA NA PRIMEIRA OOPÇÃO DO MENU ANTERIOR*/
			flag.menu_flag[1] = 0;			/*LIMPA A FLAG DE SINALIZAÇÃO DO SUBSUBMENU*/
		}
		set_date();	//*FUNÇÃO PRATA GUARDAR A DATA DO RTC*//
	}
	/*---ENTRA NO SUBMENU 1-2 SEGUNDA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==0 && flag.menu_aux[1]==2){		/*/FLAG.MENU AUX[1] INDICA A SEGUNDA OPÇÃO DO SUBMENU M1(FLAG.MENU_FLAG[0])/*/
		flag.bt = 0;
		flag.menu_aux[2] = 1;				/*CAI NA SEGUNDA OPÇÃO DO SUBSUBMENU AO APERTAR OK*/
		flag.menu_flag[1] = M1_2;			/*SINALIZA EM QUAL MENU ESTA, PARA LOGICA DOS BOTOES*/

	}
	/*---CONF SUBMENU 1-2---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_2){			/*/FLAG.MENU_FLAG[1] INDICA O SUBSUBMENU M1_2 ATIVO/*/
		flag.bt = 0;
		flag.menu_aux[2]++;					/*A CADA OK PRESSIONADO ITERA NAS OPÇOES DO MENUI ATUAL*/
		if(flag.menu_aux[2]>2){				/*AO CHEGAR NA ULTIMA OPÇÃO VOLTA AO MENU ANTERIOR*/
			flag.menu_aux[1]=2;				/*VOLTA NA SEGUNDA OOPÇÃO DO MENU ANTERIOR*/
			flag.menu_flag[1] = 0;			/*LIMPA A FLAG DE SINALIZAÇÃO DO SUBSUBMENU*/
		}
		set_time(); //*FUNÇÃO PRATA GUARDAR AS HORAS DO RTC*//
	}
	/*============================================================================================================================*/
	else{
		flag.bt = 0;
		flagTx = 0;
		flagRx = 0;
	}
	/**********************************************************************************/
	/*                           TRATA EVENTO TECLA VOLTA                             */
	/**********************************************************************************/
	if(!HAL_GPIO_ReadPin(TVL_GPIO_Port, TVL_Pin)){
		deboucing = 50;							//CONTADOR DEBOUCING
		cont[1]=0x00;
		while(deboucing>0){
			deboucing--;						//DECREMENTO DO DEBOUCING
		}
		if(!flagespera)
			flag.bt = BCK;							//SETA FLAG DE BOTÃO ACIONADO
	}
	while(!HAL_GPIO_ReadPin(TVL_GPIO_Port, TVL_Pin)){	//SEGURA O PROGRAMA ENQUANTO O BOTÃO
		//asm("nop");								//CONTINUAR PRESSIONADO
	}
	if(flag.bt)beep();
	/**********************************************************************************/
	/*                               TRATA TECLA VOLTA                                */
	/**********************************************************************************/
	/*---VOLTA PARA O MENU SUBTELAS---*/
	if(flag.bt==BCK && flag.menu==0 && flag.menu_aux1>0 && flag.trava==0 && flag.menu_aux[1]==0){
		flag.bt = 0;
		flag.menu = 0;
		flag.menu_aux1 = 1;
		tela4 = 0;
	}
	/*---VOLTA PARA O MENU DE DESCANSO---*/
	if(flag.bt==BCK && flag.menu>0 && flag.trava==0 && flag.menu_aux[1]==0){
		flag.bt = 0;
		flag.menu = 0;
		flag.menu_aux1 = 1;
	}
	/*---VOLTA DO SUBMENU 1 PARA O MENU 1---*/
	else if(flag.bt==BCK && flag.menu==1 && flag.menu_flag[0]==M1  && flag.menu_flag[1]==0 && flag.trava==1){
		flag.bt = 0;
		flag.menu_aux[1] = 0;
		flag.menu_flag[0] = 0;
		flag.trava = 0;
	}
	/*---VOLTA DO SUBMENU 1-1 PARA O SUBMENU1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1){
		flag.bt = 0;
		flag.menu_aux[2]--;
		if(flag.menu_aux[2]<1){
			flag.menu_flag[1] = 0;
			flag.menu_aux[1]=1;
		}
	}
	/*---VOLTA DO SUBMENU 1-2 PARA O SUBMENU1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_2){
		flag.bt = 0;
		flag.menu_aux[2]--;
		if(flag.menu_aux[2]<1){
			flag.menu_flag[1] = 0;
			flag.menu_aux[1]=2;
		}
	}
	else{
		flag.bt = 0;
		flagTx = 0;
		flagRx = 0;
	}
	/**********************************************************************************/
	/*                           TRATA EVENTO TECLA CIMA                              */
	/**********************************************************************************/
	if(!HAL_GPIO_ReadPin(TCM_GPIO_Port, TCM_Pin)){
		deboucing = 50;												//CONTADOR DEBOUCING
		cont[1]=0x00;
		while(deboucing>0){
			deboucing--;											//DECREMENTO DO DEBOUCING
		}
		if(!flagespera)
			flag.bt = UP;												//SETA FLAG DE BOTÃO ACIONADO
	}
	while(!HAL_GPIO_ReadPin(TCM_GPIO_Port, TCM_Pin)){				//SEGURA O PROGRAMA ENQUANTO O BOTÃO
		//asm("nop");												//CONTINUAR PRESSIONADO
	}
	if(flag.bt)beep();
	/**********************************************************************************/
	/*                              TRATA TECLA CIMA                                  */
	/**********************************************************************************/
	/*---MOVIMENTA SUBTELAS ---*/
	if(flag.bt==UP && flag.menu==0 && flag.trava==0 && flag.menu_aux[1]==0){
		flag.bt = 0;
		flag.menu_aux1--;
		if(flag.menu_aux1<1){
			flag.menu_aux1 = 5;
		}
	}
	/*---MOVIMENTA MENU 1---*/
	if(flag.bt==UP && flag.menu>0 && flag.trava==0 && flag.menu_aux[1]==0){
		flag.bt = 0;
		flag.menu--;
		if(flag.menu<1){
			flag.menu = 4;
		}
	}
	/*---MOVIMENTA SUBMENU 1---*/
	else if(flag.bt==UP && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==0 && flag.trava==1){
		flag.bt = 0;
		flag.menu_aux[1]--;
		if(flag.menu_aux[1]<1){
			flag.menu_aux[1] = 2;
		}
	}
	/*---SEÇÃO DATA---*/
	/*---INCREMENTO DA VARIAVEL MES---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==1){
		flag.bt = 0;
		mes++;
		if(mes>12){
			mes = 1;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIA---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==2){
		flag.bt = 0;
		dia++;
		if(mes==2){
			if(dia>28)dia=1;
		}
		else if(mes==4 || mes==6 || mes==9 || mes==11){
			if(dia>30)dia = 1;
		}
		else{
			if(dia>31)dia=1;
		}
	}
	/*---INCREMENTO DA VARIAVEL ANO---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==3){
		flag.bt = 0;
		ano++;
		if(ano>99)ano=00;
	}
	/*---INCREMENTO DA VARIAVEL DIA DA SEMANA---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==4){
		flag.bt = 0;
		diasem++;
		if(diasem>7)diasem=1;
	}
	/*---INCREMENTO DA VARIAVEL HORA---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_2 && flag.menu_aux[2]==1){
		flag.bt = 0;
		hr++;
		if(hr>23){
			hr = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL MINUTOS---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_2 && flag.menu_aux[2]==2){
		flag.bt = 0;
		min++;
		if(min>59){
			min = 0;
		}
	}
	/*============================================================================================================================*/
	else{
		flag.bt = 0;
		flagTx = 0;
		flagRx = 0;
	}
	/**********************************************************************************/
	/*                           TRATA EVENTO TECLA BAIXO                             */
	/**********************************************************************************/
	if(!HAL_GPIO_ReadPin(TBX_GPIO_Port, TBX_Pin)){
		deboucing = 50;							//CONTADOR DEBOUCING
		cont[1]=0x00;
		while(deboucing>0){
			deboucing--;						//DECREMENTO DO DEBOUCING
		}
		if(!flagespera)
			flag.bt = DW;							//SETA FLAG DE BOTÃO ACIONADO
	}
	while(!HAL_GPIO_ReadPin(TBX_GPIO_Port, TBX_Pin)){	//SEGURA O PROGRAMA ENQUANTO O BOTÃO
		//asm("nop");								//CONTINUAR PRESSIONADO
	}
	if(flag.bt)beep();
	/**********************************************************************************/
	/*                               TRATA TECLA BAIXO                                */
	/**********************************************************************************/
	/*---MOVIMENTA SUBTELAS ---*/
	if(flag.bt==DW && flag.menu==0 && flag.trava==0 && flag.menu_aux[1]==0){
		flag.bt = 0;
		flag.menu_aux1++;
		if(flag.menu_aux1>5){
			flag.menu_aux1=1;
		}
	}
	/*---MOVIMENTA MENU 1---*/
	if(flag.bt==DW && flag.menu>0 && flag.trava==0 && flag.menu_aux[1]==0){
		flag.bt = 0;
		flag.menu++;
		if(flag.menu>4){
			flag.menu=1;
		}
	}
	/*---MOVIMENTA SUBMENU 1---*/
	else if(flag.bt==DW && flag.menu==1 && flag.menu_flag[0]==M1 && flag.trava==1 && flag.menu_flag[1]==0){
		flag.bt = 0;
		flag.menu_aux[1]++;
		if(flag.menu_aux[1]>2){
			flag.menu_aux[1] = 1;
		}
	}
	/*---DECREMENTO DA VARIAVEL MES---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==1){
		flag.bt = 0;
		mes--;
		if(mes<1){
			mes = 12;
		}
	}
	/*---DECREMENTO DA VARIAVEL DIA---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==2){
		flag.bt = 0;
		dia--;
		if(mes==2){
			if(dia<1)dia=28;
		}
		else if(mes==4 || mes==6 || mes==9 || mes==11){
			if(dia<1)dia = 30;
		}
		else{
			if(dia<1)dia=31;
		}
	}
	/*---DECREMENTO DA VARIAVEL ANO---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==3){
		flag.bt = 0;
		if(ano<01)ano=100;
		ano--;
	}
	/*---DECREMENTO DA VARIAVEL DIA DA SEMANA---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1 && flag.menu_aux[2]==4){
		flag.bt = 0;
		if(diasem<1)diasem=8;
		diasem--;
	}
	/*---SEÇÃO HORA---*/
	/*---DECREMENTO DA VARIAVEL HORA---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_2 && flag.menu_aux[2]==1){
		flag.bt = 0;
		if(hr<1){
			hr = 24;
		}
		hr--;
	}
	/*---DECREMENTO DA VARIAVEL MINUTOS---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_2 && flag.menu_aux[2]==2){
		flag.bt = 0;
		if(min<1){
			min = 60;
		}
		min--;
	}
		else{
		flag.bt = 0;
		flagTx = 0;
		flagRx = 0;
	}
}

void atualiza_menu(void){
	char date[]="02/11/22";
	char horas[]="00:00";
	
	uint16_t aux1 = 0x00;
	uint16_t aux2 = 0x00;
	switch (flag.menu){
	case 0:
		switch(flag.menu_aux1){
		case 1:
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_10);
			OLED_DrawStr(temp, 48, 29, 1);
			OLED_DrawStr(dat, RIGHT, 29, 1);
			if(!init){OLED_DrawStr("Iniciando...", 45, 1, 1);}
			if(aux.flagicone[1] && init ){
				OLED_DrawXBM(57, 1, icon_chave);
				if(conttg[0]>=600){
					tggl1 = !tggl1;
					conttg[0] = 0;
				}
				OLED_DrawStr("BO", 40, 1, tggl1);
				OLED_DrawRectangle(39, 0, 71, 14);
			}
			if(aux.flagicone[0] && init){
				OLED_DrawXBM(91, 1, icon_chave);
				if(conttg[1]>=600){
					tggl2 = !tggl2;
					conttg[1] = 0;
				}
				OLED_DrawStr("BA", 74, 1, tggl2);
				OLED_DrawRectangle(73, 0, 105, 14);
			}

			OLED_DrawXBM(109, 1, icon_bateria);
			/*LIMITA CASAS DECIMAIS HORIMETRO*/
			if(hori.hr1>99 && hori.hr1<999)sprintf(horim,"H:%03d:%02d", hori.hr1, hori.min1[0]);
			else if(hori.hr1>999 && hori.hr1<9999)sprintf(horim,"H:%04d:%02d", hori.hr1, hori.min1[0]);
			else if(hori.hr1>9999)sprintf(horim,"H:%05:%02d", hori.hr1, hori.min1[0]);
			else sprintf(horim,"H:%02d:%02d", hori.hr1, hori.min1[0]);
			OLED_DrawStr(horim, RIGHT, 41, 1);
			/*LIMITA CASAS DECIMAIS TEMPORIZADOR*/
			if(hori.temp1>99 && hori.temp1<999)sprintf(tempo,"T:%03d:%02d", hori.temp1, hori.min1[1]);
			else if(hori.temp1>999 && hori.temp1<9999 )sprintf(tempo,"T:%04d:%02d", hori.temp1, hori.min1[1]);
			else if(hori.temp1>9999)sprintf(tempo,"T:%05d:%02d", hori.temp1, hori.min1[1]);
			else sprintf(tempo,"T:%02d:%02d", hori.temp1, hori.min1[1]);
			if(flagh[0] && !flagh[2]){
				OLED_DrawStr(tempo, RIGHT, 53, 1);
			}
			else if(flagh[2]){
				if(conttg[2]>600){
					psc = ~psc;
					conttg[2] = 0;
				}
				if(psc){
					OLED_DrawStr(tempo, RIGHT, 53, 1);
				}
			}
			if(batt<8.0 || batt>16.5){
				if(conttg[5]>600){
					tggl3 = !tggl3;
					conttg[5] = 0;
				}
			}
			else tggl3 = 1;

			OLED_DrawStr(tensao, 25, 50, tggl3);
			OLED_DrawRectangle(24, 49, 53, 63);
			if((pressdec/100)<pmin){
				if(conttg[6]>600){
					tggl4 = !tggl4;
					conttg[6] = 0;
				}
				OLED_DrawStr("P-", 34, 16, tggl4);
				OLED_DrawRectangle(32, 16, 45, 29);
			}
			if((pressdec/100)>pmax){
				if(conttg[6]>600){
					tggl4 = !tggl4;
					conttg[6] = 0;
				}
				OLED_DrawStr("P+", 31, 16, tggl4);
				OLED_DrawRectangle(30, 16, 45, 29);
			}

			indicador(altura_sensor);
			OLED_DrawStr("Pres:", 48, 16, 1);
			OLED_DrawStr(pressao1, 80, 16, 1);
			OLED_DrawStr("Bar", RIGHT, 16, 1);

			OLED_UpdateScreen();
			break;
		case 2:
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_12);
			switch (diasem){
			case 1:
				OLED_DrawStr("Segunda", CENTER, 1, 1);
				break;
			case 2:
				OLED_DrawStr("Terca", CENTER, 1, 1);
				break;
			case 3:
				OLED_DrawStr("Quarta", CENTER, 1, 1);
				break;
			case 4:
				OLED_DrawStr("Quinta", CENTER, 1, 1);
				break;
			case 5:
				OLED_DrawStr("Sexta", CENTER, 1, 1);
				break;
			case 6:
				OLED_DrawStr("Sabado", CENTER, 1, 1);
				break;
			case 7:
				OLED_DrawStr("Domingo", CENTER, 1, 1);
				break;
			}
			OLED_DrawStr(dat, CENTER, 16, 1);
			sprintf((char*)tempteste,"%02d:%02d:%02d", hr, min, sec);
			OLED_DrawStr(tempteste, CENTER, 33, 1);
			OLED_UpdateScreen();
			break;
			case 3:
				tela4 = 0;
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_12);
				uint32_t* data_ptr = (uint32_t*)eepromini;
				uint32_t read_data = *data_ptr;
				sprintf(teste,"%X", read_data);
				OLED_DrawStr(teste, CENTER, 49, 1);
				OLED_DrawStr("Horimetro", CENTER, 1, 1);
				if(hori.hr1>99 && hori.hr1<999)sprintf(horim,"H:%03d:%02d", hori.hr1, hori.min1[0]);
				else if(hori.hr1>999 && hori.hr1<9999)sprintf(horim,"H:%04d:%02d", hori.hr1, hori.min1[0]);
				else if(hori.hr1>9999)sprintf(horim,"H:%05:%02d", hori.hr1, hori.min1[0]);
				else sprintf(horim,"H:%02d:%02d", hori.hr1, hori.min1[0]);
				OLED_DrawStr(horim, CENTER, 16, 1);
				/*LIMITA CASAS DECIMAIS TEMPORIZADOR*/
				if(hori.temp1>99 && hori.temp1<999)sprintf(tempo,"T:%03d:%02d", hori.temp1, hori.min1[1]);
				else if(hori.temp1>999 && hori.temp1<9999 )sprintf(tempo,"T:%04d:%02d", hori.temp1, hori.min1[1]);
				else if(hori.temp1>9999)sprintf(tempo,"T:%05d:%02d", hori.temp1, hori.min1[1]);
				else sprintf(tempo,"T:%02d:%02d", hori.temp1, hori.min1[1]);
				if(flagh[0] && !flagh[2]){
					OLED_DrawStr(tempo, CENTER, 33, 1);
				}
				OLED_UpdateScreen();
				break;
			case 4:
				tela4 = 1;
				OLED_UpdateScreen();
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_12);
				OLED_DrawStr("Nivel", CENTER, 16, 1);
				indicador(altura_sensor);
				OLED_UpdateScreen();
				break;
			case 5:
				tela4 = 0;
				pressao2[0] = pressao1[0];
				pressao2[1]	= pressao1[1];
				pressao2[2] = pressao1[2];
				pressao2[3] = pressao1[3];
				pressao2[4] = pressao1[4];
				pressao2[5]	= pressao1[5];
				OLED_Clear(0);
				FontSet(Segoe_UI_Eng_12);
				if((pressdec/100)<pmin){
					if(conttg[6]>600){
						tggl4 = !tggl4;
						conttg[6] = 0;
					}
					OLED_DrawStr("Pressao-", CENTER, 16, tggl4);
					OLED_DrawRectangle(35, 15, 90, 31);
				}
				else if((pressdec/100)>pmax){
					if(conttg[6]>600){
						tggl4 = !tggl4;
						conttg[6] = 0;
					}
					OLED_DrawStr("Pressao+", CENTER, 16, tggl4);
					OLED_DrawRectangle(34, 15, 91, 31);
				}
				else
					OLED_DrawStr("Pressao", CENTER, 16, 1);

				OLED_DrawStr(pressao2, CENTER, 33, 1);
				OLED_DrawStr("Bar  " , RIGHT, 33, 1);
				OLED_UpdateScreen();
				break;
		}
		break;
		case 1:
			if(!flag.trava && init){
				menuprincipal(0, 1, 1, 1);
			}
			else if(flag.trava && flag.menu_flag[0]==M1 && init){
				switch (flag.menu_aux[1]){
				case 1:
					if(flag.menu_flag[1]==0){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Config data.      ", 1, 1, 0);
						OLED_DrawStr("Config hora.      ", 1, 15, 1);
						OLED_UpdateScreen();
					}
					else if(flag.menu_flag[1]==M1_1){
						switch (flag.menu_aux[2]){
						case 1:
							OLED_Clear(0);
							sprintf(date,"%02d/%02d/%02d", dia, mes, (ano));
							OLED_DrawStr("Mes", CENTER, 1, 1);
							OLED_DrawStr(date, CENTER, 15, 1);
							OLED_UpdateScreen();
							break;
						case 2:
							OLED_Clear(0);
							sprintf(date,"%02d/%02d/%02d", dia, mes, (ano));
							OLED_DrawStr("Dia", CENTER, 1, 1);
							OLED_DrawStr(date, CENTER, 15, 1);
							OLED_UpdateScreen();
							break;
						case 3:
							OLED_Clear(0);
							sprintf(date,"%02d/%02d/%02d", dia, mes, (ano));
							OLED_DrawStr("Ano", CENTER, 1, 1);
							OLED_DrawStr(date, CENTER, 15, 1);
							OLED_UpdateScreen();
							break;

						case 4:
							OLED_Clear(0);
							OLED_DrawStr("Dia da semana", CENTER, 1, 1);
							switch (diasem){
							case 1:
								OLED_DrawStr("Segunda", CENTER, 16, 1);
								break;
							case 2:
								OLED_DrawStr("Terca", CENTER, 16, 1);
								break;
							case 3:
								OLED_DrawStr("Quarta", CENTER, 16, 1);
								break;
							case 4:
								OLED_DrawStr("Quinta", CENTER, 16, 1);
								break;
							case 5:
								OLED_DrawStr("Sexta", CENTER, 16, 1);
								break;
							case 6:
								OLED_DrawStr("Sabado", CENTER, 16, 1);
								break;
							case 7:
								OLED_DrawStr("Domingo", CENTER, 16, 1);
								break;
							}
							OLED_UpdateScreen();
							break;
						}
					}
					break;
				case 2:
					if(flag.menu_flag[1]==0){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Config data.      ", 1, 1, 1);
						OLED_DrawStr("Config hora.      ", 1, 15, 0);
						OLED_UpdateScreen();
					}
					else if(flag.menu_flag[1]==M1_2){
						switch (flag.menu_aux[2]){
						case 1:
							OLED_Clear(0);
							sprintf(horas,"%02d:%02d", hr, min);
							OLED_DrawStr("Horas", CENTER, 1, 1);
							OLED_DrawStr(horas, CENTER, 15, 1);
							OLED_UpdateScreen();
							break;
						case 2:
							OLED_Clear(0);
							sprintf(horas,"%02d:%02d", hr, min);
							OLED_DrawStr("Minutos", CENTER, 1, 1);
							OLED_DrawStr(horas, CENTER, 15, 1);
							OLED_UpdateScreen();
							break;
						}
					}
					break;
				}/*FIM SWITCH flag.menu_aux[1]*/
			}/*FIM ELSE IF DENTRO DO CASE 1*/
			break;
		}
}