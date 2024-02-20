/**********************************************************************************/
/*             ESTRUTURAS DE DE VARIAVEIS PARA MANIPULAÇÃO DO HORIMETRO           */
/**********************************************************************************/
typedef struct{
	uint16_t 	min1[2];
	uint8_t 	aux1[2];
	uint8_t 	aux2[4];
	uint32_t 	hr1;
	uint32_t 	temp1;
}hor;
hor hori= { .min1=0x0, .aux1=0x0, .aux2=0x0, .hr1=0x0, .temp1=0x0};
uint8_t
/*----*/sem,
/*----*/dia,
/*----*/diasem,
/*----*/mes,
/*----*/ano,
/*----*/hr,
/*----*/min,
/*----*/sec = 0x00;

char temp[10];
char dat[10];

void funcoes_horimetro(void){
	if(flag.menu_flag[1]!=M1_2 && flag.menu_flag[1]!=M1_1){
		/* Get the RTC current Time */
		HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
		/* Get the RTC current Date */
		HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
		/* Display time Format: hh:mm:ss */
		hr  = gTime.Hours;
		min = gTime.Minutes;
		sec = gTime.Seconds;
		dia = gDate.Date;
		diasem = gDate.WeekDay;
		mes = gDate.Month;
		ano = (gDate.Year & 0xFF);
		if(ano>100)ano = 0x0;
		sprintf((char*)dat,"%02d/%02d/%02d", dia, mes, (ano));
		sprintf((char*)temp,"%02d:%02d ", hr, min);
		//verifica_manutencao();
	}
	/*---CONTAGEM DO HORIMETRO---*/
	if(hori.aux1[0] != min && flag.menu_flag[1]!=M1_2 && pressok){
		hori.aux1[0] = min;
		hori.min1[0]+=1;
		if(hori.min1[0]>59){
			hori.min1[0]=0;
			hori.hr1+=1;
			aux.manaux[0]+=1;
			aux.manaux[1]+=1;
			aux.manaux[2]+=1;
			if(hori.hr1>99999){
				hori.hr1=0;
			}
		}
	}
	/*---INICIA A CONTAGEM DO TEMPORIZADOR---*/
	if(hori.aux1[1] != min && flagh[0] && !flagh[2] && pressok){
		hori.aux1[1] = min;
		hori.min1[1]++;
		if(hori.min1[1]>59){
			hori.min1[1]=0;
			hori.temp1++;
			if(hori.temp1>999){
				hori.temp1=0;
			}
		}
	}
	/*---GRAVANDO E LENDO HORIMETRO/TIMER---*/
	if(hori.aux2[0]!= min){
		hori.aux2[0] = min;
		hori.aux2[1]+=1;
	}
	else if(hori.aux2[1]==4){
		if(hori.hr1>0)grava_horimetro();
		//grava_minutos();
		//grava_man_barra();
		//grava_man_bomba();
		//grava_limpeza();
		hori.aux2[1]=0;
		if(flagh[0]){
			//grava_minutos_timer();
			grava_timer();
		}
	}
	/*---ZERA TEMPORIZADOR---*/
	if(flagh[1]){
		flagh[0]=1;
		flagh[2]=0;
		hori.temp1 = 0x0;
		hori.min1[1] = 0x0;
		flagh[1]=0;
	}
}