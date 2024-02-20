/*---SENHA REGISTRO DE MANUTENÇOES---*/
#define d1 0
#define d2 0
#define d3 0
#define d4 0
/*-----------------------------------*/
/*---SENHA MENU FABRICANTE---*/
#define e1 0
#define e2 0
#define e3 0
#define e4 0
#define e5 0
#define e6 0
/*-----------------------------------*/
/**
 * TAMANHO DA PAGINA STM32L/STM323G --> 2K
 * PAGINA 0 --> 	0x0800 0000 - 0x0800 07FF
 * PAGINA 63 -->	0x0801 F800 - 0x0801 FFFF
 *
 *
 * STM32L0 EEPROM 6K
 * BANK1 -->		0x0808 0000 - 0x0808 0BFF
 * BANK2 -->		0x0808 0C00 - 0x0808 17FF
 **/

//#define memoba	0x08017000
//#define memoman1	0x08017400
//#define memot		0x08017800
#define memointer	0x08017C00
#define memombr		0x08018000
/*PAGINA DE MEMORIA PARA CONTADOR DAS MANUTENÇOES*/
#define memoman		0x0801E800
/*PAGINMA DE MEMORIA PARA CONTAODR DE MINUTOS*/
#define memom		0x0801F000
/*PAGINA DE MEMORIA PARA O CONTADOR DE HORAS*/
#define memoh		0x0801F800
/*BANK1 DA EEPROM*/
#define eepromini  	0x08080000					// Endereço na Flash (emulação de EEPROM)

//#define DEBUG 0

#define VERSAOFW	1.0
#define ALTURA_INSTALACAO_SENSOR			195					// Sensor instalado a 19,5cm abaixo do fundo do tanque
#define INTERVALO_ENTRE_PONTOS				1000				// Intervalo de 100L entre cada ponto


