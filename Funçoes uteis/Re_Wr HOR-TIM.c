void grava_horimetro(void){					//---PAGINA INTEIRA 1K---//
	if(indexh == 0x0){
		erase_flash(memoh);//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
	}
	if(indexh < 0x3FF){
		write_flash(memoh+indexh, hori.hr1);
		indexh +=4;
	}
	else
		indexh = 0x0;
}
void leitura_horimetro(void){
	uint32_t horit=0x00;
	if(!flagmap){
		for(horit=0x01; horit!=0x00000000; indexh +=4){//ITERAMOS INDEX NO FOR ATÉ ENCONTRAR UM ENDEREÇO VAZIO
			horit = read_flash(memoh+indexh);			//LEMOS PARA VERIFICAR O ENDEREÇO
		}
		if(indexh>0x3FF)indexh = 0;
		if(indexh==4)indexh-=4;
		else indexh-=8; 						//VOLTAMOS 8 POSIÇOES PARA 32BITS POR CONTA DO LAÇO FOR
		flagmap = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
	}
	hori.hr1 = read_flash(memoh+indexh);		//LEMOS E GUARDAMOS O ULTIMO RESULTADO ENCONTRADO NO ULTIMO ENDEREÇO GRAVADO
}
void grava_timer(void){					//---MEIA PAGINA---//
	if(indext<0x7FF){
		write_flash(memoh+indext, hori.temp1);
		indexh +=4;
	}
	else
		indext = 0x400;
}
void leitura_timer(void){			//---MEIA PAGINA---//
	if(!flagmap2){
		for(uint32_t horit=0x01; horit!=0x00000000; indext +=4){//ITERAMOS INDEX NO FOR ATÉ ENCONTRAR UM ENDEREÇO VAZIO
			horit = read_flash(memoh+indext);					//LEMOS PARA VERIFICAR O ENDEREÇO
		}
		if(indext>=0x7FF)indext = 0;
		if(indext==4)indext-=4;
		else indext-=8; 						//VOLTAMOS 8 POSIÇOES PARA 32BITS POR CONTA DO LAÇO FOR
		flagmap2 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
	}
	hori.temp1 = read_flash(memoh+indext);		//LEMOS E GUARDAMOS O ULTIMO RESULTADO ENCONTRADO NO ULTIMO ENDEREÇO GRAVADO
}
void grava_minutos(void){
	if(indexm == 0x0){
		erase_flash(memom); 					//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
		//indexmt = 0x200; 						//INICIAMOS A OUTRA MEDADE DA MEMORIA
	}
	if(indexm<0x3FF){
		write_flash(memom+indexm, hori.min1[0]);
		indexm +=4;
	}
	else
		indexm = 0x0;
}
void leitura_minutos(void){
	if(!flagmap1){
		for(uint16_t horit=0x01; horit!=0x00000000; indexm +=4){//ITERAMOS INDEX NO FOR ATÉ ENCONTRAR UM ENDEREÇO VAZIO
			horit = read_flash(memom+indexm);					//LEMOS PARA VERIFICAR O ENDEREÇO
		}
		if(indexm>0x3FF)indexm = 0;
		if(indexm==4)indexm-=4;
		else indexm-=8; 						//VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
		flagmap1 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
	}
	hori.min1[0] = (read_flash(memom+indexm) & 0xFFFF);		//LEMOS E GUARDAMOS O ULTIMO RESULTADO ENCONTRADO NO ULTIMO ENDEREÇO GRAVADO
}
void grava_minutos_timer(void){ 				//ESSA FUNÇÃO DE GRAVAÇÃO NAO APAGA A MEMORIA, APENAS A FUNÇÃO DE GRAVAÇAO PRINCIPAL DE MINUTOS
	if(indexm<0x7FF){
		write_flash(memom+indexmt, hori.min1[1]);
		indexmt +=4;
	}
	else
		indexmt = 0x400;
}
void leitura_minutos_timer(void){
	if(!flagmap3){
		for(uint16_t horit=0x01; horit!=0x00000000; indexmt +=4){//ITERAMOS INDEX NO FOR ATÉ ENCONTRAR UM ENDEREÇO VAZIO
			horit = read_flash(memom+indexmt);						//LEMOS PARA VERIFICAR O ENDEREÇO
		}
		if(indexmt==4)indexmt-=4;
		else indexmt-=8; 						//VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
		flagmap3 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
	}
	hori.min1[1] = (read_flash(memom+indexmt) & 0xFFFF);		//LEMOS E GUARDAMOS O ULTIMO RESULTADO ENCONTRADO NO ULTIMO ENDEREÇO GRAVADO
}
void grava_man_barra(void){ //*MEIA PAGINA*/
	if(indexman == 0x0){
		FLASH_apaga(memoman, 1); //A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
		erase_flash(memoman); 
	}
	if(indexman<0x3FF){
		write_flash(memoman+indexman, aux.manaux[0]);
		indexman +=4;
	}
	else
		indexman = 0x0;
}
		
void leitura_man_barra(void){
	if(!flagman){
		for(uint16_t horit=0x01; horit!=0x00000000; indexman +=4){
			horit = read_flash(memoman+indexman);
		}
		if(indexman>0x3FF)indexman = 0;
		if(indexman==4)indexman-=4;
		else indexman-=8; //VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
		flagman = 1;//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
	}
	aux.manaux[0] = read_flash(memoman+indexman);
}

void grava_man_bomba(void){			//---MEIA PAGINA---//
	if(indexman1<0x3FF){
		write_flash(memoman+indexman1, aux.manaux[1]);
		indexman1 +=4;
	}
	else
		indexman1 = 0x400;
}
void leitura_man_bomba(void){			//---MEIA PAGINA---//
	if(indexman1<=0x7FF){
		if(!flagman1){
			for(uint16_t horit=0x01; horit!=0x00000000; indexman1 +=4){
				horit = read_flash(memoman+indexman1);
			}
			if(indexman1>0x7FF)indexman1 = 0;
			if(indexman1==4)indexman1-=4;
			else indexman1-=4; //VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
			flagman1 = 1;//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
		}
		aux.manaux[1] = read_flash(memoman+indexman1);
	}
}