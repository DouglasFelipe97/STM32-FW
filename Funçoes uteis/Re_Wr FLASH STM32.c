void write_flash(uint32_t address, uint32_t data) {
	HAL_FLASH_Unlock(); // Desbloqueia a flash para gravação
	HAL_FLASH_Program(TYPEPROGRAMDATA_WORD, address, data); // Programa os dados na flash
	HAL_FLASH_Lock(); //Bloqueia a flash contra gravaçoes indesejadas
}
uint32_t read_flash(uint32_t address){
	return *(__IO uint32_t*)address; // Lê os dados diretamente da memória
	/* (uint32_t*) é um tipo de cast que converte o endereço address em um ponteiro para um objeto do tipo uint32_t.
	 * Isso significa que estamos dizendo ao compilador que tratamos o endereço address como um ponteiro para uma variável de 32 bits sem sinal (uint32_t).
	 * '__IO' é uma macro que  é usada para indicar que a variável é uma variável de entrada/saída. Serve para garantir que a leitura ou gravação na memória seja tratada corretamente.
	 * '*': Este é o operador de desreferência. Ele acessa o valor apontado pelo ponteiro.*/
}
void erase_flash(uint32_t page_address){
	FLASH_EraseInitTypeDef eraseInitStruct;
	uint32_t PageError = 0;
	HAL_FLASH_OB_Unlock();
	HAL_FLASH_Unlock(); // Desbloqueia a memória flash

	// Configura os parâmetros de apagamento
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInitStruct.PageAddress = page_address;
	eraseInitStruct.NbPages = 1; // Quantidade de páginas a serem apagadas (neste caso, apenas uma)

	// Apaga a página
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &PageError) != HAL_OK) {
		// Ocorreu um erro ao apagar a página
		// Lidar com o erro aqui
	}
	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock(); // Bloqueia a memória flash novamente
}