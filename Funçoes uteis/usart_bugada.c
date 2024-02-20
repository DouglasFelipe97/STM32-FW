#include "stm32l0xx_hal.h"

// Função para escrever na "EEPROM"
void write_to_eeprom(uint32_t address, uint32_t data) {
    HAL_FLASHEx_DATAEEPROM_Unlock(); // Destrava a EEPROM
    HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAMDATA_WORD, address, data); // Programa os dados na EEPROM
    HAL_FLASHEx_DATAEEPROM_Lock(); // Trava a EEPROM novamente
}

// Função para ler da "EEPROM"
uint32_t read_from_eeprom(uint32_t address) {
    return *(__IO uint32_t*)address; // Lê os dados diretamente da memória
}

int main(void) {
    HAL_Init();
    // Configuração do sistema (clocks, etc.) é geralmente feita aqui

    // Escreve dados na "EEPROM"
    uint32_t data_to_write = 0xAABBCCDD;
    write_to_eeprom(0x08080000, data_to_write); // Substitua o endereço pelo desejado

    // Lê dados da "EEPROM"
    uint32_t data_read = read_from_eeprom(0x08080000); // Substitua o endereço pelo desejado

    // Agora, 'data_read' contém os dados lidos da "EEPROM"
    
    while (1) {
        // Seu código principal aqui
    }
}

