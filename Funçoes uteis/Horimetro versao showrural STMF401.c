/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/*INFOS GERAIS
 * CONFIG I2C P/ DISPLAY OLED:
 * 	- Núcleo do Sistema -> RCC -> Relógio de Alta Velocidade = Cristal
 *	- Núcleo do Sistema -> Sys -> Depurar = Fio Serial
 *	- Conectividade -> I2C1 -> I2C = I2C
 *	- Em Configurações de parâmetros -> Modo de velocidade I2C = Modo rápido (caso contrário, a exibição é atualizada muito lentamente durante as animações)
 *	- A frequência está definida para o máximo - 72 MHz
 *	TIMER 3 P/ VARREDURA DE TECLADO:
 *	- PRESCLE: 287
 *	- ARR: 249
 *	- COUNTER MODE: UP
 *	- TEMPO DE OVERFLOW:1MS
 *	- NVIC: INTERRUPÇOES TIMER 3 HABILITADAS
 * */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
#include <OLED_Fonts.h>
#include <OLED_Icons.h>
#include <OLED.h>
#include "flash_v1.0.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define d1 1
#define d2 2
#define d3 3
#define d4 5

#define e1 0
#define e2 0
#define e3 0
#define e4 0
#define e5 0
#define e6 0

//#define memoba	0x08017000
//#define memoman1	0x08017400
//#define memot		0x08017800
#define memointer	0x08017C00
#define memoman		0x08018000
#define memobr		0x08018400
#define memoh		0x08018800
#define memom		0x08018C00

#define DEBUG 0

#define VERSAOFW	1.0
#define ALTURA_INSTALACAO_SENSOR			195					// Sensor instalado a 19,5cm abaixo do fundo do tanque
#define INTERVALO_ENTRE_PONTOS				1000				// Intervalo de 100L entre cada ponto

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
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
char rx_buff[35];

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate; //CRIAMOS UMA INSTANCIA DO HANDLE DO RTC ONDE CONTEM A ESTRUTURA DE DADOS ONDE SAO LIDOS/ESCRITOS OS VALORES DO RELOGIO
RTC_DateTypeDef gDate;
RTC_TimeTypeDef gTime;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void initdisp(void);
void trata_teclas(void);
void atualiza_menu(void);
void init_flags(void);
uint16_t calcula_media_nivel(uint16_t altura_sensor);
void indicador(uint16_t nivel);
void grava_horimetro(void);
void leitura_horimetro(void);
void grava_minutos(void);
void leitura_minutos(void);
void grava_timer(void);
void leitura_timer(void);
void grava_minutos_timer(void);
void leitura_minutos_timer(void);
void grava_man_barra(void);
void leitura_man_barra(void);
void grava_man_bomba(void);
void leitura_man_bomba(void);
void verifica_manutencao(void);
void historico_bagua(void);
void historico_barra(void);
void aviso_manutencao(void);
void comuart(void);
void leitura_chaves_hidraulico(void);
void beep(void);
void set_time(void);
void set_date(void);
void funcoes_horimetro(void);
void leitura_inter(void);
void grava_limpeza(void);
void le_limpeza(void);
void grava_flagt(void);
void menuprincipal(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4);
void menutimer(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4);
void registraman(uint8_t n1, uint8_t n2, uint8_t n3);
void preferencias(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4);
void conffabricante(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4);
void intervaloman(uint8_t n1, uint8_t n2, uint8_t n3);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**********************************************************************************/
/*                     FUNÇÃO CALL BACK QUE TRATA TIMERS                          */
/**********************************************************************************/
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef * htim){//ENDEREÇO DE HTIM COMO PARAMETRO
	if(htim->Instance == TIM3){ //IR�? LER O MEMBRO DA ESTRUTURA APONTADO PELO PONTEIRO PASSADO COMO ARGUMENTO (*htim/nome da estrutura) DA FUNC CALL BACK
		/*FUNÇÃO DE TRATAMENTO DE INTERRUPÇÃO POR OVERFLOW TIMER3*/ /*Instance É A FLAG QUE INDICA QUAL TIMER GEROU A INTERRUPÇÃO*/
		/*DELAYS POR INTERRUPÇÃO - CONTADORES*/
		cteclas++;												/*instance = Register base address */
		cmenu++;
		cmenu1++;
		cont[0]++;
		cont[1]++;
		contuart++;
		conttg[0]++;
		conttg[1]++;
		conttg[2]++;
		conttg[3]++;
		conttg[4]++;
		conttg[5]++;
		conttg[6]++;
		/*-----------------------------------*/
		if(cont[1]>=(inter.temp_oci[0]*1000)){
			if(inter.temp_oci[0]>=10){
				flag.trava = 0x00;
				flag.menu = 0x00;
				flag.menu_flag[0] = 0x00;
				flag.menu_flag[1] = 0x00;
				flag.menu_flag[2] = 0x00;
				flag.menu_flag[3] = 0x00;
				flag.menu_flag[4] = 0x00;
				flag.menu_aux[0] = 0x00;
				flag.menu_aux[1] = 0x00;
				flag.menu_aux[2] = 0x00;
				flag.menu_aux[3] = 0x00;
				flag.menu_aux[4] = 0x00;
				flag.menu_aux1 = 0x1;
				cont[1]=0x00;
				pass.flag = 0x00;
				password.flag = 0x00;
			}
		}
		if(init && cont[0]>=300){
			funcoes_horimetro();
			cont[0]=0x0;
		}
		if(init && contuart>=200){
			comuart();
			HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);
		}
		if(init && cteclas>=100){
			trata_teclas();
			cteclas = 0;
		}
		if (cmenu1 >= 100){
			HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN); 							//LE OS VALORES DO RTC E ARMAZENA NO HANDLE
			cmenu1 = 0;
		}
	}
}
void trata_teclas(void){
	beep();
	/**********************************************************************************/
	/*                           TRATA EVENTO TECLA CONF                              */
	/**********************************************************************************/
	if(!HAL_GPIO_ReadPin(TCF_GPIO_Port, TCF_Pin)){
		deboucing = 50;							//CONTADOR DEBOUCING
		cont[1]=0x00;
		while(deboucing>0){
			deboucing--;						//DECREMENTO DO DEBOUCING
		}
		if(!flagespera)
			flag.bt = OK;							//SETA FLAG DE BOTÃO ACIONADO
	}
	while(!HAL_GPIO_ReadPin(TCF_GPIO_Port, TCF_Pin)){	//SEGURA O PROGRAMA ENQUANTO O BOTÃO
		//asm("nop");								//CONTINUAR PRESSIONADO
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
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==0){
		flag.bt= 0;
		flag.trava = 1;
		flag.menu_aux[1] = 1;
		flag.menu_flag[0] = M1;
		flag.menu_flag[1] = 0;
	}
	/*---ENTRA NO SUBMENU 1-1 PRIMEIRA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==0 && flag.menu_aux[1]==1){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M1_1;
	}
	/*---CONF SUBMENU 1-1---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_1){
		flag.bt = 0;
		flag.menu_aux[2]++;
		if(flag.menu_aux[2]>4){
			flag.menu_aux[1]=1;
			flag.menu_flag[1] = 0;
		}
		set_date();
	}
	/*---ENTRA NO SUBMENU 1-2 SEGUNDA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==0 && flag.menu_aux[1]==2){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M1_2;
	}
	/*---CONF SUBMENU 1-2---*/
	else if(flag.bt==OK && flag.menu==1 && flag.menu_flag[0]==M1 && flag.menu_flag[1]==M1_2){
		flag.bt = 0;
		flag.menu_aux[2]++;
		if(flag.menu_aux[2]>2){
			flag.menu_aux[1]=2;
			flag.menu_flag[1] = 0;
		}
		set_time();
	}
	/*--ENTRA NA SEGUNDA OPÇÃO DO MENU 1---*/
	else if(flag.bt==OK && flag.menu==2 && flag.menu_flag[0]==0){
		flag.bt= 0;
		flag.trava = 1;
		flag.menu_aux[1] = 1;
		flag.menu_flag[0] = M2;
		flag.menu_flag[1] = 0;
	}
	/*---SELECIONA INICIA TEMP---*/
	else if(flag.bt==OK && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_aux[1]==1){
		flag.bt= 0;
		flagh[0]=1;
		flagh[2]=0;
		flagaviso=1;
	}
	/*---SELECIONA PAUSA TEMP---*/
	else if(flag.bt==OK && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_aux[1]==2){
		flag.bt= 0;
		flagaviso=1;
		if(flagh[0])flagh[2]=1;
	}
	/*---SELECIONA REINICIA TEMP---*/
	else if(flag.bt==OK && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_aux[1]==3){
		flag.bt= 0;
		flagh[1]=1;
		flagaviso=1;
	}
	/*---SELECIONA DESABILITA TEMP---*/
	else if(flag.bt==OK && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_aux[1]==4){
		flag.bt= 0;
		flagh[0]=0;
		flagh[2]=0;
		flagaviso=1;
	}
	/*---ENTRA NO SUBMENU 2-3 ---*/
	else if(flag.bt==OK && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_flag[1]==0 && flag.menu_aux[1]==5){
		flag.bt = 0;
		//flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M2_1;
	}
	else if(flag.bt==OK && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_flag[1]==M2_1 && flag.menu_aux[1]==5){
		flag.bt = 0;
		flag.menu_flag[1] = 0;
	}
	/*--ENTRA NA TERCEIRA OPÇÃO DO MENU 1---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==0){
		flag.bt= 0;
		flagman1=1;
		flag.trava = 1;
		flag.menu_aux[1] = 1;
		flag.menu_flag[0] = M3;
		flag.menu_flag[1] = 0;
	}
	/*---ENTRA NO SUBMENU 3-1 PRIMEIRA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==0 && flag.menu_aux[1]==1){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M3_1;
	}
	/*---ENTRA NO SUBMENU 3-1-1---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_aux[2]==1 && !flag.menu_flag[2]){
		flag.bt = 0;
		flag.menu_flag[2] = M3_1_1;
		point=0;
		flaghistba = 0x00;
	}
	/*---ENTRA NO SUBMENU 3-1-2---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_aux[2]==2 && !flag.menu_flag[2]){
		flag.bt = 0;
		flag.menu_flag[2] = M3_1_2;
		point=0;
		flaghistb = 0x00;
	}
	/*---CONFIRMA HISTORICO SUBMENU 3-1-1---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_1){
		flag.bt = 0;
		flag.menu_aux[0]++;
		if(flag.menu_aux[0]>=1){
			flag.menu_aux[0]=0;
			flag.menu_flag[2] = 0;
			flaghistba = 0;
			point=0;
			auxindex = 0;
		}
	}
	/*---CONFIRMA HISTORICO SUBMENU 3-1-2---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_2){
		flag.bt = 0;
		flag.menu_aux[0]++;
		if(flag.menu_aux[0]>=1){
			flag.menu_aux[0]=0;
			flag.menu_flag[2] = 0;
			flaghistb = 0;
			point=0;
			auxindex1 = 0;
		}
	}
	/*---ENTRA NO SUBMENU 3-2 SEGUNDA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==0 && flag.menu_aux[1]==2){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M3_2;
	}
	/*---CONF SUBMENU 3-2---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && !pass.flag){
		flag.bt = 0;
		flag.menu_aux[2]++;
	}
	/*---CONFIRMA MANUTENÇÃO SUBMENU 3-2-1---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag && flag.menu_aux[2]==1){
		flag.bt = 0;
		flag.menu_aux[3]++;
	}
	/*---CONFIRMA MANUTENÇÃO SUBMENU 3-2-2---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag && flag.menu_aux[2]==2){
		flag.bt = 0;
		flag.menu_aux[3]++;
	}
	/*---CONFIRMA MANUTENÇÃO SUBMENU 3-2-3---*/
	else if(flag.bt==OK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag && flag.menu_aux[2]==3){
		flag.bt = 0;
		flag.menu_aux[3]++;
	}
	/*--ENTRA NA QUARTA OPÇÃO DO MENU 1---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==0){
		flag.bt= 0;
		flag.trava = 1;
		flag.menu_aux[1] = 1;
		flag.menu_flag[0] = M4;
		flag.menu_flag[1] = 0;
	}
	/*---ENTRA NO SUBMENU 4-1 PRIMEIRA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==0 && flag.menu_aux[1]==1){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M4_1;
	}
	/*--NAVEGA NA PRIMEIRA OPÇÃO DO MENU 4---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_1 && flag.menu_aux[1]==1){
		flag.bt= 0;
		flagaviso=1;
		flag.menu_aux[2] = 0;
		//flag.menu_flag[1]=0;
		flag.menu_aux[1]=1;
	}
	/*---ENTRA NO SUBMENU 4-2 SEGUNDA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==0 && flag.menu_aux[1]==2 && !flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_flag[1] = M4_2;
		flag.menu_aux[4] = 1;
	}
	/*--NAVEGA NA SEGUNDA OPÇÃO DO MENU 4---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_2 && flag.menu_aux[1]==2 && flag.menu_aux[4]){
		flag.bt= 0;
		flag.menu_aux[4]++;
		if(flag.menu_aux[4]>2)
			flagaviso=1;
	}
	/*---ENTRA NO SUBMENU 4-3 PRIMEIRA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==0 && flag.menu_aux[1]==3){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M4_3;
	}
	/*---CONF SUBMENU 4-3---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && !password.flag){
		flag.bt = 0;
		flag.menu_aux[2]++;
	}
	/*--NAVEGA NA TERCEIRA OPÇÃO DO MENU 4 SEÇÃO 1---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==1 && password.flag && !flag.menu_aux[4]){
		flag.bt= 0;
		flag.menu_aux[4]=1;
	}
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==1 && password.flag && flag.menu_aux[4]){
		flag.bt= 0;
		flag.menu_aux[4]++;
	}
	/*--ENTRA NA TERCEIRA OPÇÃO DO MENU 4 SEÇÃO 2---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==2 && password.flag && !flag.menu_flag[2]){
		flag.bt= 0;
		flag.menu_flag[2] = M4_3_1;
		flag.menu_aux[3] = 1;
	}
	/*--ENTRA NA TERCEIRA OPÇÃO DO MENU 4 SEÇÃO 3-2 (INTERVALO MAN)---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==2 && password.flag && flag.menu_flag[2]==M4_3_1 && !flag.menu_aux[3]){
		flag.bt= 0;
		flag.menu_aux[3]=1;
	}
	/*--NAVEGA NA TERCEIRA OPÇÃO DO MENU 4 SEÇÃO 3-1-1 (INTERVALO MAN BOMBA)---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==2 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==1){
		flag.bt= 0;
		flag.menu_aux[4]++;
	}
	/*--NAVEGA NA TERCEIRA OPÇÃO DO MENU 4 SEÇÃO 3-1-2 (INTERVALO MAN BARRA)---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==2 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==2){
		flag.bt= 0;
		flag.menu_aux[4]++;
	}
	/*--NAVEGA NA TERCEIRA OPÇÃO DO MENU 4 SEÇÃO 3-1-3 (INTERVALO MAN LIMPEZA)---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==2 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==3){
		flag.bt= 0;
		flag.menu_aux[4]++;
	}
	/*MENU OCIOSO*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==3 && password.flag && !flag.menu_flag[2]){
		flag.bt= 0;
		flag.menu_flag[2] = M4_3_2;
		flag.menu_aux[4] = 1;
	}
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==3 && password.flag && flag.menu_flag[2]==M4_3_2){
		flag.bt= 0;
		flag.menu_aux[4]++;
	}
	/*---MENU TANQUE--*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==4 && password.flag && !flag.menu_flag[2]){
		flag.bt= 0;
		flag.menu_flag[2] = M4_3_3;
		flag.menu_aux[4] = 1;
	}
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[1]==3 && flag.menu_aux[2]==4 && password.flag && flag.menu_flag[2]==M4_3_3){
		flag.bt= 0;
		tanque = flag.menu_aux[4];
		flagaviso = 1;
	}
	/*---ENTRA NO SUBMENU 4-4 QUARTA OPÇÃO---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==0 && flag.menu_aux[1]==4){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M4_4;
	}

	/*--NAVEGA NA QUARTA OPÇÃO DO MENU 4---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_4 && flag.menu_aux[1]==4){
		flag.bt= 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1]=0;
		flag.menu_aux[1]=4;
	}
	/*============================================================================================================================*/
#ifdef DEBUG
	/*---ENTRA NO SUBMENU 4-5 QUINTA OPÇÃO---*/				//SEÇÃO DEBUG
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==0 && flag.menu_aux[1]==5){
		flag.bt = 0;
		flag.menu_aux[2] = 1;
		flag.menu_flag[1] = M4_5;
	}
	/*--NAVEGA NA QUINTA OPÇÃO DO MENU 4---*/
	else if(flag.bt==OK && flag.menu==4 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_5 && flag.menu_aux[1]==5){
		flag.bt= 0;
		flagaviso=1;
		flag.menu_aux[2] = 0;
		//flag.menu_flag[1]=0;
		flag.menu_aux[1]=5;							//SEÇÃO DEBUG
	}
