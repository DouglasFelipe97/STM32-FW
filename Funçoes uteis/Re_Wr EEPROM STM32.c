// Função para escrever na "EEPROM" STM32
void write_to_eeprom(uint32_t address, uint32_t data) {
    HAL_FLASHEx_DATAEEPROM_Unlock(); // Destrava a EEPROM
    HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD, address, data); // Programa os dados na EEPROM
    HAL_FLASHEx_DATAEEPROM_Lock(); // Trava a EEPROM novamente
}

// Função para ler da "EEPROM"
uint32_t read_from_eeprom(uint32_t address) {
	return *(__IO uint32_t*)address; // Lê os dados diretamente da memória
	/* (uint32_t*) é um tipo de cast que converte o endereço address em um ponteiro para um objeto do tipo uint32_t.
	 * Isso significa que estamos dizendo ao compilador que tratamos o endereço address como um ponteiro para uma variável de 32 bits sem sinal (uint32_t).
	 * '__IO' é uma macro que  é usada para indicar que a variável é uma variável de entrada/saída. Serve para garantir que a leitura ou gravação na memória seja tratada corretamente.
	 * '*': Este é o operador de desreferência. Ele acessa o valor apontado pelo ponteiro.*/
}
