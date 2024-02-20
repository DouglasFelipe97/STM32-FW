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