#endif
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
	/*---VOLTA DO SUBMENU 2-1 PARA O MENU 2---*/
	else if(flag.bt==BCK && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_flag[1]==M2_1 && flag.menu_aux[1]==5){
		flag.bt = 0;
		flag.menu_flag[1] = 0;
	}
	/*---VOLTA DO SUBMENU 2 PARA O MENU 1---*/
	else if(flag.bt==BCK && flag.menu==2 && flag.menu_flag[0]==M2  && flag.menu_flag[1]==0 && flag.trava==1){
		flag.bt = 0;
		flag.menu_aux[1] = 0;
		flag.menu_flag[0] = 0;
		flag.trava = 0;
	}
	/*---VOLTA DO SUBMENU 3 PARA O MENU 1---*/
	else if(flag.bt==BCK && flag.menu==3 && flag.menu_flag[0]==M3  && flag.menu_flag[1]==0 && flag.trava==1){
		flag.bt = 0;
		flag.menu_aux[1] = 0;
		flag.menu_flag[0] = 0;
		flag.trava = 0;
	}

	/*---VOLTA DO SUBMENU 3-1 PARA O SUBMENU1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && !flag.menu_flag[2]){
		flag.bt = 0;
		flag.menu_flag[1] = 0;
		flag.menu_aux[1]=1;
	}
	/*---VOLTA SUBMENU 3-1-1---*/
	else if(flag.bt==BCK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_1){
		flag.bt = 0;
		flag.menu_aux[0] = 0;
		flag.menu_flag[2]=0;
		flag.menu_aux[2]=1;
		flaghistba = 0;
		auxindex = 0;
		point=0;
	}
	/*---VOLTA SUBMENU 3-1-2---*/
	else if(flag.bt==BCK && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_2){
		flag.bt = 0;
		flag.menu_aux[0] = 0;
		flag.menu_flag[2]=0;
		flag.menu_aux[2]=2;
		flaghistb = 0;
		auxindex1 = 0;
		point=0;
	}
	/*---VOLTA DO SUBMENU 3-2 PARA O SUBMENU1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && !pass.flag){
		flag.bt = 0;
		flag.menu_aux[2]--;
		if(flag.menu_aux[2]<1){
			flag.menu_flag[1] = 0;
			flag.menu_aux[1]=2;
		}
	}
	/*---VOLTA DO SUBMENU 3-2-1 PARA O SUBMENU 3-2---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag && !flag.menu_aux[3]){
		flag.bt = 0;
		flag.menu_flag[1] = 0;
		flag.menu_aux[1] = 1;
		flag.menu_aux[2] = 2;
		pass.flag = 0;
	}
	/*---VOLTA DO SUBMENU 3-2-1 SUBTELA OPCAO1 PARA O SUBMENU 3-2-1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag && flag.menu_aux[2]==1 && flag.menu_aux[3]){
		flag.bt = 0;
		flag.menu_aux[3] = 0;
		flag.menu_aux[2] = 1;
	}
	/*---VOLTA DO SUBMENU 3-2-1 SUBTELA OPCAO2 PARA O SUBMENU 3-2-1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag && flag.menu_aux[2]==2 && flag.menu_aux[3]){
		flag.bt = 0;
		flag.menu_aux[3] = 0;
		flag.menu_aux[2] = 2;
	}
	/*---VOLTA DO SUBMENU 3-2-1 SUBTELA OPCAO2 PARA O SUBMENU 3-2-1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag && flag.menu_aux[2]==3 && flag.menu_aux[3]){
		flag.bt = 0;
		flag.menu_aux[3] = 0;
		flag.menu_aux[2] = 3;
	}
	/*---VOLTA DO SUBMENU 4 PARA O MENU 1---*/
	else if(flag.bt==BCK && flag.menu==4 && flag.menu_flag[0]==M4 && !flag.menu_flag[1]){
		flag.bt= 0;
		flag.menu_flag[0]=0;
		flag.menu_aux[1]=0;
		flag.trava = 0;
	}
	/*---VOLTA SUBMENU 4_1 PARA SUBMENU 4---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_1){
		flag.bt = 0;
		flag.menu_flag[1]=0;
	}
	/*---VOLTA SUBMENU 4_2 PARA SUBMENU 4---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_2){
		flag.bt = 0;
		flag.menu_aux[4]--;
		if(flag.menu_aux[4]<1){
			flag.menu_flag[1]=0;
			flag.menu_aux[4]=0;
		}
	}
	/*---VOLTA DO SUBMENU 4-3 PARA O SUBMENU1---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && !password.flag){
		flag.bt = 0;
		flag.menu_aux[2]--;
		if(flag.menu_aux[2]<1){
			flag.menu_flag[1] = 0;
			flag.menu_aux[1]=2;
		}
	}
	/*---VOLTA DO SUBMENU 4-3-1 PARA O SUBMENU 4-3---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && !flag.menu_flag[2] && !flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_flag[1] = 0;
		flag.menu_aux[1] = 3;
		flag.menu_aux[2] = 2;
		password.flag = 0;
	}
	/*---VOLTA DO SUBMENU RESET HORIMETRO PARA O SUBMENU 3-2---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && !flag.menu_flag[2] && flag.menu_aux[4]){
		flag.bt= 0;
		flag.menu_aux[4]--;
		if(flag.menu_aux[4]<1){
			flag.menu_aux[4]=0;
		}
	}
	/*---VOLTA DO SUBMENU 4-3-1 PARA O SUBMENU 3-2---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && flag.menu_flag[2]==M4_3_1 && !flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_flag[2] = 0;
		flag.menu_aux[2] = 2;
	}
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==1 && flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[4]--;
		if(flag.menu_aux[4]<1){
			flag.menu_aux[3] = 1;
		}
	}
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==2 && flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[4]--;
		if(flag.menu_aux[4]<1){
			flag.menu_aux[3] = 2;
		}
	}
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==3 && flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[4]--;
		if(flag.menu_aux[4]<1){
			flag.menu_aux[3] = 3;
		}
	}
	/*---VOLTA DO SUBMENU 4-3-2 PARA O SUBMENU 3-2---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && flag.menu_flag[2]==M4_3_2 && flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[4]--;
		flag.menu_flag[2] = 0;
		if(flag.menu_aux[4]<1){
			flag.menu_aux[2] = 3;
		}
	}
	/*---VOLTA DO SUBMENU 4-3-3 PARA O SUBMENU 3-2---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2] && password.flag && flag.menu_flag[2]==M4_3_3 && flag.menu_aux[3]){
		flag.bt = 0;
		flag.menu_flag[2] = 0;
		flag.menu_aux[3]=0;
		flag.menu_aux[2] = 4;
	}
	/*---VOLTA SUBMENU 4_4 PARA SUBMENU 4---*/
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_4){
		flag.bt = 0;
		flag.menu_flag[1]=0;
	}
#ifdef 	DEBUG
	/*============================================================================================================================*/
	/*---VOLTA SUBMENU 4_5 PARA SUBMENU 4---*/		//SEÇÃO DEBUG
	else if(flag.bt==BCK && flag.menu>0 && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_5){
		flag.bt = 0;
		flag.menu_flag[1]=0;
	}									//SEÇÃO DEBUG
	/*============================================================================================================================*/
#endif
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
	/*---SEÇÃO HORA---*/
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
	/*---MOVIMENTA SUBMENU 2---*/
	else if(flag.bt==UP && flag.menu==2 && flag.menu_flag[0]==M2 && flag.menu_flag[1]==0 && flag.trava==1){
		flag.bt = 0;
		flag.menu_aux[1]--;
		if(flag.menu_aux[1]<1){
			flag.menu_aux[1] = 5;
		}
	}
	/*---MOVIMENTA SUBMENU 3---*/
	else if(flag.bt==UP && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==0 && flag.trava==1){
		flag.bt = 0;
		flag.menu_aux[1]--;
		if(flag.menu_aux[1]<1){
			flag.menu_aux[1] = 2;
		}
	}
	/*---MOVIMENTA SUBMENU 3-1---*/
	else if(flag.bt==UP && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && !flag.menu_flag[2]){
		flag.bt = 0;
		flag.menu_aux[2]--;
		if(flag.menu_aux[2]<1){
			flag.menu_aux[2] = 2;
		}
	}
	/*---MOVIMENTA SUBMENU 3-1-1---*/
	else if(flag.bt==UP && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_1){
		flag.bt = 0;
		auxindexba = (indexba-0x200);
		if(indexba>0x230 && indexba<0x260){ //PARA PAGINA INTEIRA USAR 48 E 96
			while(auxindexba%48!=0){
				indexba--;
				auxindexba--;
			}
			//while(indexba%48!=0)indexba--;	//PARA PAGINA INTEIRA
			indexba-=48;
			point = 0;
		}
		else if(indexba>0x230 && auxindex>=indexba){
			if(auxindexba%48!=0){
				//if(indexba%48!=0){				//PARA PAGINA INTEIRA
				while(auxindexba%48!=0){
					indexba--;
					auxindexba--;
				}
				//while(indexba%48!=0)indexba--;	//PARA PAGINA INTEIRA
				indexba-=48;
				point = 0;
			}
			else{
				indexba-=96;
				point = 0;
			}
		}
	}
	/*---MOVIMENTA SUBMENU 3-1-2---*/
	else if(flag.bt==UP && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_2){
		flag.bt = 0;
		if(indexbr>48 && indexbr<96){
			while(indexbr%48!=0)indexbr--;
			indexbr-=48;
			point = 0;
		}
		else if(indexbr>48 && auxindex1>=indexbr){
			if(indexbr%48!=0){
				while(indexbr%48!=0)indexbr--;
				indexbr-=48;
				point = 0;
			}
			else{
				indexbr-=96;
				point = 0;
			}
		}
	}
	/*---SEÇÃO SENHA---*/
	/*---INCREMENTO DA VARIAVEL DIGITO1---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==1 && !pass.flag){
		flag.bt = 0;
		pass.dig1++;
		if(pass.dig1>9){
			pass.dig1 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO2---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==2 && !pass.flag){
		flag.bt = 0;
		pass.dig2++;
		if(pass.dig2>9){
			pass.dig2 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO3---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==3 && !pass.flag){
		flag.bt = 0;
		pass.dig3++;
		if(pass.dig3>9){
			pass.dig3 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO4---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==4 && !pass.flag){
		flag.bt = 0;
		pass.dig4++;
		if(pass.dig4>9){
			pass.dig4 = 0;
		}
	}
	/*---MOVIMENTA SUBMENU 3-2---*/
	else if(flag.bt==UP && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag){
		flag.bt = 0;
		flag.menu_aux[2]--;
		if(flag.menu_aux[2]<1){
			flag.menu_aux[2] = 3;
		}
	}
	/*---MOVIMENTA SUBMENU 4-1---*/
	else if(flag.bt==UP && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==0){
		flag.bt = 0;
		flag.menu_aux[1]--;
#ifdef DEBUG
		if(flag.menu_aux[1]<1){
			flag.menu_aux[1] = 5; /**/ //INCLUIR PARA SEÇÃO DEBUG
		}
#else
		if(flag.menu_aux[1]<1){
			flag.menu_aux[1] = 4;
		}
#endif
	}
	/*---MOVIMENTA SUBMENU 4-1---*/
	else if(flag.bt==UP && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_1){
		flag.bt = 0;
		sttbeep--;
		if(sttbeep<1){
			sttbeep = 2;
		}
	}
	/*---MOVIMENTA SUBMENU 4-2---*/
	else if(flag.bt==UP && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_2 && flag.menu_aux[4]==1){
		flag.bt = 0;
		pmin++;
		if(pmin>250){
			pmin=1;
		}
	}
	/*---MOVIMENTA SUBMENU 4-2-2---*/
	else if(flag.bt==UP && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_2 && flag.menu_aux[4]==2){
		flag.bt = 0;
		pmax++;
		if(pmax>250){
			pmax=1;
		}
	}
	/*---SEÇÃO SENHA---*/
	/*---INCREMENTO DA VARIAVEL DIGITO1---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==1 && !password.flag){
		flag.bt = 0;
		password.dig1++;
		if(password.dig1>9){
			password.dig1 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO2---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==2 && !password.flag){
		flag.bt = 0;
		password.dig2++;
		if(password.dig2>9){
			password.dig2 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO3---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==3 && !password.flag){
		flag.bt = 0;
		password.dig3++;
		if(password.dig3>9){
			password.dig3 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO4---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==4 && !password.flag){
		flag.bt = 0;
		password.dig4++;
		if(password.dig4>9){
			password.dig4 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO5---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==5 && !password.flag){
		flag.bt = 0;
		password.dig5++;
		if(password.dig5>9){
			password.dig5 = 0;
		}
	}
	/*---INCREMENTO DA VARIAVEL DIGITO6---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==6 && !password.flag){
		flag.bt = 0;
		password.dig6++;
		if(password.dig6>9){
			password.dig6 = 0;
		}
	}
	/*---MOVIMENTA SUBMENU 4-3---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && !flag.menu_flag[2]){
		flag.bt = 0;
		flag.menu_aux[2]--;
		if(flag.menu_aux[2]<1){
			flag.menu_aux[2] = 4;
		}
	}
	/*---MOVIMENTA SUBMENU 4-3-1---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && !flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[3]--;
		if(flag.menu_aux[3]<1){
			flag.menu_aux[3] = 3;
		}
	}
	/*---INCREMENTA PRIMEIRA OPÇÃO SUBMENU 4-3-1 SEÇÃO1---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==1 && flag.menu_aux[4]==1){
		flag.bt = 0;
		inter.primeira_man[0]+=5;
		if(inter.primeira_man[0]>100){
			inter.primeira_man[0]=0;
		}
	}
	/*---INCREMENTA SEGUNDA OPÇÃO SUBMENU 4-3-1 SEÇÃO1---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==1 && flag.menu_aux[4]==2){
		flag.bt = 0;
		inter.intervalo_man[0]+=5;
		if(inter.intervalo_man[0]>1000){
			inter.intervalo_man[0]=0;
		}

	}
	/*---INCREMENTA PRIMEIRA OPÇÃO SUBMENU 4-3-1 SEÇÃO2---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==2 && flag.menu_aux[4]==1){
		flag.bt = 0;
		inter.primeira_man[1]+=5;
		if(inter.primeira_man[1]>100){
			inter.primeira_man[1]=0;
		}
	}
	/*---INCREMENTA SEGUNDA OPÇÃO SUBMENU 4-3-1 SEÇÃO2---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==2 && flag.menu_aux[4]==2){
		flag.bt = 0;
		inter.intervalo_man[1]+=5;
		if(inter.intervalo_man[1]>1000){
			inter.intervalo_man[1]=0;
		}
	}
	/*---INCREMENTA PRIMEIRA OPÇÃO SUBMENU 4-3-1 SEÇÃO3---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==3 && flag.menu_aux[4]==1){
		flag.bt = 0;
		inter.limpeza[0]+=5;
		if(inter.limpeza[0]>100){
			inter.limpeza[0]=0;
		}
	}
	/*---MOVIMENTA SUBMENU 4-3-2---*/
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_2 && flag.menu_aux[4]){
		flag.bt = 0;
		inter.temp_oci[1]+=10;
		if(inter.temp_oci[1]>100){
			inter.temp_oci[1]=0;
		}
	}
	/*---INCREMENTA PRIMEIRA OPÇÃO SUBMENU 4-3-3---*/			//TANQUE
	else if(flag.bt==UP && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_3 && flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[4]--;
		if(flag.menu_aux[4]<1){
			flag.menu_aux[4]=2;
		}
	}
	/*============================================================================================================================*/
