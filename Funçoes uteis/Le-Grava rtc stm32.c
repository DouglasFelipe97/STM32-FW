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

//CRIAMOS UMA INSTANCIA DO HANDLE DO RTC ONDE CONTEM A ESTRUTURA DE DADOS ONDE SAO LIDOS/ESCRITOS OS VALORES DO RELOGIO
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate; 
RTC_DateTypeDef gDate;
RTC_TimeTypeDef gTime;

void set_time(void){
	sTime.Hours = (hr & 0xFF);
	sTime.Minutes = (min & 0xFF);
	sTime.Seconds = (0 & 0xFF);
	if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK);
}
void set_date(void){
	sDate.Date = (dia & 0xFF);
	sDate.Month = (mes & 0xFF);
	sDate.Year = (ano & 0xFF);
	sDate.WeekDay = (diasem & 0xFF);
	if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK);
}
void funcoes_relogio(void){
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
}