/**********************************************************************************/
/*          ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO DE FLAGS                  */
/**********************************************************************************/
typedef enum{
	TANQUE = 1,
	BEEP,
	TEMP0SC,
}flag_conf;

void leitura_flags(uint8_t flag){
	switch (flag){
	case TANQUE:
		tanque = (read_from_eeprom(TANQUEM) & 0XFF);
		break;
	case BEEP:
		sttbeep = (read_from_eeprom(BEEPM) & 0XFF);
		flagbeep = sttbeep;
		break;
	case TEMPOSC:
		inter.temp_oci[0] = (read_from_eeprom(BEEPM) & 0XFF);
		inter.temp_oci[1] = inter.temp_oci[0];
		break;
	}
}