#ifdef DEBUG
	/*-- SEÇÃO dEBUG 4-5--*/				//SEÇÃO DEBUG
	else if(flag.bt==UP && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_5){
		flag.bt = 0;
		aux.manaux[0]++;
		aux.manaux[1]++;
		aux.manaux[2]++;
		if(aux.manaux[0]>99){
			aux.manaux[0] = 1;
			aux.manaux[1] = 1;
			aux.manaux[2] = 1;
		}								//SEÇÃO DEBUG
	}/**/
#endif
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
		if(diasem<01)diasem=7;
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
	/*---MOVIMENTA SUBMENU 2---*/
	else if(flag.bt==DW && flag.menu==2 && flag.menu_flag[0]==M2 && flag.trava==1 && flag.menu_flag[1]==0){
		flag.bt = 0;
		flag.menu_aux[1]++;
		if(flag.menu_aux[1]>5){
			flag.menu_aux[1] = 1;
		}
	}
	/*---MOVIMENTA SUBMENU 3---*/
	else if(flag.bt==DW && flag.menu==3 && flag.menu_flag[0]==M3 && flag.trava==1 && flag.menu_flag[1]==0){
		flag.bt = 0;
		flag.menu_aux[1]++;
		if(flag.menu_aux[1]>2){
			flag.menu_aux[1] = 1;
		}
	}
	/*---MOVIMENTA SUBMENU 3-1---*/
	else if(flag.bt==DW && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && !flag.menu_flag[2]){
		flag.bt = 0;
		flag.menu_aux[2]++;
		if(flag.menu_aux[2]>2){
			flag.menu_aux[2] = 1;
		}
	}
	/*---MOVIMENTA SUBMENU 3-1-1---*/
	else if(flag.bt==DW && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_1){
		flag.bt = 0;
		if(auxindex>48 && indexba<auxindex)point = 0;
	}
	/*---MOVIMENTA SUBMENU 3-1-2---*/
	else if(flag.bt==DW && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_1 && flag.menu_flag[2]==M3_1_2){
		flag.bt = 0;
		if(auxindex1>48 && indexbr<auxindex1)point = 0;
	}
	/*---SEÇÃO SENHA---*/
	/*---DECREMENTO DA VARIAVEL DIGITO1---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==1 && !pass.flag){
		flag.bt = 0;
		if(pass.dig1<1){
			pass.dig1 = 10;
		}
		pass.dig1--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO2---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==2 && !pass.flag){
		flag.bt = 0;
		if(pass.dig2<1){
			pass.dig2 = 10;
		}
		pass.dig2--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO3---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==3 && !pass.flag){
		flag.bt = 0;
		if(pass.dig3<1){
			pass.dig3 = 10;
		}
		pass.dig3--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO4---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && flag.menu_aux[2]==4 && !pass.flag){
		flag.bt = 0;
		if(pass.dig4<1){
			pass.dig4 = 10;
		}
		pass.dig4--;
	}
	/*---MOVIMENTA SUBMENU 3-2---*/
	else if(flag.bt==DW && flag.menu==3 && flag.menu_flag[0]==M3 && flag.menu_flag[1]==M3_2 && pass.flag){
		flag.bt = 0;
		flag.menu_aux[2]++;
		if(flag.menu_aux[2]>3){
			flag.menu_aux[2] = 1;
		}
	}
	/*---MOVIMENTA SUBMENU 4-1---*/
	else if(flag.bt==DW && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==0){
		flag.bt = 0;
		flag.menu_aux[1]++;
#ifdef DEBUG
		if(flag.menu_aux[1]>5){
			flag.menu_aux[1] = 1;
		}
#else
		if(flag.menu_aux[1]>4){
			flag.menu_aux[1] = 1;
		}
#endif
	}
	/*---MOVIMENTA SUBMENU 4-1---*/
	else if(flag.bt==DW && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_1){
		flag.bt = 0;
		sttbeep++;
		if(sttbeep>2){
			sttbeep = 1;
		}
	}
	/*---MOVIMENTA SUBMENU 4-2---*/
	else if(flag.bt==DW && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_2 && flag.menu_aux[4]==1){
		flag.bt = 0;
		pmin--;
		if(pmin<1){
			pmin=250;
		}
	}

	/*---MOVIMENTA SUBMENU 4-2-2---*/
	else if(flag.bt==DW && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_2 && flag.menu_aux[4]==2){
		flag.bt = 0;
		pmax--;
		if(pmax<1){
			pmax=250;
		}
	}
	/*---SEÇÃO SENHA---*/
	/*---DECREMENTO DA VARIAVEL DIGITO1---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==1 && !password.flag){
		flag.bt = 0;
		if(password.dig1<1){
			password.dig1 = 10;
		}
		password.dig1--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO2---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==2 && !password.flag){
		flag.bt = 0;
		if(password.dig2<1){
			password.dig2 = 10;
		}
		password.dig2--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO3---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==3 && !password.flag){
		flag.bt = 0;
		if(password.dig3<1){
			password.dig3 = 10;
		}
		password.dig3--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO4---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==4 && !password.flag){
		flag.bt = 0;
		if(password.dig4<1){
			password.dig4 = 10;
		}
		password.dig4--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO5---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==5 && !password.flag){
		flag.bt = 0;
		if(password.dig5<1){
			password.dig5 = 10;
		}
		password.dig5--;
	}
	/*---DECREMENTO DA VARIAVEL DIGITO6---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && flag.menu_aux[2]==6 && !password.flag){
		flag.bt = 0;
		if(password.dig6<1){
			password.dig6 = 10;
		}
		password.dig6--;
	}
	/*---MOVIMENTA SUBMENU 4-3---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && !flag.menu_flag[2]){
		flag.bt = 0;
		flag.menu_aux[2]++;
		if(flag.menu_aux[2]>4){
			flag.menu_aux[2] = 1;
		}
	}
	/*---MOVIMENTA SUBMENU 4-3-1---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && !flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[3]++;
		if(flag.menu_aux[3]>3){
			flag.menu_aux[3] = 1;
		}
	}
	/*---DECREMENTA PRIMEIRA OPÇÃO SUBMENU 4-3-1 SEÇÃO1---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==1 && flag.menu_aux[4]==1){
		flag.bt = 0;
		inter.primeira_man[0]-=5;
		if(inter.primeira_man[0]<1){
			inter.primeira_man[0]=100;
		}
	}
	/*---DECREMENTA SEGUNDA OPÇÃO SUBMENU 4-3-1 SEÇÃO1---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==1 && flag.menu_aux[4]==2){
		flag.bt = 0;
		inter.intervalo_man[0]-=5;
		if(inter.intervalo_man[0]<1){
			inter.intervalo_man[0]=1000;
		}

	}
	/*---DECREMENTA PRIMEIRA OPÇÃO SUBMENU 4-3-1 SEÇÃO2---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==2 && flag.menu_aux[4]==1){
		flag.bt = 0;
		inter.primeira_man[1]-=5;
		if(inter.primeira_man[1]<1){
			inter.primeira_man[1]=100;
		}
	}
	/*---DECREMENTA SEGUNDA OPÇÃO SUBMENU 4-3-1 SEÇÃO2---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==2 && flag.menu_aux[4]==2){
		flag.bt = 0;
		inter.intervalo_man[1]-=5;
		if(inter.intervalo_man[1]<1){
			inter.intervalo_man[1]=1000;
		}
	}
	/*---DECREMENTA SEGUNDA OPÇÃO SUBMENU 4-3-1 SEÇÃO3---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_1 && flag.menu_aux[3]==3 && flag.menu_aux[4]==1){
		flag.bt = 0;
		inter.limpeza[0]-=5;
		if(inter.limpeza[0]<1){
			inter.limpeza[0]=1000;
		}
	}
	/*---MOVIMENTA SUBMENU 4-3-2---*/
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_2 && flag.menu_aux[4]){
		flag.bt = 0;
		if(inter.temp_oci[1]<200)inter.temp_oci[1]-=10;
		else inter.temp_oci[1]=0;
		if(inter.temp_oci[1]<1 || inter.temp_oci[1]<0){
			inter.temp_oci[1]=0;
		}
	}
	/*---INCREMENTA PRIMEIRA OPÇÃO SUBMENU 4-3-3---*/             //TANQUE
	else if(flag.bt==DW && flag.menu_flag[0]==M4 && flag.menu_flag[1]==M4_3 && password.flag && flag.menu_flag[2]==M4_3_3 && flag.menu_aux[4]){
		flag.bt = 0;
		flag.menu_aux[4]++;
		if(flag.menu_aux[4]>2){
			flag.menu_aux[4]=1;
		}
	}
	/*============================================================================================================================*/
#ifdef DEBUG
	/*-- SEÇÃO dEBUG 4-5--*/				//SEÇÃO DEBUG
	else if(flag.bt==DW && flag.menu==4 && flag.menu_flag[0]==M4 && flag.trava==1 && flag.menu_flag[1]==M4_5){
		flag.bt = 0;
		aux.manaux[0]--;
		aux.manaux[1]--;
		aux.manaux[2]--;
		if(aux.manaux[0]<1){
			aux.manaux[0] = 99;
			aux.manaux[1] = 99;
			aux.manaux[2] = 99;
		}
	}/**/
	/*============================================================================================================================*/
#endif
	else{
		flag.bt = 0;
		flagTx = 0;
		flagRx = 0;
	}
}

