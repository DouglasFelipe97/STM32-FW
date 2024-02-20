/**VARIAVEIS GLOBAIS**/
uint16_t buffer_pontos[31];

void indicador(uint16_t altura_sensor){
	uint8_t aux = 0x00;
	uint16_t nivel = 0x00;
	char buff[15];

	struct VolumeRange { 		//STRUCTS QUE IRA GUARDAR AS INFORMAÇOES DE CADA RANGE DA LITRAGEM
		float min;
		float max;
		const char* str;
	};
	/*==============================================================================================================================*/
	/*==================INFOS TANQUE 600L==================*/
	/*====PONTOS====*/
	if(tanque == T6){
		buffer_pontos[10] = 483;
		buffer_pontos[ 9] = 448;
		buffer_pontos[ 8] = 410;
		buffer_pontos[ 7] = 374;
		buffer_pontos[ 6] = 335;
		buffer_pontos[ 5] = 299;
		buffer_pontos[ 4] = 259;
		buffer_pontos[ 3] = 211;
		buffer_pontos[ 2] = 161;
		buffer_pontos[ 1] = 100;
		buffer_pontos[ 0] = 0;
	}
	/*====RANGES====*/
	const struct VolumeRange rangeT6[] = { //UTILIZAMOS UM VETOR DE STRUCTS QUE CONTEM AS INFORMAÇOES DOS RANGES MIN E MAX E O VALOR EM (L) EM CADA POSIÇÃO DO VETOR
			{0, 24, "BAIXO"},
			{25, 49, "25L"},
			{50, 74, "50L"},
			{75, 99, "75L"},
			{100, 124, "100L"},
			{125, 149, "125L"},
			{150, 174, "150L"},
			{175, 199, "175L"},
			{200, 224, "200L"},
			{225, 249, "225L"},
			{250, 274, "250L"},
			{275, 299, "275L"},
			{300, 324, "300L"},
			{325, 349, "325L"},
			{350, 374, "350L"},
			{375, 399, "375L"},
			{400, 424, "400L"},
			{425, 449, "425L"},
			{450, 474, "450L"},
			{475, 499, "475L"},
			{500, 524, "500L"},
			{525, 549, "525L"},
			{550, 574, "550L"},
			{575, 599, "575L"},
			{600, 624, "600L"},
			{625, 649, "625L"},
			{650, 674, "650L"},
			{675, 699, "675L"},
			{700, 724, "700L"},
			{725, 749, "725L"},
			{750, 1000, "CHEIO"},
	};

	if (tanque == T25){
		/*==================INFOS TANQUE 2500L==================*/
		/*====PONTOS====*/
		buffer_pontos[30] = 1300;				// Altura estimada para compor tabela até 3000L
		buffer_pontos[29] = 1210;				// Altura estimada para compor tabela até 3000L
		buffer_pontos[28] = 1160;				// Altura estimada para compor tabela até 3000L
		buffer_pontos[27] = 1110;				// Altura estimada para compor tabela até 3000L
		buffer_pontos[26] = 1060;				// Altura estimada para compor tabela até 3000L

		buffer_pontos[25] = 1011;
		buffer_pontos[24] = 972;
		buffer_pontos[23] = 933;
		buffer_pontos[22] = 897;
		buffer_pontos[21] = 861;
		buffer_pontos[20] = 827;
		buffer_pontos[19] = 793;
		buffer_pontos[18] = 763;
		buffer_pontos[17] = 728;
		buffer_pontos[16] = 695;
		buffer_pontos[15] = 661;
		buffer_pontos[14] = 627;
		buffer_pontos[13] = 592;
		buffer_pontos[12] = 558;
		buffer_pontos[11] = 520;
		buffer_pontos[10] = 483;
		buffer_pontos[ 9] = 448;
		buffer_pontos[ 8] = 410;
		buffer_pontos[ 7] = 374;
		buffer_pontos[ 6] = 335;
		buffer_pontos[ 5] = 299;
		buffer_pontos[ 4] = 259;
		buffer_pontos[ 3] = 211;
		buffer_pontos[ 2] = 161;
		buffer_pontos[ 1] = 100;
		buffer_pontos[ 0] = 0;
	}
	/*====RANGES====*/
	const struct VolumeRange rangeT25[] = { //UTILIZAMOS UM VETOR DE STRUCTS COM OS RANGES MIN E MAX E O VALOR EM (L) DESSE INTERVALOS
			{0, 24, "BAIXO"},
			{25, 49, "25L"},
			{50, 74, "50L"},
			{75, 99, "75L"},
			{100, 124, "100L"},
			{125, 149, "125L"},
			{150, 174, "150L"},
			{175, 199, "175L"},
			{200, 224, "200L"},
			{225, 249, "225L"},
			{250, 274, "250L"},
			{275, 299, "275L"},
			{300, 324, "300L"},
			{325, 349, "325L"},
			{350, 374, "350L"},
			{375, 399, "375L"},
			{400, 424, "400L"},
			{425, 449, "425L"},
			{450, 474, "450L"},
			{475, 499, "475L"},
			{500, 524, "500L"},
			{525, 549, "525L"},
			{550, 574, "550L"},
			{575, 599, "575L"},
			{600, 624, "600L"},
			{625, 649, "625L"},
			{650, 674, "650L"},
			{675, 699, "675L"},
			{700, 724, "700L"},
			{725, 749, "725L"},
			{750, 774, "750L"},
			{775, 799, "775L"},
			{800, 824, "800L"},
			{825, 849, "825L"},
			{850, 874, "850L"},
			{875, 899, "875L"},
			{900, 924, "900L"},
			{925, 949, "925L"},
			{950, 974, "950L"},
			{975, 999, "975L"},
			{1000, 1024, "1000L"},
			{1025, 1049, "1025L"},
			{1050, 1074, "1050L"},
			{1075, 1099, "1075L"},
			{1100, 1124, "1100L"},
			{1125, 1149, "1125L"},
			{1150, 1174, "1150L"},
			{1175, 1199, "1175L"},
			{1200, 1224, "1200L"},
			{1225, 1249, "1225L"},
			{1250, 1274, "1250L"},
			{1275, 1299, "1275L"},
			{1300, 1324, "1300L"},
			{1325, 1349, "1325L"},
			{1350, 1374, "1350L"},
			{1375, 1399, "1375L"},
			{1400, 1424, "1400L"},
			{1425, 1449, "1425L"},
			{1450, 1474, "1450L"},
			{1475, 1499, "1475L"},
			{1500, 1524, "1500L"},
			{1525, 1549, "1525L"},
			{1550, 1574, "1550L"},
			{1575, 1599, "1575L"},
			{1600, 1624, "1600L"},
			{1625, 1649, "1625L"},
			{1650, 1674, "1650L"},
			{1675, 1699, "1675L"},
			{1700, 1724, "1700L"},
			{1725, 1749, "1725L"},
			{1750, 1774, "1750L"},
			{1775, 1799, "1775L"},
			{1800, 1824, "1800L"},
			{1825, 1849, "1825L"},
			{1850, 1874, "1850L"},
			{1875, 1899, "1875L"},
			{1900, 1924, "1900L"},
			{1925, 1949, "1925L"},
			{1950, 1974, "1950L"},
			{1975, 1999, "1975L"},
			{2000, 2024, "2000L"},
			{2025, 2049, "2025L"},
			{2050, 2074, "2050L"},
			{2075, 2099, "2075L"},
			{2100, 2124, "2100L"},
			{2125, 2149, "2125L"},
			{2150, 2174, "2150L"},
			{2175, 2199, "2175L"},
			{2200, 2224, "2200L"},
			{2225, 2249, "2225L"},
			{2250, 2274, "2250L"},
			{2275, 2299, "2275L"},
			{2300, 2324, "2300L"},
			{2325, 2349, "2325L"},
			{2350, 2374, "2350L"},
			{2375, 2399, "2375L"},
			{2400, 2424, "2400L"},
			{2425, 2449, "2425L"},
			{2450, 2474, "2450L"},
			{2475, 2499, "2475L"},
			{2500, 2524, "2500L"},
			{2525, 2549, "2525L"},
			{2550, 2574, "2550L"},
			{2575, 2599, "2575L"},
			{2600, 2624, "2600L"},
			{2625, 2649, "2625L"},
			{2650, 2674, "2650L"},
			{2675, 2699, "2675L"},
			{2700, 2724, "2700L"},
			{2725, 2749, "2725L"},
			{2750, 2774, "2750L"},
			{2775, 2799, "2775L"},
			{2800, 3000, "CHEIO"},
	};

	/*==============================================================================================================================*/

	/*---MODIFICAR LIMITES DE AUX APÓS RESULTADO DO A/D---*/
	altura_sensor_media = calcula_media_nivel(altura_sensor);

	//altura_sensor_media -= ALTURA_INSTALACAO_SENSOR;
	/*====CALCULO MEDIA MOVEL===*/
	if(altura_sensor_media > 0)
	{
		for(uint8_t cont_buffer = 0; cont_buffer <= 29; cont_buffer++)
		{
			if(altura_sensor_media >= buffer_pontos[cont_buffer] && altura_sensor_media < buffer_pontos[cont_buffer + 1])
			{
				volume_final = (altura_sensor_media - buffer_pontos[cont_buffer]) * INTERVALO_ENTRE_PONTOS;
				volume_final = volume_final / (buffer_pontos[cont_buffer + 1] - buffer_pontos[cont_buffer]);
				volume_final = volume_final + (cont_buffer * INTERVALO_ENTRE_PONTOS);
				volume_final_float = (float)volume_final / 10;
			}
		}
	}
	else
	{
		volume_final = 0;
		volume_final_float = 0.0;
	}
	/*INDICADOR DE BARRAS NA LATERAL DA TELA*/
	switch(tanque){
	case T6:
		if(volume_final_float <=50)aux = 0;
		else if(volume_final_float<=119)aux=1;
		else if(volume_final_float>=120 && volume_final_float<=239)aux=2;
		else if(volume_final_float>=240 && volume_final_float<=359)aux=3;
		else if(volume_final_float>=360 && volume_final_float<=479)aux=4;
		else aux=5;
		break;
	case T25:
		if(volume_final_float <=100)aux = 0;
		else if(volume_final_float<=1050)aux=1;
		else if(volume_final_float>=1050 && volume_final_float<=1600)aux=2;
		else if(volume_final_float>=1600 && volume_final_float<=2150)aux=3;
		else if(volume_final_float>=2150 && volume_final_float<=2700)aux=4;
		else aux=5;
		break;
	}
	/*----------------------------------------*/
	switch(tanque){
	case T6:
		for (size_t i = 0; i < sizeof(rangeT6) / sizeof(rangeT6[0]); ++i) {	//ITERAMOS I E PERCORREMOS TODO O VETOR.
			if (volume_final_float >= rangeT6[i].min && volume_final_float <= rangeT6[i].max) { //AO ACHARMOS UM VALOR QUE SE ENCAIXA NO RANGE IMPRIMMOS NA TELA
				if(tela4)
					OLED_DrawStr(rangeT6[i].str, CENTER, 33, 1);
				else
					OLED_DrawStr(rangeT6[i].str, 2, 2, 1);
				break;
			}
		}
		break;
	case T25:
		for (size_t i = 0; i < sizeof(rangeT25) / sizeof(rangeT25[0]); ++i) {	//ITERAMOS I E PERCORREMOS TODO O VETOR.
			if (volume_final_float >= rangeT25[i].min && volume_final_float <= rangeT25[i].max) { //AO ACHARMOS UM VALOR QUE SE ENCAIXA NO RANGE IMPRIMMOS NA TELA
				if(tela4)
					OLED_DrawStr(rangeT25[i].str, CENTER, 33, 1);
				else
					OLED_DrawStr(rangeT25[i].str, 2, 2, 1);
				break;
			}
		}
		break;
	}


	switch (aux){
	case 0:
		if(conttg[3]>600){
			tggl = !tggl;
			conttg[3]=0;
		}
		if(tggl==1){
			if(tela4){
				OLED_DrawRectangleFill(2, 57, 21, 63, 1);
				OLED_DrawRectangleFill(106, 57, 125, 63, 1);
			}
			else
				OLED_DrawRectangleFill(2, 57, 15, 63, 1);

		}
		else if(tggl==0){
			if(tela4){
				OLED_DrawRectangleFill(2, 57, 21, 63, 0);
				OLED_DrawRectangleFill(106, 57, 125, 63, 0);
			}
			else
				OLED_DrawRectangleFill(2, 57, 15, 63, 0);

		}
		break;
	case 1:
		if(tela4){
			OLED_DrawRectangleFill(2, 57, 21, 63, 1);
			OLED_DrawRectangleFill(106, 57, 125, 63, 1);
		}
		else
			OLED_DrawRectangleFill(2, 57, 15, 63, 1);
		break;
	case 2:
		if(tela4){
			OLED_DrawRectangleFill(2, 47, 21, 53, 1);
			OLED_DrawRectangleFill(2, 57, 21, 63, 1);
			OLED_DrawRectangleFill(106, 47, 125, 53, 1);
			OLED_DrawRectangleFill(106, 57, 125, 63, 1);
		}

		else{
			OLED_DrawRectangleFill(2, 47, 15, 53, 1);
			OLED_DrawRectangleFill(2, 57, 15, 63, 1);
		}
		break;
	case 3:
		if(tela4){
			OLED_DrawRectangleFill(2, 37, 21, 43, 1);
			OLED_DrawRectangleFill(2, 47, 21, 53, 1);
			OLED_DrawRectangleFill(2, 57, 21, 63, 1);
			OLED_DrawRectangleFill(106, 37, 125, 43, 1);
			OLED_DrawRectangleFill(106, 47, 125, 53, 1);
			OLED_DrawRectangleFill(106, 57, 125, 63, 1);
		}

		else{
			OLED_DrawRectangleFill(2, 37, 15, 43, 1);
			OLED_DrawRectangleFill(2, 47, 15, 53, 1);
			OLED_DrawRectangleFill(2, 57, 15, 63, 1);
		}
		break;
	case 4:
		if(tela4){
			OLED_DrawRectangleFill(2, 27, 21, 33, 1);
			OLED_DrawRectangleFill(2, 37, 21, 43, 1);
			OLED_DrawRectangleFill(2, 47, 21, 53, 1);
			OLED_DrawRectangleFill(2, 57, 21, 63, 1);
			OLED_DrawRectangleFill(106, 27, 125, 33, 1);
			OLED_DrawRectangleFill(106, 37, 125, 43, 1);
			OLED_DrawRectangleFill(106, 47, 125, 53, 1);
			OLED_DrawRectangleFill(106, 57, 125, 63, 1);
		}
		else{
			OLED_DrawRectangleFill(2, 27, 15, 33, 1);
			OLED_DrawRectangleFill(2, 37, 15, 43, 1);
			OLED_DrawRectangleFill(2, 47, 15, 53, 1);
			OLED_DrawRectangleFill(2, 57, 15, 63, 1);
		}
		break;
	case 5:
		if(tela4){
			OLED_DrawRectangleFill(2, 17, 21, 23, 1);
			OLED_DrawRectangleFill(2, 27, 21, 33, 1);
			OLED_DrawRectangleFill(2, 37, 21, 43, 1);
			OLED_DrawRectangleFill(2, 47, 21, 53, 1);
			OLED_DrawRectangleFill(2, 57, 21, 63, 1);
			OLED_DrawRectangleFill(106, 17, 125, 23, 1);
			OLED_DrawRectangleFill(106, 27, 125, 33, 1);
			OLED_DrawRectangleFill(106, 37, 125, 43, 1);
			OLED_DrawRectangleFill(106, 47, 125, 53, 1);
			OLED_DrawRectangleFill(106, 57, 125, 63, 1);
		}

		else{
			OLED_DrawRectangleFill(2, 17, 15, 23, 1);
			OLED_DrawRectangleFill(2, 27, 15, 33, 1);
			OLED_DrawRectangleFill(2, 37, 15, 43, 1);
			OLED_DrawRectangleFill(2, 47, 15, 53, 1);
			OLED_DrawRectangleFill(2, 57, 15, 63, 1);
		}
		break;
	}
}