/**********************************************************************************/
/*                                      VERSAO FW                                 */
/**********************************************************************************/
char vfw1[] = "V1.0";
/**********************************************************************************/
/*          ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO DA SEÇÃO DO MENU          */
/**********************************************************************************/
typedef enum{ //ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO DA SEÇÃO DO MENU
	M1 = 1,
	M1_1,
	M1_2,
	M2,
	M2_1,
	M3,
	M3_1,
	M3_1_1,
	M3_1_2,
	M3_2,
	M3_2_1,
	M4,
	M4_1,
	M4_2,
	M4_3,
	M4_3_1,
	M4_3_2,
	M4_3_3,
	M4_4,
	M4_5,
	M5
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
/*          ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO DE FLAGS                  */
/**********************************************************************************/
typedef enum{
	TANQUEM = eepromini + 4,
	BEEPM = TANQUEM + 4,
	TEMPOSCM = BEEPM + 4,
}flag_memo;
/**********************************************************************************/
/*          ESTRUTURAS DE CONSTANTES PARA IDENTIFICAÇÃO MEMORIA FLAGS             */
/**********************************************************************************/
typedef enum{
	TANQUE = 1,
	BEEP,
	TEMPOSC,
}flag_conf;
/**********************************************************************************/
/*                ESTRUTURAS CONSTANTES PARA LITRAGEM DO TANQUE                   */
/**********************************************************************************/
typedef enum{
	T6 = 1,
	T25
}litros;
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
men flag= {.trava = 0x0, .menu = 0x1, .menu_aux = 0x0, .menu_aux1 = 0x1, .menu_flag = 0x0, .bt = 0x0};
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
/**********************************************************************************/
/*             ESTRUTURAS DE DE VARIAVEIS PARA MANIPULAÇÃO DA SENHA               */
/**********************************************************************************/
typedef struct{
	uint8_t 	flag;
	uint8_t 	dig1;
	uint8_t 	dig2;
	uint8_t 	dig3;
	uint8_t 	dig4;
	uint8_t 	dig5;
	uint8_t 	dig6;
}senha;
senha pass={.flag = 0x0, .dig1=0x0, .dig2=0x0, .dig3=0x0, .dig4=0x0};
senha password={.flag = 0x0, .dig1=0x0, .dig2=0x0, .dig3=0x0, .dig4=0x0, .dig5=0x0, .dig6=0x0};

typedef struct{
	uint8_t 	flagicone[3];
	uint8_t 	flagman[3];
	uint8_t 	flagad[2];
	uint16_t 	manaux[3];
	uint32_t 	horas[2];
	uint8_t 	cont[5];
}manutencao;
manutencao aux = {.flagicone = 0x0, .flagman = 0x0, .manaux = 0x0, .horas = 0x0, .cont = 0x0};

typedef struct{
	uint16_t 	intervalo_man[2];
	uint8_t 	primeira_man[2];
	uint16_t 	limpeza[2];
	uint8_t		temp_oci[2];
}inte;
inte inter= { .intervalo_man=0x1, .primeira_man=0x1, .limpeza=0x1, .temp_oci=0x1};

/*---Flags Do programa----*/
uint8_t
/*----*/flagTx,
/*----*/flagRx,
/*----*/flagh[3],
/*----*/flagmap,
/*----*/flagaviso,
/*----*/flagmap1,
/*----*/flagmap2,
/*----*/flagmap3,
/*----*/flagmap4,
/*----*/flagmap5,
/*----*/flagman,
/*----*/flagman1,
/*----*/flaghistba,
/*----*/flaghistb,
/*----*/flagbeep,
/*----*/flagbp,
/*----*/flagespera,
/*----*/sttbeep,
/*----*/sttnvl,
/*----*/pmin,
/*----*/pmax,
/*----*/nvlpre,
/*----*/flagnvl,
/*----*/tanque,
/*----*/pressok,
/*----*/tela4,
/*----*/flagman1 = 0x00;
/*----Variaveis chaves----*/
uint8_t
/*----*/agua,
/*----*/pressao,
/*----*/iluminacao = 0x00;
/*----Variaveis gerais----*/
uint8_t
/*----*/point,
/*----*/deboucing,
/*----*/tggl,
/*----*/psc,
/*----*/tggl1,
/*----*/tggl2,
/*----*/tggl3,
/*----*/tggl4,
/*----*/sem,
/*----*/dia,
/*----*/diasem,
/*----*/mes,
/*----*/ano,
/*----*/hr,
/*----*/min,
/*----*/cont_amostras,
/*----*/sec = 0x00; //A BLIBLIO aRTC DESLOCA VALORES DE 16BITS, NECESSARIO VARIAVEIS COMPATIVEIS
/*--------------------------*/
uint16_t
/*----*/altura_sensor,
/*----*/altura_sensor_media,
/*----*/battdec,
/*----*/pressdec,
/*----*/stt,
/*----*/auxuart,
/*----*/auxuart1,
/*----*/varia = 0x00;
uint16_t init, flaginit, aux1, aux2 = 0x00;
uint16_t buffer_pontos[31];
uint16_t buffer_media[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/*------Variaveis para contagem-----*/
uint16_t
/*----*/cteclas,
/*----*/cmenu,
/*----*/cmenu1,
/*----*/contdelay,
/*----*/cont[4],
/*----*/contuart,
/*----*/conttg[7],
/*----*/contpress,
/*----*/hidraulico,
/*----*/contcom,
/*----*/cont1=0x0;
/*----------------------------------*/
uint32_t
/*----*/indexh,
/*----*/indext,
/*----*/indexm,
/*----*/indexmt,
/*----*/indexba,
/*----*/auxindexba,
/*----*/indexbr,
/*----*/indexman,
/*----*/indexinter,
/*----*/indexinter1,
/*----*/auxindex,
/*----*/auxindex1,
/*----*/volume_final,
/*----*/indexman1 = 0x00;
uint32_t data_to_write = 0xABCD1259;
float volume_final_float, press[10], pressfinal, batt = 0x00;
char temp[10];
char dat[10];
char pass1[4];
char pass2[6];
char pressao1[6];
char pressao2[11];
char tensao[6];
char nserie[7];
char vfw[7];
char vol[5];
char manstr[] = "M00 00000hr 00/00/00";
char serial_tx_buff[] = "(1EE241,0000,00,00,4.2.00,006,00,00,00,0000)";
//char rx_buff[35];
char rx_buff[40];