void atualiza_menu(void){
	char date[]="02/11/22";
	char horas[]="00:00";
	char horim[]= "H:00000:00";
	char tempteste[]="00:00:00"; //PARA CALIBRAÇÃO RTC
	char hm[]= "HM:00000:00";
	char hm1[]= "HM:00000:00";
	char hm2[]= "HM:00000:00";
	char tempo[]= "T:00000:00";
	char intervalo[] = "000";
	char pressrange[] = "000 bar";
	char osc[] = "Desligado";
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
			/*if(hori.temp1>99 && hori.temp1<999)sprintf(tempo,"T:%03d:%02d", hori.temp1, hori.min1[1]);
			else if(hori.temp1>999 && hori.temp1<9999 )sprintf(tempo,"T:%04d:%02d", hori.temp1, hori.min1[1]);
			else if(hori.temp1>9999)sprintf(tempo,"T:%05d:%02d", hori.temp1, hori.min1[1]);
			else sprintf(tempo,"T:%02d:%02d", hori.temp1, hori.min1[1]);*/
			if(flagh[0] && !flagh[2]){
				OLED_DrawStr("33:12", RIGHT, 53, 1);
			}
			else if(flagh[2]){
				if(conttg[2]>600){
					psc = ~psc;
					conttg[2] = 0;
				}
				if(psc){
					OLED_DrawStr("33:12", RIGHT, 53, 1);
				}
			}
			if(batt<8.0 || batt>16.5){
				if(conttg[5]>600){
					tggl3 = !tggl3;
					conttg[5] = 0;
				}
			}
			else tggl3 = 1;

			OLED_DrawStr("13.2", 25, 50, tggl3);
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

			indicador(435);
			OLED_DrawStr("Pres:", 48, 16, 1);
			OLED_DrawStr("06.42", 80, 16, 1);
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
			OLED_DrawStr("Horimetro", CENTER, 1, 1);
			if(hori.hr1>99 && hori.hr1<999)sprintf(horim,"H:%03d:%02d", hori.hr1, hori.min1[0]);
			else if(hori.hr1>999 && hori.hr1<9999)sprintf(horim,"H:%04d:%02d", hori.hr1, hori.min1[0]);
			else if(hori.hr1>9999)sprintf(horim,"H:%05:%02d", hori.hr1, hori.min1[0]);
			else sprintf(horim,"H:%02d:%02d", hori.hr1, hori.min1[0]);
			OLED_DrawStr(horim, CENTER, 16, 1);
			/*LIMITA CASAS DECIMAIS TEMPORIZADOR*/
			/*if(hori.temp1>99 && hori.temp1<999)sprintf(tempo,"T:%03d:%02d", hori.temp1, hori.min1[1]);
			else if(hori.temp1>999 && hori.temp1<9999 )sprintf(tempo,"T:%04d:%02d", hori.temp1, hori.min1[1]);
			else if(hori.temp1>9999)sprintf(tempo,"T:%05d:%02d", hori.temp1, hori.min1[1]);
			else sprintf(tempo,"T:%02d:%02d", hori.temp1, hori.min1[1]);*/
			if(flagh[0] && !flagh[2]){
				OLED_DrawStr("33:12", CENTER, 33, 1);
			}
			OLED_UpdateScreen();
			break;
		case 4:
			tela4 = 1;
			OLED_UpdateScreen();
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_12);
			OLED_DrawStr("Nivel", CENTER, 16, 1);
			indicador(435);
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

			OLED_DrawStr("6.42", CENTER, 33, 1);
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
				}
			}
			break;
		case 2:
			if(!flag.trava && init){
				menuprincipal(1, 0, 1, 1);
			}
			else if(flag.trava && flag.menu_flag[0]==M2 && init){
				switch (flag.menu_aux[1]){
				case 1:
					if(!flagaviso){
						menutimer(0, 1, 1, 1);
					}
					else{
						flagaviso=0;
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_10);
						flagespera = 1;
						OLED_DrawStr("Temporizador", CENTER, 1, 1);
						OLED_DrawStr("iniciado!", CENTER, 16, 1);
						OLED_UpdateScreen();
						grava_flagt();
						HAL_Delay(1500);
						flag.menu_flag[0]=0;
						flag.menu_aux[1]=0;
						flag.trava = 0;
						flagespera = 0;
					}
					break;
				case 2:
					if(!flagaviso){
						menutimer(1, 0, 1, 1);
					}
					else{
						flagaviso=0;
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_10);
						flagespera = 1;
						OLED_DrawStr("Temporizador", CENTER, 1, 1);
						OLED_DrawStr("pausado!", CENTER, 16, 1);
						OLED_UpdateScreen();
						grava_flagt();
						HAL_Delay(1500);
						flag.menu_flag[0]=0;
						flag.menu_aux[1]=0;
						flag.trava = 0;
						flagespera = 0;
					}
					break;
				case 3:
					if(!flagaviso){
						menutimer(1, 1, 0, 1);
					}
					else{
						flagaviso=0;
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_10);
						flagespera = 1;
						OLED_DrawStr("Temporizador", CENTER, 1, 1);
						OLED_DrawStr("reiniciado!", CENTER, 16, 1);
						OLED_UpdateScreen();
						HAL_Delay(1500);
						flag.menu_flag[0]=0;
						flag.menu_aux[1]=0;
						flag.trava = 0;
						flagespera = 0;
					}
					break;
				case 4:
					if(!flagaviso){
						menutimer(1, 1, 1, 0);
					}
					else{
						flagaviso=0;
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_10);
						flagespera = 1;
						OLED_DrawStr("Temporizador", CENTER, 1, 1);
						OLED_DrawStr("desabilitado!", CENTER, 16, 1);
						OLED_UpdateScreen();
						grava_flagt();
						HAL_Delay(1500);
						flag.menu_flag[0]=0;
						flag.menu_aux[1]=0;
						flag.trava = 0;
						flagespera = 0;
					}
					break;
				case 5:
					if(!flag.menu_flag[1]){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Pausa  temp.         ", 1, 0, 1);
						OLED_DrawStr("Reinicia temp.       ", 1, 16, 1);
						OLED_DrawStr("Desabilita temp.     ", 1, 33, 1);
						OLED_DrawStr("Valor parcial.       ", 1, 49, 0);
						OLED_UpdateScreen();
					}
					else if(flag.menu_flag[1]==M2_1) {
						sprintf(tempo,"T:%05d:%02d", hori.temp1, hori.min1[1]);
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Valor parcial", CENTER, 1, 1);
						OLED_DrawStr(tempo, CENTER, 16, 1);
						OLED_UpdateScreen();
					}
					break;
				}
			}
			break;
		case 3:
			if(!flag.trava && init){
				menuprincipal(1, 1, 0, 1);
			}
			else if(flag.trava && flag.menu_flag[0]==M3 && init){
				if(flagman1==1){
					aviso_manutencao();
				}
				flagman1=0;
				switch (flag.menu_aux[1]){
				case 1:
					if(flag.menu_flag[1]==0){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Historico man.     ", 1, 1, 0);
						OLED_DrawStr("Registrar man.      ", 1, 15, 1);
						OLED_UpdateScreen();
					}
					else if(flag.menu_flag[1]==M3_1){
						switch (flag.menu_aux[2]){
						case 1:
							if(!flag.menu_flag[2]){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_12);
								OLED_DrawStr("Hist. Bomba dagua ", 1, 1, 0);
								OLED_DrawStr("Hist. Barra        ", 1, 15, 1);
								OLED_UpdateScreen();
							}
							else if(flag.menu_flag[2]==M3_1_1) historico_bagua();
							break;
						case 2:
							if(!flag.menu_flag[2]){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_12);
								OLED_DrawStr("Hist. Bomba dagua ", 1, 1, 1);
								OLED_DrawStr("Hist. Barra        ", 1, 15, 0);
								OLED_UpdateScreen();
							}
							else if(flag.menu_flag[2]==M3_1_2) historico_barra();
							break;
						}
					}

					break;
				case 2:
					if(flag.menu_flag[1]==0){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Historico man.    ", 1, 1, 1);
						OLED_DrawStr("Registrar man.      ", 1, 15, 0);
						OLED_UpdateScreen();
					}
					else if(!pass.flag && flag.menu_flag[1]==M3_2){
						switch(flag.menu_aux[2]){
						case 1:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass1,"%d***", pass.dig1);
							OLED_DrawStr(pass1, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 2:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass1,"*%d**", pass.dig2);
							OLED_DrawStr(pass1, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 3:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass1,"**%d*", pass.dig3);
							OLED_DrawStr(pass1, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 4:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass1,"***%d", pass.dig4);
							OLED_DrawStr(pass1, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						default:
							if(pass.dig1==d1 && pass.dig2==d2 && pass.dig3==d3 && pass.dig4==d4 && flag.menu_aux[2]>4){
								pass.flag = 1;
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_12);
								flagespera = 1;
								OLED_DrawStr("SENHA OK!", CENTER, 15, 1);
								OLED_UpdateScreen();
								HAL_Delay(1200);
								pass.dig1 = 0;
								pass.dig2 = 0;
								pass.dig3 = 0;
								pass.dig4 = 0;
								flag.menu_aux[2]=1;
								flagespera = 0;
							}
							else if((pass.dig1!=d1 || pass.dig2!=d2 || pass.dig3!=d3 || pass.dig4!=d4) && flag.menu_aux[2]>4){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_12);
								flagespera = 1;
								OLED_DrawStr("SENHA ERRADA!", CENTER, 15, 1);
								OLED_UpdateScreen();
								HAL_Delay(1200);
								pass.dig1 = 0;
								pass.dig2 = 0;
								pass.dig3 = 0;
								pass.dig4 = 0;
								flag.menu_aux[2]=1;
								flagespera = 0;
							}
							break;
						}
					}
					else if(pass.flag && flag.menu_flag[1]==M3_2){
						switch (flag.menu_aux[2]){
						case 1:
							if(!flag.menu_aux[3]){
								registraman(0, 1, 1);
							}
							else if(flag.menu_aux[3]==1){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								OLED_DrawStr("Registrar manutencao", CENTER, 1, 1);
								OLED_DrawStr("bomba d'agua?", CENTER, 16, 1);
								OLED_DrawStr("OK para confirmar", CENTER, 32, 1);
								OLED_UpdateScreen();
							}
							else{
								flag.menu_aux[2] = 0;
								flag.menu_aux[3] = 0;
								aux.flagman[1] = 1;
								flag.menu_aux[1] = 2;
								flag.menu_flag[1] = 0;
								pass.flag = 0;
								verifica_manutencao();
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								flagespera = 1;
								OLED_DrawStr("Manutecao registrada!", CENTER, 16, 1);
								grava_man_bomba();
								OLED_UpdateScreen();
								HAL_Delay(1500);
								flagespera = 0;
							}
							break;
						case 2:
							if(!flag.menu_aux[3]){
								registraman(1, 0, 1);
							}
							else if(flag.menu_aux[3]==1){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								OLED_DrawStr("Registrar manutencao", CENTER, 1, 1);
								OLED_DrawStr("oleo barra?", CENTER, 16, 1);
								OLED_DrawStr("OK para confirmar", CENTER, 32, 1);
								OLED_UpdateScreen();
							}
							else{
								flag.menu_aux[2] = 0;
								flag.menu_aux[3] = 0;
								aux.flagman[0] = 1;
								flag.menu_aux[1] = 2;
								flag.menu_flag[1] = 0;
								pass.flag = 0;
								verifica_manutencao();
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								flagespera = 1;
								OLED_DrawStr("Manutecao registrada!", CENTER, 16, 1);
								grava_man_barra();
								OLED_UpdateScreen();
								HAL_Delay(1500);
								flagespera = 0;
							}
							break;
						case 3:
							if(!flag.menu_aux[3]){
								registraman(1, 1, 0);
							}
							else if(flag.menu_aux[3]==1){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								OLED_DrawStr("Registrar manutencao", CENTER, 1, 1);
								OLED_DrawStr("limpeza sensores?", CENTER, 16, 1);
								OLED_DrawStr("OK para confirmar", CENTER, 32, 1);
								OLED_UpdateScreen();
							}
							else{
								flag.menu_aux[2] = 0;
								flag.menu_aux[3] = 0;
								aux.flagman[2] = 1;
								flag.menu_aux[1] = 2;
								flag.menu_flag[1] = 0;
								pass.flag = 0;
								verifica_manutencao();
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								flagespera = 1;
								OLED_DrawStr("Manutecao registrada!", CENTER, 16, 1);
								aux.manaux[2] = 0x0;
								grava_limpeza();
								OLED_UpdateScreen();
								HAL_Delay(1500);
								flagespera = 0;
							}
							break;
						}
					}

				}
			}
			break;
		case 4:
			if(!flag.trava && init){
				menuprincipal(1, 1, 1, 0);
			}
			else if(flag.trava && flag.menu_flag[0]==M4 && init){
				switch (flag.menu_aux[1]){
				case 1:
					if(!flag.menu_flag[1]){
						preferencias(0, 1, 1, 1);
					}
					else{
						if(flagaviso){
							flag.menu_flag[1]=0;
							flagaviso=0;
							OLED_Clear(0);
							flagespera = 1;
							FontSet(Segoe_UI_Eng_10);
							OLED_DrawStr("Opcao registrada!", CENTER, 16, 1);
							indexinter = 0x00;
							inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							inter.limpeza[0] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							inter.temp_oci[0] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							flagh[0] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							flagh[2] = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							aux1 = *(uint32_t*)(memointer+indexinter);
							indexinter+=2;
							sttnvl = *(uint32_t*)(memointer+indexinter);
							indexinter=0;
							/*Apaga memoria*/
							FLASH_apaga(memointer, 1);
							/*Regrava memoria*/
							HAL_FLASH_Unlock();
							HAL_FLASH_OB_Unlock();
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[0])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[1])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.limpeza[0])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.temp_oci[0])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[0])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[1])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[0])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[2])) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttbeep)) != HAL_OK){}
							indexinter+=2;
							if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttnvl)) != HAL_OK){}
							indexinter+=2;
							HAL_FLASH_OB_Lock();
							HAL_FLASH_Lock();
							OLED_UpdateScreen();
							flagbeep = sttbeep;
							grava_limpeza();
							HAL_Delay(1500);
							flagespera = 0;
						}
						else if(flag.menu_flag[1]){
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Beep teclas.", CENTER, 0, 1);
							if(sttbeep==1)OLED_DrawStr("Ligado", CENTER, 16, 1);
							else if(sttbeep==2) OLED_DrawStr("Desligado", CENTER, 16, 1);
							OLED_UpdateScreen();
						}
					}
					break;
				case 2:
					if(!flag.menu_aux[4]){
						preferencias(1, 0, 1, 1);
					}
					else if(flagaviso){
						flag.menu_flag[1]=0;
						flag.menu_aux[4]=0;
						flagaviso=0;
						OLED_Clear(0);
						flagespera = 1;
						FontSet(Segoe_UI_Eng_10);
						OLED_DrawStr("Opcao registrada!", CENTER, 16, 1);
						OLED_UpdateScreen();
						HAL_Delay(1500);
						flagespera = 0;
					}
					else if(flag.menu_aux[4]==1){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Nivel pressao", CENTER, 0, 1);
						OLED_DrawStr("minimo.", CENTER, 16, 1);
						sprintf(pressrange,"%03d bar", pmin);
						OLED_DrawStr(pressrange, CENTER, 33, 1);
						OLED_UpdateScreen();
					}
					else if(flag.menu_aux[4]==2){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Nivel pressao", CENTER, 0, 1);
						OLED_DrawStr("maximo.", CENTER, 16, 1);
						sprintf(pressrange,"%03d bar", pmax);
						OLED_DrawStr(pressrange, CENTER, 33, 1);
						OLED_UpdateScreen();
					}

					break;
				case 3:
					if(!flag.menu_flag[1]){
						preferencias(1, 1, 0, 1);
					}
					else if(!password.flag && flag.menu_flag[1]==M4_3){
						switch(flag.menu_aux[2]){
						case 1:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass2,"%d*****", password.dig1);
							OLED_DrawStr(pass2, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 2:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass2,"*%d****", password.dig2);
							OLED_DrawStr(pass2, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 3:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass2,"**%d***", password.dig3);
							OLED_DrawStr(pass2, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 4:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass2,"***%d**", password.dig4);
							OLED_DrawStr(pass2, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 5:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass2,"****%d*", password.dig5);
							OLED_DrawStr(pass2, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						case 6:
							OLED_Clear(0);
							FontSet(Segoe_UI_Eng_12);
							OLED_DrawStr("Digite a senha", CENTER, 1, 1);
							sprintf(pass2,"*****%d", password.dig6);
							OLED_DrawStr(pass2, CENTER, 15, 0);
							OLED_UpdateScreen();
							break;
						default:
							if(password.dig1==e1 && password.dig2==e2 && password.dig3==e3 && password.dig4==e4 && password.dig5==e5 && password.dig6==e6 && flag.menu_aux[2]>6){
								password.flag = 1;
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_12);
								flagespera = 1;
								OLED_DrawStr("SENHA OK!", CENTER, 15, 1);
								OLED_UpdateScreen();
								HAL_Delay(1200);
								password.dig1 = 0;
								password.dig2 = 0;
								password.dig3 = 0;
								password.dig4 = 0;
								password.dig5 = 0;
								password.dig6 = 0;
								flag.menu_aux[2]=1;
								flagespera = 0;
							}
							else if((password.dig1!=e1 || password.dig2!=e2 || password.dig3!=e3 || password.dig4!=e4 || password.dig5!=e5 || password.dig6!=e6) && flag.menu_aux[2]>6){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_12);
								flagespera = 1;
								OLED_DrawStr("SENHA ERRADA!", CENTER, 15, 1);
								OLED_UpdateScreen();
								HAL_Delay(1200);
								password.dig1 = 0;
								password.dig2 = 0;
								password.dig3 = 0;
								password.dig4 = 0;
								password.dig5 = 0;
								password.dig6 = 0;
								flag.menu_aux[2]=1;
								flagespera = 0;
							}
							break;
						}
					}
					else if(password.flag && flag.menu_flag[1]==M4_3){
						switch (flag.menu_aux[2]){
						case 1:
							if(!flag.menu_aux[4]){
								conffabricante(0, 1, 1, 1);
							}
							else if(flag.menu_aux[4]==1){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								OLED_DrawStr("Registrar", CENTER, 1, 1);
								OLED_DrawStr("Reset Horimetro?", CENTER, 16, 1);
								OLED_DrawStr("OK para confirmar", CENTER, 32, 1);
								OLED_UpdateScreen();
							}
							else{
								/*ZERA O HORIMETRO E LIMPA  A PAGINA DE MEMORIA PARA RECOMEÇAR A GRAVAR OS VALORES DO INICIO*/
								hori.hr1=0x00;
								hori.min1[0]=0x00;
								indexh = 0x0;
								indexm = 0x0;
								grava_horimetro();
								grava_minutos();
								/*-----------------------------------------------------------------------------------------*/
								flag.menu_aux[2] = 1;
								flag.menu_aux[4] = 0;
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								flagespera = 1;
								OLED_DrawStr("Horimetro zerado!", CENTER, 16, 1);
								OLED_UpdateScreen();
								HAL_Delay(1500);
								flagespera = 0;
							}
							break;
						case 2:
							if(!flag.menu_flag[2]){
								conffabricante(1, 0, 1, 1);
							}
							else if(flag.menu_flag[2]==M4_3_1){
								switch(flag.menu_aux[3]){
								case 1:
									if(!flag.menu_aux[4]){
										intervaloman(0, 1, 1);
									}
									else if(flag.menu_aux[4] == 1){
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_12);
										OLED_DrawStr("Horas primeira", CENTER, 0, 1);
										OLED_DrawStr("manutencao", CENTER, 16, 1);
										if(inter.primeira_man[0]>=0xFF)inter.primeira_man[0] = 0;
										if(inter.primeira_man[0]<5)inter.primeira_man[0] = 0;
										sprintf(intervalo,"%03d hrs", inter.primeira_man[0]);
										OLED_DrawStr(intervalo, CENTER, 33, 1);
										OLED_UpdateScreen();
									}
									else if(flag.menu_aux[4] == 2){
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_12);
										OLED_DrawStr("Horas demais", CENTER, 0, 1);
										OLED_DrawStr("manutencoes", CENTER, 16, 1);
										if(inter.intervalo_man[0]>=0xFFFF)inter.intervalo_man[0] = 0;
										if(inter.intervalo_man[0]<5)inter.intervalo_man[0] = 0;
										sprintf(intervalo,"%03d hrs", inter.intervalo_man[0]);
										OLED_DrawStr(intervalo, CENTER, 33, 1);
										OLED_UpdateScreen();
									}
									else{
										flag.menu_aux[3] = 1;
										flag.menu_aux[4] = 0;
										/*Le memoria*/
										indexinter = 0x00;
										aux1 = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.limpeza[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.temp_oci[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										aux2 = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										flagh[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										flagh[2] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										sttbeep = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										sttnvl = *(uint32_t*)(memointer+indexinter);
										indexinter=0;
										/*Apaga memoria*/
										FLASH_apaga(memointer, 1);
										/*Regrava memoria*/
										HAL_FLASH_Unlock();
										HAL_FLASH_OB_Unlock();
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[1])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.limpeza[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.temp_oci[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[1])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[2])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttbeep)) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttnvl)) != HAL_OK){}
										indexinter+=2;
										HAL_FLASH_OB_Lock();
										HAL_FLASH_Lock();
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_10);
										flagespera = 1;
										OLED_DrawStr("Opcoes registradas!!", CENTER, 16, 1);
										grava_limpeza();
										OLED_UpdateScreen();
										HAL_Delay(1500);
										flagespera = 0;
									}
									break;
								case 2:
									if(!flag.menu_aux[4]){
										intervaloman(1, 0, 1);
									}
									else if(flag.menu_aux[4] == 1){
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_12);
										OLED_DrawStr("Horas primeira", CENTER, 0, 1);
										OLED_DrawStr("manutencao", CENTER, 16, 1);
										if(inter.primeira_man[1]>=0xFF)inter.primeira_man[1] = 0;
										sprintf(intervalo,"%03d hrs", inter.primeira_man[1]);
										OLED_DrawStr(intervalo, CENTER, 33, 1);
										OLED_UpdateScreen();
									}
									else if(flag.menu_aux[4] == 2){
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_12);
										OLED_DrawStr("Horas demais", CENTER, 0, 1);
										OLED_DrawStr("manutencoes", CENTER, 16, 1);
										if(inter.intervalo_man[1]>=0xFFFF)inter.intervalo_man[1] = 0;
										sprintf(intervalo,"%03d hrs", inter.intervalo_man[1]);
										OLED_DrawStr(intervalo, CENTER, 33, 1);
										OLED_UpdateScreen();
									}
									else{
										flag.menu_aux[3] = 2;
										flag.menu_aux[4] = 0;
										/*Le memoria*/
										indexinter = 0x00;
										inter.intervalo_man[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										aux1 = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.limpeza[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.temp_oci[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.primeira_man[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										aux2 = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										flagh[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										flagh[2] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										sttbeep = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										sttnvl = *(uint32_t*)(memointer+indexinter);
										indexinter=0x0;
										/*Apaga memoria*/
										FLASH_apaga(memointer, 1);
										/*Regrava memoria*/
										HAL_FLASH_Unlock();
										HAL_FLASH_OB_Unlock();
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[1])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.limpeza[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.temp_oci[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[1])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[2])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttbeep)) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttnvl)) != HAL_OK){}
										indexinter+=2;
										HAL_FLASH_OB_Lock();
										HAL_FLASH_Lock();
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_10);
										flagespera = 1;
										OLED_DrawStr("Opcoes registradas!!", CENTER, 16, 1);
										grava_limpeza();
										OLED_UpdateScreen();
										HAL_Delay(1500);
										flagespera = 0;
									}
									break;
								case 3:
									if(!flag.menu_aux[4]){
										intervaloman(1, 1, 0);
									}
									else if(flag.menu_aux[4] == 1){
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_12);
										OLED_DrawStr("Horas para", CENTER, 0, 1);
										OLED_DrawStr("limpeza", CENTER, 16, 1);
										if(inter.limpeza[0]>=0xFF)inter.limpeza[0] = 0;
										sprintf(intervalo,"%03d hrs", inter.limpeza[0]);
										OLED_DrawStr(intervalo, CENTER, 33, 1);
										OLED_UpdateScreen();
									}
									else{
										flag.menu_aux[3] = 3;
										flag.menu_aux[4] = 0;
										/*Le memoria*/
										indexinter = 0x00;
										inter.intervalo_man[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										aux1 = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.temp_oci[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.primeira_man[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										flagh[0] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										flagh[2] = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										sttbeep = *(uint32_t*)(memointer+indexinter);
										indexinter+=2;
										sttnvl = *(uint32_t*)(memointer+indexinter);
										indexinter=0x00;
										/*Apaga memoria*/
										FLASH_apaga(memointer, 1);
										/*Regrava memoria*/
										HAL_FLASH_Unlock();
										HAL_FLASH_OB_Unlock();
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[1])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.limpeza[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.temp_oci[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[1])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[0])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[2])) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttbeep)) != HAL_OK){}
										indexinter+=2;
										if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttnvl)) != HAL_OK){}
										indexinter+=2;
										HAL_FLASH_OB_Lock();
										HAL_FLASH_Lock();
										OLED_Clear(0);
										FontSet(Segoe_UI_Eng_10);
										flagespera = 1;
										OLED_DrawStr("Opcoes registradas!!", CENTER, 16, 1);
										grava_limpeza();
										OLED_UpdateScreen();
										HAL_Delay(1500);
										flagespera = 0;
									}
									break;
								}
							}
							break;
						case 3:
							if(!flag.menu_aux[4]){
								conffabricante(1, 1, 0, 1);
							}
							else if(flag.menu_aux[4] == 1){
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_12);
								OLED_DrawStr("Tempo menu", CENTER, 0, 1);
								OLED_DrawStr("ocioso", CENTER, 16, 1);
								if(inter.temp_oci[1]>=255)inter.temp_oci[1] = 0;
								sprintf(intervalo,"%03d seg", inter.temp_oci[1]);
								if(inter.temp_oci[1]<10)OLED_DrawStr(osc, CENTER, 33, 1);
								else OLED_DrawStr(intervalo, CENTER, 33, 1);
								OLED_UpdateScreen();
							}
							else{
								flag.menu_aux[2] = 3;
								flag.menu_aux[4] = 0;
								flag.menu_flag[2] = 0;
								inter.temp_oci[0]=inter.temp_oci[1];
								/*Le memoria*/
								indexinter = 0x00;
								inter.intervalo_man[0] = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								inter.limpeza[0] = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								aux1 = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								inter.primeira_man[0] = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								flagh[0] = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								flagh[2] = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								sttbeep = *(uint32_t*)(memointer+indexinter);
								indexinter+=2;
								sttnvl = *(uint32_t*)(memointer+indexinter);
								indexinter=0x00;
								/*Apaga memoria*/
								FLASH_apaga(memointer, 1);
								/*Regrava memoria*/
								HAL_FLASH_Unlock();
								HAL_FLASH_OB_Unlock();
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[0])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[1])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.limpeza[0])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.temp_oci[0])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[0])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[1])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[0])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[2])) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttbeep)) != HAL_OK){}
								indexinter+=2;
								if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttnvl)) != HAL_OK){}
								indexinter+=2;
								HAL_FLASH_OB_Lock();
								HAL_FLASH_Lock();
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								flagespera = 1;
								OLED_DrawStr("Opcoes registradas!!", CENTER, 16, 1);
								grava_limpeza();
								OLED_UpdateScreen();
								HAL_Delay(1500);
								flagespera = 0;
							}
							break;
						case 4:					//---IMPLEMENTAÇÃO FUTURA---//
							if(!flag.menu_aux[4]){
								conffabricante(1, 1, 1, 0);
							}
							else if(flagaviso){
								flag.menu_aux[2] = 4;
								flag.menu_aux[4] = 0;
								flag.menu_flag[2] = 0;
								flagaviso=0;
								OLED_Clear(0);
								FontSet(Segoe_UI_Eng_10);
								flagespera = 1;
								OLED_DrawStr("Opcoes registradas!!", CENTER, 16, 1);
								grava_limpeza();
								OLED_UpdateScreen();
								HAL_Delay(1500);
								flagespera = 0;
							}
							else{
								switch (flag.menu_aux[4]){
								case 1:
									OLED_Clear(0);
									FontSet(Segoe_UI_Eng_12);
									OLED_DrawStr("Tanque 600L  ", 1, 1, 0);
									OLED_DrawStr("Tanque 2500L     ", 1, 16, 1);
									OLED_UpdateScreen();
									break;
								case 2:
									OLED_Clear(0);
									FontSet(Segoe_UI_Eng_12);
									OLED_DrawStr("Tanque 600L  ", 1, 1, 1);
									OLED_DrawStr("Tanque 2500L     ", 1, 16, 0);
									OLED_UpdateScreen();
									break;
								}
							}
							break;
						}

					}
					break;
				case 4:
					if(!flag.menu_flag[1]){
						preferencias(1, 1, 1, 0);
					}
					else{											//SEÇÃO SOBRE PARA PROJETO DEFINITIVO
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_10);
						OLED_DrawStr("Autoban tecnologia.", CENTER, 0, 1);
						OLED_DrawStr("Pato Branco-PR.", CENTER, 16, 1);
						OLED_DrawStr("Versao FW:", 1, 33, 1);
						OLED_DrawStr(vfw, 70, 33, 1);
						OLED_DrawStr("N serie:", 1, 49, 1);
						OLED_DrawStr(nserie,45 , 49, 1);
						/*============================================================================================================================*/
						/*		hori.hr1 =89;					//TESTES MEMORIA HORIMETRO E TIMER
					hori.min1[0]=59;
					hori.temp1 = 89;
					hori.min1[1]=59;	/**/
						/*============================================================================================================================*/
						OLED_UpdateScreen();
					}
					/*============================================================================================================================*/
					/*else{											//PARA CALIBRAÇÃO RELOGIO
					OLED_Clear(0);
					FontSet(Segoe_UI_Eng_10);
					OLED_DrawStr("Autoban tecnologia.", CENTER, 0, 1);
					OLED_DrawStr("Pato Branco-PR.", CENTER, 16, 1);
					OLED_DrawStr("Versao FW:", 1, 33, 1);
					OLED_DrawStr(vfw, 70, 33, 1);
					aRtc_get(&hrtc, &GetTime);
					hr  = GetTime.tm_hour;
					min = GetTime.tm_min;
					sec = GetTime.tm_sec;
					sprintf((char*)tempteste,"%02d:%02d:%02d", hr, min, sec);
					OLED_DrawStr(tempteste, CENTER, 49, 1);
					OLED_UpdateScreen();
				}*/
					/*============================================================================================================================*/
					break;
					/*============================================================================================================================*/
					/*		case 5:							//SEÇÃO DEBUG
				if(!flag.menu_flag[1]){
					OLED_Clear(0);
					FontSet(Segoe_UI_Eng_12);
					OLED_DrawStr("Nivel tanque.     ", 1, 0, 1);
					OLED_DrawStr("Conf. fabricante.  ", 1, 16, 1);
					OLED_DrawStr("Sobre.            ", 1, 33, 1);
					OLED_DrawStr("Debug.            ", 1, 49, 0);
					OLED_UpdateScreen();
				}
				else{
					if(flagaviso){
						flag.menu_flag[1]=0;
						flagaviso=0;
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_10);
						OLED_DrawStr("Opcao registrada!", CENTER, 16, 1);
						OLED_UpdateScreen();
						flagnvl = sttnvl;
						HAL_Delay(1500);
					}
					else if(flag.menu_flag[1]){
						OLED_Clear(0);
						FontSet(Segoe_UI_Eng_12);
						OLED_DrawStr("Horas.", CENTER, 0, 1);
						sprintf(hm,"HM:%05d:00", aux.manaux[0]);
						sprintf(hm1,"HM:%05d:00", aux.manaux[1]);
						sprintf(hm2,"HM:%05d:00", aux.manaux[2]);
						OLED_DrawStr(hm, CENTER, 16, 1);
						OLED_DrawStr(hm1, CENTER, 33, 1);
						OLED_DrawStr(hm2, CENTER, 49, 1);
						OLED_UpdateScreen();
					}
				}
				break;		/**/
					/*============================================================================================================================*/
					break;
				}
			}
			break;
	}
}

