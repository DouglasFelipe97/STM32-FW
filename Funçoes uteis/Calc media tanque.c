/**VARIAVEIS GLOBAIS**/
uint16_t buffer_media[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint16_t calcula_media_nivel(uint16_t altura_sensor)
{
	uint32_t aux_media = 0;
	uint8_t cont_for, cont_amostras = 0;
	
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