uint16_t calcula_media_nivel(uint16_t altura_sensor)
{
	uint32_t aux_media = 0;
	uint8_t cont_for = 0;

	buffer_media[19] = buffer_media[18];
	buffer_media[18] = buffer_media[17];
	buffer_media[17] = buffer_media[16];
	buffer_media[16] = buffer_media[15];
	buffer_media[15] = buffer_media[14];
	buffer_media[14] = buffer_media[13];
	buffer_media[13] = buffer_media[12];
	buffer_media[12] = buffer_media[11];
	buffer_media[11] = buffer_media[10];
	buffer_media[10] = buffer_media[ 9];

	buffer_media[ 9] = buffer_media[ 8];
	buffer_media[ 8] = buffer_media[ 7];
	buffer_media[ 7] = buffer_media[ 6];
	buffer_media[ 6] = buffer_media[ 5];
	buffer_media[ 5] = buffer_media[ 4];
	buffer_media[ 4] = buffer_media[ 3];
	buffer_media[ 3] = buffer_media[ 2];
	buffer_media[ 2] = buffer_media[ 1];
	buffer_media[ 1] = buffer_media[ 0];
	buffer_media[ 0] = altura_sensor;

	cont_amostras++;
	if(cont_amostras >= 20)
		cont_amostras = 20;

	aux_media = 0;
	for(cont_for = 0; cont_for < cont_amostras; cont_for++)
	{
		aux_media += buffer_media[cont_for];
	}
	aux_media = aux_media / cont_amostras;

	return (uint16_t)aux_media;
}
void indicador(uint16_t altura_sensor){
	uint8_t aux = 0x00;
	uint16_t nivel = 0x00;
	char buff[15];

	struct VolumeRange {
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
	const struct VolumeRange rangeT6[] = { //UTILIZAMOS UM VETOR DE STRUCTS COM OS RANGES MIN E MAX E O VALOR EM (L) DESSE INTERVALOS
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
		void initdisp(void){
			OLED_Clear(0);
			OLED_DrawXBM(0 , 0, logov2);
			OLED_UpdateScreen();
			HAL_Delay(2500);
			OLED_Clear(0);
			OLED_UpdateScreen();
			OLED_DrawXBM(0 , 0, maxforte);
			OLED_UpdateScreen();
			HAL_Delay(2500);
			OLED_Clear(0);
			OLED_UpdateScreen();
			verifica_manutencao();
			aviso_manutencao();
		}
		void init_flags(void){
			init=0x00;
			aux.flagman[0] = 0x00;
			aux.flagman[1] = 0x00;
			aux.flagad[0] = 0x00;
			aux.flagad[1] = 0x00;
			indexmt = 0x200;
			indext = 0x200;
			indexman1 = 0X200;
			indexba = 0x200;
			flagRx = 0;
			flagTx = 0;
			auxuart = 1;
			auxuart1 = 1;
			memset(rx_buff, 0, sizeof(rx_buff));
			memset(pressao1, 0, sizeof(pressao1));
			flagbeep = 0x1;
			sttbeep = 0x1;
			flagnvl = 0x1;
			tanque = 0x1;
			tggl3 = 0x1;
			dia = 0x01;
			mes = 0x01;
			ano = 0x01;
			hr = 0x01;
			min = 0x01;
		}
		/*============================================================================================================================*/
		void grava_horimetro(void){					//--- MEIA PAGINA ---//
			if(indexh == 0x0){
				FLASH_apaga(memoh, 1); 					//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
			}
			if(indexh < 0x1FF){
				//FLASH_escreve_32bits (memoh+indexh, hori.hr1);
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();

				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memoh+indexh), hori.hr1) != HAL_OK){
					//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
				}
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexh +=4;
			}
			else
				indexh = 0x0;
		}
		/*============================================================================================================================*/
		/*void grava_horimetro(void){					//---PAGINA INTEIRA 1K---//
	if(indexh == 0x0){
		FLASH_apaga(memoh, 1); 					//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
	}
	if(indexh < 0x400){
		//FLASH_escreve_32bits (memoh+indexh, hori.hr1);
		HAL_FLASH_Unlock();
		HAL_FLASH_OB_Unlock();

		if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memoh+indexh), hori.hr1) != HAL_OK){
			//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
		}
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
		indexh +=4;
	}
	else
		indexh = 0x0;
}*/
		/*============================================================================================================================*/
		void leitura_horimetro(void){
			if(!flagmap){
				for(uint32_t horit=0x00; horit!=0xFFFFFFFF; indexh +=4){
					FLASH_le_32bits(memoh+indexh, &horit);
				}
				if(indexh>0x1FF)indexh = 0;
				if(indexh==4)indexh-=4;
				else indexh-=8; 						//VOLTAMOS 8 POSIÇOES PARA 32BITS POR CONTA DO LAÇO FOR
				flagmap = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
			}
			FLASH_le_32bits(memoh+indexh, &hori.hr1);
		}
		void grava_minutos(void){
			if(indexm == 0x0){
				FLASH_apaga(memom, 1); 					//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
				indexmt = 0x200; 						//INICIAMOS A OUTRA MEDADE DA MEMORIA
			}
			if(indexm<0x1FF){
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();

				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memom+indexm), hori.min1[0]) != HAL_OK){}
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexm +=2;
			}
			else
				indexm = 0x0;
		}
		void leitura_minutos(void){
			if(!flagmap1){
				for(uint16_t horit=0x00; horit!=0xFFFF; indexm +=2){
					FLASH_le_16bits(memom+indexm, &horit);
				}
				if(indexm>0x1FF)indexm = 0;
				if(indexm==2)indexm-=2;
				else indexm-=4; 						//VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
				flagmap1 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
			}
			FLASH_le_16bits(memom+indexm, &hori.min1[0]);
			//FLASH_le_16bits(memom+(indexm-2), &horit1);
			if(hori.min1[0] ==0xFFFF)hori.min1[0]=0x00;
		}
		/*============================================================================================================================*/
		void grava_timer(void){					//---MEIA PAGINA---//
			if(indext<0x400){
				//FLASH_escreve_32bits (memot+indext, hori.temp1);
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();

				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memoh+indext), hori.temp1) != HAL_OK){
					// ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
				}
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexh +=4;
			}
			else
				indext = 0x200;
		}
		/*============================================================================================================================*/
		/*void grava_timer(void){						//---PAGINA INTEIRA---//
	if(indext == 0x0){
		FLASH_apaga(memot, 1); 					//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
	}
	if(indext < 0x400){
		//FLASH_escreve_32bits (memot+indext, hori.temp1);
		HAL_FLASH_Unlock();
		HAL_FLASH_OB_Unlock();

		if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memot+indext), hori.temp1) != HAL_OK){
			// ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
		}
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
		indexh +=4;
	}
	else
		indext = 0x0;
}*/
		/*============================================================================================================================*/
		void leitura_timer(void){			//---MEIA PAGINA---//
			if(!flagmap2){
				for(uint32_t horit=0x00; horit!=0xFFFFFFFF; indext +=4){
					FLASH_le_32bits(memoh+indext, &horit);
				}
				if(indext>=0x400)indext = 0;
				if(indext==4)indext-=4;
				else indext-=8; 						//VOLTAMOS 8 POSIÇOES PARA 32BITS POR CONTA DO LAÇO FOR
				flagmap2 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
			}
			FLASH_le_32bits(memoh+indext, &hori.temp1);
		}
		/*============================================================================================================================*/
		/*void leitura_timer(void){		//---PAGINA INTEIRA---//
	if(!flagmap2){
		for(uint32_t horit=0x00; horit!=0xFFFFFFFF; indext +=4){
			FLASH_le_32bits(memot+indext, &horit);
		}
		if(indext==4)indext-=4;
		else indext-=8; 						//VOLTAMOS 8 POSIÇOES PARA 32BITS POR CONTA DO LAÇO FOR
		flagmap2 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
	}
	FLASH_le_32bits(memot+indext, &hori.temp1);
}*/
		/*============================================================================================================================*/
		void grava_minutos_timer(void){ 				//ESSA FUNÇÃO DE GRAVAÇÃO NAO APAGA A MEMORIA, APENAS A FUNÇÃO DE GRAVAÇAO PRINCIPAL DE MINUTOS
			if(indexm<0x400){
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();

				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memom+indexmt), hori.min1[1]) != HAL_OK){}
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexmt +=2;
			}
			else
				indexmt = 0x200;
		}
		void leitura_minutos_timer(void){
			if(!flagmap3){
				for(uint16_t horit=0x00; horit!=0xFFFF; indexmt +=2){
					FLASH_le_16bits(memom+indexmt, &horit);
				}
				if(indexmt==2)indexmt-=2;
				else indexmt-=4; 						//VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
				flagmap3 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
			}
			FLASH_le_16bits(memom+indexmt, &hori.min1[1]);
			//FLASH_le_16bits(memom+(indexm-2), &horit1);
			if(hori.min1[1] ==0xFFFF)hori.min1[1]=0x00;
		}
		/*============================================================================================================================*/
		void grava_man_barra(void){ //*MEIA PAGINA*/
			if(indexman == 0x0){
				FLASH_apaga(memoman, 1); //A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
			}
			if(indexman<0x1FF){
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();

				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoman+indexman), (aux.manaux[0])) != HAL_OK){}
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexman +=2;
			}
			else
				indexman = 0x0;
		}
		/*============================================================================================================================*/
		/*void grava_man_barra(void){ 			//---MEIA PAGINA---//
	if(indexman == 0x0){
		FLASH_apaga(memoman, 1); //A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
	}
	if(indexman<0x400){
		HAL_FLASH_Unlock();
		HAL_FLASH_OB_Unlock();

		if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoman+indexman), (aux.manaux[0])) != HAL_OK){}
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
		indexman +=2;
	}
	else
		indexman = 0x0;
}*/
		/*============================================================================================================================*/
		void leitura_man_barra(void){
			if(!flagman){
				for(uint16_t horit=0x00; horit!=0xFFFF; indexman +=2){
					horit = *(uint32_t*)(memoman+indexman);
				}
				if(indexman>0x1FF)indexman = 0;
				if(indexman==2)indexman-=2;
				else indexman-=4; //VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
				flagman = 1;//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
			}
			aux.manaux[0] = *(uint32_t*)(memoman+indexman);
			if(aux.manaux[0] ==0xFFFF)aux.manaux[0]=0x00;
		}
		/*============================================================================================================================*/
		void grava_man_bomba(void){			//---MEIA PAGINA---//
			if(indexman1>=0x200 && indexman1<0x400){
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();

				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoman+indexman1), (aux.manaux[1])) != HAL_OK){}
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexman1 +=2;
			}
			else
				indexman1 = 0x200;
		}
		/*============================================================================================================================*/
		/*void grava_man_bomba(void){				//---PAGINA INTEIRA---//
	if(indexman1 == 0x0){
		FLASH_apaga(memoman1, 1); //A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
	}
	if(indexman1<0x400){
		HAL_FLASH_Unlock();
		HAL_FLASH_OB_Unlock();

		if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoman1+indexman1), (aux.manaux[1])) != HAL_OK){}
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
		indexman1 +=2;
	}
	else
		indexman1 = 0x0;
}*/
		/*============================================================================================================================*/
		void leitura_man_bomba(void){			//---MEIA PAGINA---//
			if(indexman1>=0x200 && indexman1<=0x400){
				if(!flagman1){
					for(uint16_t horit=0x00; horit!=0xFFFF; indexman1 +=2){
						horit = *(uint32_t*)(memoman+indexman1);
					}
					if(indexman1>0x400)indexman1 = 0;
					if(indexman1==2)indexman1-=2;
					else indexman1-=4; //VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
					flagman1 = 1;//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
				}
				aux.manaux[1] = *(uint32_t*)(memoman+indexman1);
				if(aux.manaux[1] ==0xFFFF)aux.manaux[1]=0x00;
			}
		}
		/*============================================================================================================================*/
		/*void leitura_man_bomba(void){			//---PAGINA INTEIRA--//
	if(!flagman1){
		for(uint16_t horit=0x00; horit!=0xFFFF; indexman1 +=2){
			horit = *(uint32_t*)(memoman1+indexman1);
		}
		if(indexman1==2)indexman1-=2;
		else indexman1-=4; //VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
		flagman1 = 1;//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
	}
	aux.manaux[1] = *(uint32_t*)(memoman1+indexman1);
	if(aux.manaux[1] ==0xFFFF)aux.manaux[1]=0x00;
}*/
		/*============================================================================================================================*/
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
				for(uint16_t aux=0x00; aux!=0xFFFF; indexbr+=2){
					aux = *(uint32_t*)(memobr+indexbr);
				}
				if(indexbr==2)indexbr-=2;
				else {
					indexbr-=2;
					aux.cont[0] = *(uint32_t*)(memobr+(indexbr-2));
				}
				/*=============================================================================================================================*/
				if(indexbr < 0x1FF){				//---MEIA PAGINA--//
					HAL_FLASH_Unlock();
					HAL_FLASH_OB_Unlock();

					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memobr+indexbr), aux.horas[0]) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=4;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), dia) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), mes) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), ano) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					aux.cont[0]+=1;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), aux.cont[0]) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					HAL_FLASH_OB_Lock();
					HAL_FLASH_Lock();

				}
				else{
					FLASH_apaga(memobr, 1); 		//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
					indexbr = 0x0;
					HAL_FLASH_Unlock();
					HAL_FLASH_OB_Unlock();

					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memobr+indexbr), aux.horas[0]) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=4;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), dia) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), mes) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), ano) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), aux.cont[0]) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexbr+=2;
					HAL_FLASH_OB_Lock();
					HAL_FLASH_Lock();
				}
				/*=============================================================================================================================*/
				/*		if(indexbr < 0x400){		//---PAGINA INTEIRA---//
			HAL_FLASH_Unlock();
			HAL_FLASH_OB_Unlock();

			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memobr+indexbr), aux.horas[0]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=4;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), dia) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), mes) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), ano) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			aux.cont[0]+=1;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), aux.cont[0]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();

		}
		else{
			FLASH_apaga(memobr, 1); 		//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
			indexbr = 0x0;
			HAL_FLASH_Unlock();
			HAL_FLASH_OB_Unlock();

			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memobr+indexbr), aux.horas[0]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=4;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), dia) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), mes) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), ano) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexbr), aux.cont[0]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexbr+=2;
			HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();
		}*/
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
				if(indexba>=0x200 && indexba<0x400){
					for(uint16_t aux=0x00; aux!=0xFFFF; indexba+=2){
						aux = *(uint32_t*)(memobr+indexba);
					}
					if(indexba==2)indexba-=2;
					else {
						indexba-=2;
						aux.cont[1] = *(uint32_t*)(memobr+(indexba-2));
					}
				}
				else indexba+=2; //INCREMENTA ATÉ ACHAR O MEIODA MEMORIA
				/*=============================================================================================================================*/
				if(indexba>=0x200 && indexba<0x400){	//---MEIA PAGINA---//
					HAL_FLASH_Unlock();
					HAL_FLASH_OB_Unlock();

					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memobr+indexba), aux.horas[1]) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexba+=4;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexba), dia) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexba+=2;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexba), mes) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexba+=2;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexba), ano) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexba+=2;
					aux.cont[1]+=1;
					if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memobr+indexba), aux.cont[1]) != HAL_OK){
						//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
					}
					indexba+=2;
					HAL_FLASH_OB_Lock();
					HAL_FLASH_Lock();

				}
				else{
					indexba = 0x200;
				}
				/*=============================================================================================================================*/
				/*
		 for(uint16_t aux=0x00; aux!=0xFFFF; indexba+=2){
			aux = *(uint32_t*)(memoba+indexba);
		}
		if(indexba==2)indexba-=2;
		else {
			indexba-=2;
			aux.cont[1] = *(uint32_t*)(memoba+(indexba-2));
		}
		 if(indexba < 0x400){			//---PAGINA INTEIRA---//
			HAL_FLASH_Unlock();
			HAL_FLASH_OB_Unlock();

			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memoba+indexba), aux.horas[1]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=4;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), dia) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), mes) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), ano) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			aux.cont[1]+=1;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), aux.cont[1]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();

		}
		else{
			FLASH_apaga(memoba, 1); 		//A PAGINA É APAGADA QUANDO A MEMORIA CHEGA AO FIM
			indexba = 0x0;
			HAL_FLASH_Unlock();
			HAL_FLASH_OB_Unlock();

			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint64_t)(memoba+indexba), aux.horas[1]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=4;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), dia) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), mes) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), ano) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memoba+indexba), aux.cont[1]) != HAL_OK){
				//ROTINA CASO OCORRA ERRO NO PROCESSO DE APAGAR
			}
			indexba+=2;
			HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();
		}*/
				/*=============================================================================================================================*/
				aux.horas[1] = 0x00;
			}
			if(aux.manaux[2]>=(aux5-10) && !aux.flagman[2]){   //HORIMETRO COM DE 100 HORAS E MANUTENÇÃO NAO FEITA PREVIAMENTE
				aux.flagicone[2] = 1;					//HABILITA ICONE E AVISO MANUTENÇÃO
			}
			else if (aux.flagman[2])aux.flagicone[2] = 1;
		}
		/*=============================================================================================================================*/
		void historico_bagua(void){				//---MEIA PAGINA---//
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
				indexba=0x200;
				auxindex = 0X00;
				for(uint16_t aux=0x00; aux!=0xFFFF; indexba+=2){
					aux = *(uint32_t*)(memobr+indexba);
				}

				indexba-=2;
				auxindex += indexba;
				indexba=0x200;
				flaghistba = 1;
			}
			if(!point){
				for(uint16_t aux = 0x0; aux<=4; aux+=1){
					switch (aux){
					case 1:
						if(auxindex>=indexba+12){
							historico.hora = *(uint32_t*)(memobr+indexba);
							indexba+=4;
							historico.dia = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.mes = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.ano = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.aux = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							sprintf(man1,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
						}
						break;
					case 2:
						if(auxindex>=indexba+12){
							historico.hora = *(uint32_t*)(memobr+indexba);
							indexba+=4;
							historico.dia = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.mes = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.ano = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.aux = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							sprintf(man2,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
						}
						break;
					case 3:
						if(auxindex>=indexba+12){
							historico.hora = *(uint32_t*)(memobr+indexba);
							indexba+=4;
							historico.dia = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.mes = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.ano = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.aux = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							sprintf(man3,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
						}
						break;
					case 4:
						if(auxindex>=indexba+12){
							historico.hora = *(uint32_t*)(memobr+indexba);
							indexba+=4;
							historico.dia = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.mes = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.ano = *(uint32_t*)(memobr+indexba);
							indexba+=2;
							historico.aux = *(uint32_t*)(memobr+indexba);
							indexba+=2;
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
		/*void historico_bagua(void){				//---PAGINA INTEIRA---//
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
		for(uint16_t aux=0x00; aux!=0xFFFF; indexba+=2){
			aux = *(uint32_t*)(memoba+indexba);
		}

		indexba-=2;
		auxindex += indexba;
		indexba=0x0;
		flaghistba = 1;
	}
	if(!point){
		for(uint16_t aux = 0x0; aux<=4; aux+=1){
			switch (aux){
			case 1:
				if(auxindex>=indexba+12){
					historico.hora = *(uint32_t*)(memoba+indexba);
					indexba+=4;
					historico.dia = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.mes = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.ano = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.aux = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					sprintf(man1,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 2:
				if(auxindex>=indexba+12){
					historico.hora = *(uint32_t*)(memoba+indexba);
					indexba+=4;
					historico.dia = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.mes = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.ano = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.aux = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					sprintf(man2,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 3:
				if(auxindex>=indexba+12){
					historico.hora = *(uint32_t*)(memoba+indexba);
					indexba+=4;
					historico.dia = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.mes = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.ano = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.aux = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					sprintf(man3,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
				}
				break;
			case 4:
				if(auxindex>=indexba+12){
					historico.hora = *(uint32_t*)(memoba+indexba);
					indexba+=4;
					historico.dia = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.mes = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.ano = *(uint32_t*)(memoba+indexba);
					indexba+=2;
					historico.aux = *(uint32_t*)(memoba+indexba);
					indexba+=2;
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
}*/
		/*=============================================================================================================================*/
		void historico_barra(void){
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
				for(uint16_t aux=0x00; aux != 0xFFFF; indexbr+=2){
					aux = *(uint32_t*)(memobr+indexbr);
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
						if(auxindex1>=indexbr+12){
							historico.hora = *(uint32_t*)(memobr+indexbr);
							indexbr+=4;
							historico.dia = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.mes = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.ano = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.aux = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							sprintf(man1,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
						}
						break;
					case 2:
						if(auxindex1>=indexbr+12){
							historico.hora = *(uint32_t*)(memobr+indexbr);
							indexbr+=4;
							historico.dia = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.mes = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.ano = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.aux = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							sprintf(man2,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
						}
						break;
					case 3:
						if(auxindex1>=indexbr+12){
							historico.hora = *(uint32_t*)(memobr+indexbr);
							indexbr+=4;
							historico.dia = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.mes = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.ano = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.aux = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							sprintf(man3,"%02d %05dhr %02d/%02d/%02d", historico.aux, historico.hora, historico.dia, historico.mes, historico.ano);
						}
						break;
					case 4:
						if(auxindex1>=indexbr+12){
							historico.hora = *(uint32_t*)(memobr+indexbr);
							indexbr+=4;
							historico.dia = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.mes = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.ano = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
							historico.aux = *(uint32_t*)(memobr+indexbr);
							indexbr+=2;
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
		void comuart(void){

			float media = 0x00;
			/**********************************************************************************/
			/*                                CODIGO RECEPTOR                                 */
			/**********************************************************************************/
			if(!flagTx){
				leitura_chaves_hidraulico();
				if(HAL_UART_Transmit_IT(&huart1, serial_tx_buff, sizeof(serial_tx_buff)) == HAL_OK){
					flagTx=1;
					contcom++;
				}
			}
			if(flagTx){
				if(HAL_UART_Receive_DMA(&huart1, rx_buff, sizeof(rx_buff)) == HAL_OK){
					flagRx=1;
				}
			}
			if(flagRx){
				if(rx_buff[0]=='('){
					contcom = 0;

					altura_sensor = 0;
					altura_sensor = rx_buff[20] - 0x30;
					altura_sensor *= 10;
					altura_sensor += rx_buff[21] - 0x30;
					altura_sensor *= 10;
					altura_sensor += rx_buff[22] - 0x30;
					altura_sensor *= 10;
					altura_sensor += rx_buff[24] - 0x30;

					//altura_sensor -= ALTURA_INSTALACAO_SENSOR;

					battdec = 0;
					battdec = (rx_buff[15] - 0x30);
					battdec*=10;
					battdec += (rx_buff[16] - 0x30);
					battdec*=10;
					battdec += (rx_buff[18] - 0x30);

					batt = (float)battdec/10.0;

					/*nserie[0] = rx_buff[1];
			nserie[1] = rx_buff[2];
			nserie[2] = rx_buff[3];
			nserie[3] = rx_buff[4];
			nserie[4] = rx_buff[5];
			nserie[5] = rx_buff[6];
			nserie[6] = '\0';

			vfw[0] = rx_buff[8];
			vfw[1] = rx_buff[9];
			vfw[2] = rx_buff[10];
			vfw[3] = rx_buff[11];
			vfw[4] = rx_buff[12];
			vfw[5] = rx_buff[13];
			vfw[6] = '\0';*/

					if(auxuart1<10)auxuart1++;
					else{
						tensao[0] = rx_buff[15];
						tensao[1] = rx_buff[16];
						tensao[2] = '.';
						tensao[3] = rx_buff[18];
						tensao[4] = 'V';
						tensao[5] = '\0';
						pressao1[0] = rx_buff[27];
						pressao1[1] = rx_buff[28];
						pressao1[2] = '.';
						pressao1[3] = rx_buff[30];
						pressao1[4] = rx_buff[31];
						pressao1[5] = '\0';
						auxuart1 = 0;
					}

					pressdec = 0;
					pressdec = (rx_buff[27] - 0x30);
					pressdec *= 10;
					pressdec += (rx_buff[28] - 0x30);
					pressdec *= 10;
					pressdec += (rx_buff[30] - 0x30);
					pressdec *= 10;
					pressdec += (rx_buff[31] - 0x30);

					press[contpress] = (float)pressdec / 100.0;
					contpress++;
					if(contpress>=10){
						media = (press[0]+press[1]+press[2]+press[3]+press[4]+press[5]+press[6]+press[7]+press[8]+press[9])/10;
						if(media<=0.1){
							pressok=0;
						}
						else pressok=1;
						contpress=0;
					}

					flagRx = 0;

					flagTx = 0;
				}
				else{
					flagRx = 0;
					flagTx = 0;
					memset(rx_buff, 0, sizeof(rx_buff));
					memset(pressao1, 0, sizeof(pressao1));
				}
			}
			if(contcom>=10){
				memset(rx_buff, 0, sizeof(rx_buff));
				memset(pressao1, 0, sizeof(pressao1));
				memset(tensao, 0, sizeof(tensao));
				pressok = 0;
				media = 0;
				batt = 0;
				altura_sensor = 0;
			}
			contuart=0;
		}

		void leitura_chaves_hidraulico(void)
		{
			hidraulico &= ~(1 << 0);
			hidraulico &= ~(1 << 1);

			if(!HAL_GPIO_ReadPin(HIDR_1_UP_GPIO_Port, HIDR_1_UP_Pin))
			{
				hidraulico |= (1 << 0);
			}
			else
			{
				hidraulico &= ~(1 << 0);
			}
			if(!HAL_GPIO_ReadPin(HIDR_1_DOWN_GPIO_Port, HIDR_1_DOWN_Pin))
			{
				hidraulico |= (1 << 1);
			}
			else
			{
				hidraulico &= ~(1 << 1);
			}

			hidraulico &= ~(1 << 2);
			hidraulico &= ~(1 << 3);

			if(!HAL_GPIO_ReadPin(HIDR_2_UP_GPIO_Port, HIDR_2_UP_Pin))
			{
				hidraulico |= (1 << 2);
			}
			else
			{
				hidraulico &= ~(1 << 2);
			}
			if(!HAL_GPIO_ReadPin(HIDR_2_DOWN_GPIO_Port, HIDR_2_DOWN_Pin))
			{
				hidraulico |= (1 << 3);
			}
			else
			{
				hidraulico &= ~(1 << 3);
			}

			hidraulico &= ~(1 << 4);
			hidraulico &= ~(1 << 5);

			if(!HAL_GPIO_ReadPin(HIDR_3_UP_GPIO_Port, HIDR_3_UP_Pin))
			{
				hidraulico |= (1 << 4);
			}
			else
			{
				hidraulico &= ~(1 << 4);
			}
			if(!HAL_GPIO_ReadPin(HIDR_3_DOWN_GPIO_Port, HIDR_3_DOWN_Pin))
			{
				hidraulico |= (1 << 5);
			}
			else
			{
				hidraulico &= ~(1 << 5);
			}

			if(!HAL_GPIO_ReadPin(HIDR_4_UP_GPIO_Port, HIDR_4_UP_Pin))
			{
				hidraulico |= (1 << 6);
			}
			else
			{
				hidraulico &= ~(1 << 6);
			}
			if(!HAL_GPIO_ReadPin(HIDR_4_DOWN_GPIO_Port, HIDR_4_DOWN_Pin))
			{
				hidraulico |= (1 << 7);
			}
			else
			{
				hidraulico &= ~(1 << 7);
			}

			if(!HAL_GPIO_ReadPin(HIDR_5_UP_GPIO_Port, HIDR_5_UP_Pin))
			{
				hidraulico |= (1 << 8);
			}
			else
			{
				hidraulico &= ~(1 << 8);
			}
			if(!HAL_GPIO_ReadPin(HIDR_5_DOWN_GPIO_Port, HIDR_5_DOWN_Pin))
			{
				hidraulico |= (1 << 9);
			}
			else
			{
				hidraulico &= ~(1 << 9);
			}

			if(!HAL_GPIO_ReadPin(HIDR_6_UP_GPIO_Port, HIDR_6_UP_Pin))
			{
				hidraulico |= (1 << 10);
			}
			else
			{
				hidraulico &= ~(1 << 10);
			}
			if(!HAL_GPIO_ReadPin(HIDR_6_DOWN_GPIO_Port, HIDR_6_DOWN_Pin))
			{
				hidraulico |= (1 << 11);
			}
			else
			{
				hidraulico &= ~(1 << 11);
			}

			if(!HAL_GPIO_ReadPin(HIDR_7_UP_GPIO_Port, HIDR_7_UP_Pin))
			{
				hidraulico |= (1 << 12);
			}
			else
			{
				hidraulico &= ~(1 << 12);
			}
			if(!HAL_GPIO_ReadPin(HIDR_7_DOWN_GPIO_Port, HIDR_7_DOWN_Pin))
			{
				hidraulico |= (1 << 13);
			}
			else
			{
				hidraulico &= ~(1 << 13);
			}

			serial_tx_buff[8] = (hidraulico >> 12) & 0x0F;
			if(serial_tx_buff[8] >= 0 && serial_tx_buff[8] <= 9)
				serial_tx_buff[8] += 0x30;
			else if(serial_tx_buff[8] >= 0x0A && serial_tx_buff[8] <= 0x0F)
				serial_tx_buff[8] += 0x37;
			else
				serial_tx_buff[8] = 'Y';

			serial_tx_buff[9] = (hidraulico >> 8) & 0x0F;
			if(serial_tx_buff[9] >= 0 && serial_tx_buff[9] <= 9)
				serial_tx_buff[9] += 0x30;
			else if(serial_tx_buff[9] >= 0x0A && serial_tx_buff[9] <= 0x0F)
				serial_tx_buff[9] += 0x37;
			else
				serial_tx_buff[9] = 'Y';

			serial_tx_buff[10] = (hidraulico >> 4) & 0x0F;
			if(serial_tx_buff[10] >= 0 && serial_tx_buff[10] <= 9)
				serial_tx_buff[10] += 0x30;
			else if(serial_tx_buff[10] >= 0x0A && serial_tx_buff[10] <= 0x0F)
				serial_tx_buff[10] += 0x37;
			else
				serial_tx_buff[10] = 'Y';

			serial_tx_buff[11] = hidraulico & 0x0F;
			if(serial_tx_buff[11] >= 0 && serial_tx_buff[11] <= 9)
				serial_tx_buff[11] += 0x30;
			else if(serial_tx_buff[11] >= 0x0A && serial_tx_buff[11] <= 0x0F)
				serial_tx_buff[11] += 0x37;
			else
				serial_tx_buff[11] = 'Y';
		}
		void beep(void){
			if(flagbeep==1){
				HAL_GPIO_WritePin(BZ1_GPIO_Port, BZ1_Pin, GPIO_PIN_SET);
				if(conttg[4]>=4){
					HAL_GPIO_WritePin(BZ1_GPIO_Port, BZ1_Pin, GPIO_PIN_RESET);
					conttg[4] = 0;
				}
			}
		}
		void set_time(void) {
			sTime.Hours = (hr & 0xFF);
			sTime.Minutes = (min & 0xFF);
			sTime.Seconds = (0 & 0xFF);
			if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK);
		}
		void set_date(void){
			//sDate.WeekDay = RTC_WEEKDAY_THURSDAY; // day
			sDate.Date = (dia & 0xFF);
			sDate.Month = (mes & 0xFF);
			sDate.Year = (ano & 0xFF);
			sDate.WeekDay = (diasem & 0xFF);
			if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK);
		}
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
				verifica_manutencao();
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
				grava_horimetro();
				grava_minutos();
				grava_man_barra();
				grava_man_bomba();
				grava_limpeza();
				hori.aux2[1]=0;
				if(flagh[0]){
					grava_minutos_timer();
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
		void grava_limpeza(void){
			if(indexinter1 == 0x0){
				/*Le memoria*/
				indexinter1 = 0x00;
				inter.intervalo_man[0] = *(uint32_t*)(memointer+indexinter1);
				indexinter1+=2;
				inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter1);
				indexinter1+=2;
				inter.limpeza[0] = *(uint32_t*)(memointer+indexinter);
				indexinter1+=2;
				inter.temp_oci[0] = *(uint32_t*)(memointer+indexinter);
				indexinter1+=2;
				inter.primeira_man[0] = *(uint32_t*)(memointer+indexinter1);
				indexinter1+=2;
				inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter1);
				indexinter1+=2;
				flagh[0] = *(uint32_t*)(memointer+indexinter1);
				indexinter1+=2;
				flagh[2] = *(uint32_t*)(memointer+indexinter1);
				indexinter1+=2;
				sttbeep = *(uint32_t*)(memointer+indexinter1);
				indexinter1+=2;
				tanque = *(uint32_t*)(memointer+indexinter1);
				indexinter1=0;
				/*Apaga memoria*/
				FLASH_apaga(memointer, 1);
				/*Regrava memoria*/
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();
				indexinter1 = 0x00;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (inter.intervalo_man[0])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (inter.intervalo_man[1])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (inter.limpeza[0])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (inter.temp_oci[0])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (inter.primeira_man[0])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (inter.primeira_man[1])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (flagh[0])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (flagh[2])) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (sttbeep)) != HAL_OK){}
				indexinter1+=2;
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), (tanque)) != HAL_OK){}
				indexinter1+=2;
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexinter1 = 0x200; 						//INICIAMOS A OUTRA MEDADE DA MEMORIA
			}
			if(indexinter1<0x400){
				HAL_FLASH_Unlock();
				HAL_FLASH_OB_Unlock();
				if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter1), aux.manaux[2]) != HAL_OK){}
				HAL_FLASH_OB_Lock();
				HAL_FLASH_Lock();
				indexinter1 +=2;
			}
			else
				indexinter1 = 0x0;
		}
		void le_limpeza(void){
			indexinter1 = 0x200;
			if(indexinter1>=0x200 && indexinter1<=0x400){
				if(!flagmap5){
					for(uint16_t horit=0x00; horit!=0xFFFF; indexinter1 +=2){
						horit = *(uint32_t*)(memointer+indexinter1);
					}
					if(indexinter1>0x400)indexinter1 = 0;
					if(indexinter1==2)indexinter1-=2;
					else indexinter1-=4; //VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
					flagmap5 = 1;//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
				}
				aux.manaux[2] = *(uint32_t*)(memointer+indexinter1);
				if(aux.manaux[2] ==0xFFFF)aux.manaux[2]=0x00;
			}

		}
		/*FUNÇÃO LEITURA MEMORIA FLAGS*/
		void leitura_inter(void){
			/*Le memoria*/
			indexinter = 0x00;
			inter.intervalo_man[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.limpeza[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.temp_oci[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.primeira_man[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			flagh[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			flagh[2] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			sttbeep = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			tanque = *(uint32_t*)(memointer+indexinter);
			indexinter=0;

			inter.temp_oci[1]=inter.temp_oci[0];

			if(inter.intervalo_man[0]==0xFFFF)inter.intervalo_man[0]=0x00;
			if(inter.intervalo_man[1]==0xFFFF)inter.intervalo_man[1]=0x00;
			if(inter.primeira_man[0]==0xFFFF)inter.primeira_man[0]=0x00;
			if(inter.primeira_man[1]==0xFFFF)inter.primeira_man[1]=0x00;
			if(inter.limpeza[0]==0xFF)inter.limpeza[0]=0x00;
			if(inter.temp_oci[0]==0xFF)inter.temp_oci[0]=0x00;
			if(flagh[0]==0xFF)flagh[0]=0x00;
			if(flagh[2]==0xFF)flagh[2]=0x00;
			if(sttbeep==0xFF)sttbeep=0x01;
			if(tanque==0xFF)tanque=0x01;
			flagbeep = sttbeep;
			flagnvl = tanque;


			if(!flagmap4){
				indexinter1 = 0x200;
				for(uint16_t horit=0x00; horit!=0xFFFF; indexinter1 +=2){
					FLASH_le_16bits(memointer+indexinter1, &horit);
				}
				if(indexinter1==2)indexinter1-=2;
				else indexinter1-=4; 						//VOLTAMOS 4 POSIÇOES PARA 16BITS POR CONTA DO LAÇO FOR
				flagmap4 = 1;							//ASSIM VOLTAMOS AO INICIO DA ULTIMA POSIÇÃO QUE FOI GRAVADA!
			}
			FLASH_le_16bits(memointer+indexinter1, &aux.manaux[2]); //LIMPEZA SENSOR?
			if(aux.manaux[2]==0xFFFF)aux.manaux[2]=0x00;
		}
		void grava_flagt(void){
			/*Le memoria*/
			indexinter = 0x00;
			inter.intervalo_man[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.intervalo_man[1] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.limpeza[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.temp_oci[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.primeira_man[0] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			inter.primeira_man[1] = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			aux1 = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			aux2 = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			sttbeep = *(uint32_t*)(memointer+indexinter);
			indexinter+=2;
			tanque = *(uint32_t*)(memointer+indexinter);
			indexinter=0x00;
			/*Apaga memoria*/
			FLASH_apaga(memointer, 1);
			/*Regrava memoria*/
			HAL_FLASH_Unlock();
			HAL_FLASH_OB_Unlock();
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[0])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.intervalo_man[1])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.limpeza[0])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.temp_oci[0])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[0])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (inter.primeira_man[1])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[0])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (flagh[2])) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (sttbeep)) != HAL_OK){}
			indexinter+=2;
			if( HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,(uint64_t)(memointer+indexinter), (tanque)) != HAL_OK){}
			indexinter+=2;
			HAL_FLASH_OB_Lock();
			HAL_FLASH_Lock();
		}
		void menuprincipal(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4){
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_12);
			OLED_DrawStr("Config calend.      ", 1, 0, n1);
			OLED_DrawStr("Temporizador.     ", 1, 16, n2);
			OLED_DrawStr("Manutencoes.      ", 1, 33, n3);
			OLED_DrawStr("Preferencias.        ", 1, 49, n4);
			OLED_UpdateScreen();
		}
		void menutimer(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4){
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_12);
			OLED_DrawStr("Inicia   temp.      ", 1, 0, n1);
			OLED_DrawStr("Pausa  temp.        ", 1, 16, n2);
			OLED_DrawStr("Reinicia temp.      ", 1, 33, n3);
			OLED_DrawStr("Desabilita temp.    ", 1, 49, n4);
			OLED_UpdateScreen();
		}
		void registraman(uint8_t n1, uint8_t n2, uint8_t n3){
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_12);
			OLED_DrawStr("Man bomba d'agua.", 1, 1, n1);
			OLED_DrawStr("Man barra.       ", 1, 16, n2);
			OLED_DrawStr("Limpeza sensores  ", 1, 33, n3);
			OLED_UpdateScreen();
		}
		void preferencias(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4){
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_12);
			OLED_DrawStr("Beep teclas.        ", 1, 0, n1);
			OLED_DrawStr("Alarme pressao.      ", 1, 16, n2);
			OLED_DrawStr("Conf. fabricante.   ", 1, 33, n3);
			OLED_DrawStr("Sobre.              ", 1, 49, n4);
			OLED_UpdateScreen();
		}
		void conffabricante(uint8_t n1, uint8_t n2, uint8_t n3, uint8_t n4){
			FontSet(Segoe_UI_Eng_12);
			OLED_DrawStr("Reset horimetro.    ", 1, 1, n1);
			OLED_DrawStr("Intervalo man.     ", 1, 16, n2);
			OLED_DrawStr("Tempo ocioso.     ", 1, 33, n3);
			OLED_DrawStr("Modelo Tanque      ", 1, 49, n4);
			OLED_UpdateScreen();
		}
		void intervaloman(uint8_t n1, uint8_t n2, uint8_t n3){
			OLED_Clear(0);
			FontSet(Segoe_UI_Eng_12);
			OLED_DrawStr("Intervalo bomba.     ", 1, 1, n1);
			OLED_DrawStr("Intervalo barra.     ", 1, 16, n2);
			OLED_DrawStr("Intervalo limpeza.     ", 1, 33, n3);
			OLED_UpdateScreen();
		}
		/* USER CODE END 0 */

		/**
		 * @brief  The application entry point.
		 * @retval int
		 */
		int main(void)
		{
			/* USER CODE BEGIN 1 */

			/* USER CODE END 1 */

			/* MCU Configuration--------------------------------------------------------*/

			/* Reset of all peripherals, Initializes the Flash interface and the Systick. */

			HAL_Init();

			/* USER CODE BEGIN Init */

			/* USER CODE END Init */

			/* Configure the system clock */
			SystemClock_Config();

			/* USER CODE BEGIN SysInit */

			/* USER CODE END SysInit */

			/* Initialize all configured peripherals */
			MX_GPIO_Init();
			MX_DMA_Init();
			MX_I2C1_Init();
			MX_RTC_Init();
			MX_TIM3_Init();
			MX_USART1_UART_Init();
			/* USER CODE BEGIN 2 */
			//HAL_TIM_Base_Start(&htim3); //INICIA TIMER3 SEM INTERRUPÇÃO NO OVERFLOW
			HAL_TIM_Base_Start_IT(&htim3); //INCIA TIMER3 COM INTERRUPÇÃO NO OVERFLOW
			init_flags();
			OLED_Init(&hi2c1); //INICIANDO DISPLAY
			OLED_Clear(0);
			OLED_UpdateScreen();
			leitura_horimetro();
			if(hori.hr1==0xFFFFFFFF)hori.hr1=0x00;
			leitura_timer();
			if(hori.temp1==0xFFFFFFFF)hori.temp1=0x00;
			leitura_minutos();
			if(hori.min1[0]==0xFFFF)hori.min1[0]=0x00;
			leitura_minutos_timer();
			if(hori.min1[1]==0xFFFF)hori.min1[1]=0x00;
			leitura_man_barra();
			leitura_man_bomba();
			leitura_inter();
			le_limpeza();
			initdisp(); //IMPRIMINDO O BITMAP
			flag.menu = 0x00;
			OLED_Clear(0);
			OLED_UpdateScreen();
			/* USER CODE END 2 */

			/* Infinite loop */
			/* USER CODE BEGIN WHILE */
			while (1)
			{
				atualiza_menu();
				if(!init)HAL_Delay(2000);
				init=1;
			}
			/* USER CODE END WHILE */

			/* USER CODE BEGIN 3 */

			/* USER CODE END 3 */
		}

		/**
		 * @brief System Clock Configuration
		 * @retval None
		 */
		void SystemClock_Config(void)
		{
			RCC_OscInitTypeDef RCC_OscInitStruct = {0};
			RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

			/** Configure the main internal regulator output voltage
			 */
			__HAL_RCC_PWR_CLK_ENABLE();
			__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

			/** Initializes the RCC Oscillators according to the specified parameters
			 * in the RCC_OscInitTypeDef structure.
			 */
			RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
			RCC_OscInitStruct.LSEState = RCC_LSE_ON;
			RCC_OscInitStruct.HSIState = RCC_HSI_ON;
			RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
			RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
			RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
			RCC_OscInitStruct.PLL.PLLM = 8;
			RCC_OscInitStruct.PLL.PLLN = 84;
			RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
			RCC_OscInitStruct.PLL.PLLQ = 4;
			if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
			{
				Error_Handler();
			}

			/** Initializes the CPU, AHB and APB buses clocks
			 */
			RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
					|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
			RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
			RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
			RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

			if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
			{
				Error_Handler();
			}
		}

		/**
		 * @brief I2C1 Initialization Function
		 * @param None
		 * @retval None
		 */
		static void MX_I2C1_Init(void)
		{

			/* USER CODE BEGIN I2C1_Init 0 */

			/* USER CODE END I2C1_Init 0 */

			/* USER CODE BEGIN I2C1_Init 1 */

			/* USER CODE END I2C1_Init 1 */
			hi2c1.Instance = I2C1;
			hi2c1.Init.ClockSpeed = 400000;
			hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
			hi2c1.Init.OwnAddress1 = 0;
			hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
			hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
			hi2c1.Init.OwnAddress2 = 0;
			hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
			hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
			if (HAL_I2C_Init(&hi2c1) != HAL_OK)
			{
				Error_Handler();
			}
			/* USER CODE BEGIN I2C1_Init 2 */

			/* USER CODE END I2C1_Init 2 */

		}

		/**
		 * @brief RTC Initialization Function
		 * @param None
		 * @retval None
		 */
		static void MX_RTC_Init(void)
		{

			/* USER CODE BEGIN RTC_Init 0 */

			/* USER CODE END RTC_Init 0 */

			/* USER CODE BEGIN RTC_Init 1 */

			/* USER CODE END RTC_Init 1 */

			/** Initialize RTC Only
			 */
			hrtc.Instance = RTC;
			hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
			hrtc.Init.AsynchPrediv = 127;
			hrtc.Init.SynchPrediv = 255;
			hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
			hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
			hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
			if (HAL_RTC_Init(&hrtc) != HAL_OK)
			{
				Error_Handler();
			}
			/* USER CODE BEGIN RTC_Init 2 */
			HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
			return;
			/* USER CODE END RTC_Init 2 */

		}

		/**
		 * @brief TIM3 Initialization Function
		 * @param None
		 * @retval None
		 */
		static void MX_TIM3_Init(void)
		{

			/* USER CODE BEGIN TIM3_Init 0 */

			/* USER CODE END TIM3_Init 0 */

			TIM_ClockConfigTypeDef sClockSourceConfig = {0};
			TIM_MasterConfigTypeDef sMasterConfig = {0};

			/* USER CODE BEGIN TIM3_Init 1 */

			/* USER CODE END TIM3_Init 1 */
			htim3.Instance = TIM3;
			htim3.Init.Prescaler = 249;
			htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
			htim3.Init.Period = 336;
			htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
			htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
			if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
			{
				Error_Handler();
			}
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
			{
				Error_Handler();
			}
			sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
			sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
			if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
			{
				Error_Handler();
			}
			/* USER CODE BEGIN TIM3_Init 2 */

			/* USER CODE END TIM3_Init 2 */

		}

		/**
		 * @brief USART1 Initialization Function
		 * @param None
		 * @retval None
		 */
		static void MX_USART1_UART_Init(void)
		{

			/* USER CODE BEGIN USART1_Init 0 */

			/* USER CODE END USART1_Init 0 */

			/* USER CODE BEGIN USART1_Init 1 */

			/* USER CODE END USART1_Init 1 */
			huart1.Instance = USART1;
			huart1.Init.BaudRate = 9600;
			huart1.Init.WordLength = UART_WORDLENGTH_8B;
			huart1.Init.StopBits = UART_STOPBITS_1;
			huart1.Init.Parity = UART_PARITY_NONE;
			huart1.Init.Mode = UART_MODE_TX_RX;
			huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			huart1.Init.OverSampling = UART_OVERSAMPLING_16;
			if (HAL_UART_Init(&huart1) != HAL_OK)
			{
				Error_Handler();
			}
			/* USER CODE BEGIN USART1_Init 2 */

			/* USER CODE END USART1_Init 2 */

		}

		/**
		 * Enable DMA controller clock
		 */
		static void MX_DMA_Init(void)
		{

			/* DMA controller clock enable */
			__HAL_RCC_DMA2_CLK_ENABLE();

			/* DMA interrupt init */
			/* DMA2_Stream2_IRQn interrupt configuration */
			HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

		}

		/**
		 * @brief GPIO Initialization Function
		 * @param None
		 * @retval None
		 */
		static void MX_GPIO_Init(void)
		{
			GPIO_InitTypeDef GPIO_InitStruct = {0};
			/* USER CODE BEGIN MX_GPIO_Init_1 */
			/* USER CODE END MX_GPIO_Init_1 */

			/* GPIO Ports Clock Enable */
			__HAL_RCC_GPIOC_CLK_ENABLE();
			__HAL_RCC_GPIOH_CLK_ENABLE();
			__HAL_RCC_GPIOA_CLK_ENABLE();
			__HAL_RCC_GPIOB_CLK_ENABLE();

			/*Configure GPIO pin Output Level */
			HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_RESET);

			/*Configure GPIO pin Output Level */
			HAL_GPIO_WritePin(BZ1_GPIO_Port, BZ1_Pin, GPIO_PIN_RESET);

			/*Configure GPIO pins : HIDR_1_UP_Pin HIDR_1_DOWN_Pin HIDR_2_UP_Pin HIDR_2_DOWN_Pin
                           HIDR_3_UP_Pin HIDR_3_DOWN_Pin HIDR_4_UP_Pin */
			GPIO_InitStruct.Pin = HIDR_1_UP_Pin|HIDR_1_DOWN_Pin|HIDR_2_UP_Pin|HIDR_2_DOWN_Pin
					|HIDR_3_UP_Pin|HIDR_3_DOWN_Pin|HIDR_4_UP_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/*Configure GPIO pins : TCM_Pin HIDR_4_DOWN_Pin */
			GPIO_InitStruct.Pin = TCM_Pin|HIDR_4_DOWN_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/*Configure GPIO pins : TCF_Pin TBX_Pin TVL_Pin */
			GPIO_InitStruct.Pin = TCF_Pin|TBX_Pin|TVL_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

			/*Configure GPIO pins : HIDR_5_UP_Pin HIDR_5_DOWN_Pin HIDR_6_UP_Pin HIDR_6_DOWN_Pin
                           HIDR_7_UP_Pin HIDR_7_DOWN_Pin */
			GPIO_InitStruct.Pin = HIDR_5_UP_Pin|HIDR_5_DOWN_Pin|HIDR_6_UP_Pin|HIDR_6_DOWN_Pin
					|HIDR_7_UP_Pin|HIDR_7_DOWN_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

			/*Configure GPIO pin : led1_Pin */
			GPIO_InitStruct.Pin = led1_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
			HAL_GPIO_Init(led1_GPIO_Port, &GPIO_InitStruct);

			/*Configure GPIO pin : BZ1_Pin */
			GPIO_InitStruct.Pin = BZ1_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
			HAL_GPIO_Init(BZ1_GPIO_Port, &GPIO_InitStruct);

			/* USER CODE BEGIN MX_GPIO_Init_2 */
			/* USER CODE END MX_GPIO_Init_2 */
		}

		/* USER CODE BEGIN 4 */

		/* USER CODE END 4 */

		/**
		 * @brief  This function is executed in case of error occurrence.
		 * @retval None
		 */
		void Error_Handler(void)
		{
			/* USER CODE BEGIN Error_Handler_Debug */
			/* User can add his own implementation to report the HAL error return state */
			__disable_irq();
			while (1)
			{
			}
			/* USER CODE END Error_Handler_Debug */
		}

#ifdef  USE_FULL_ASSERT
		/**
		 * @brief  Reports the name of the source file and the source line number
		 *         where the assert_param error has occurred.
		 * @param  file: pointer to the source file name
		 * @param  line: assert_param error line source number
		 * @retval None
		 */
		void assert_failed(uint8_t *file, uint32_t line)
		{
			/* USER CODE BEGIN 6 */
			/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
			/* USER CODE END 6 */
		}
#endif /* USE_FULL_ASSERT */
