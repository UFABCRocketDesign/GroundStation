//======================================================= CONFIGURATION SYSTEM =================================================
// Activate/deactivate modules of the embedded system. Each macro controls the inclusion of a functionality.

//------------------------------------------------- BOARD SELECTION -------------------------------------------------//
// Leave only ONE board active by uncommenting the corresponding line. This will ensure that the correct pinout and configurations are applied for your specific hardware setup.

// #define PLACA_MAIN_OCTA_PTH
#define PLACA_ESSENTIALS_V1
// #define PLACA_MAIN_PTH
// #define PLACA_MAIN_SMD
// #define PLACA_MAIN_IC

// ----------------------- CONFIG -----------------------

#define URD_CORE_ENABLE 1
#define URD_GROUNDSTATION_ENABLE 1
    #define PRINT_MODE 1  // Only for testing, not for flight 
    #define DEBUG_MODE 1  // Only for testing, not for flight 

#define LORA_MODE 1
    #define LORA_MANAGER (LORA_MODE && 1)
      #define LORA_MANAGER_E32 1  // Set to 1 if using the E32 LoRa module (Do not change this for now, as the E22 support is not implemented yet)
      #define LORA_MANAGER_E22 0  // Set 1 if using the E22 LoRa module (Not implemented yet)
      #define LORA_FREQUENCY_MODE 900 // Set the LoRa frequency mode, based on the physical module (only implemented for 900 MHz yet)
      
      #define LORA_SET_FREQUENCY_ON_STARTUP (LORA_MANAGER && 1) // If enabled, the ground station will set the LoRa frequency on startup based on the defined LORA_BASE_CHAN, LORA_BASE_ADDH and LORA_BASE_ADDL values.
        #define LORA_BASE_CHAN 42 // LoRa base frequency in MHz (needs to be inside the frequency range of the module)
        #define LORA_BASE_ADDH 0x00 // LoRa address high byte (hex)
        #define LORA_BASE_ADDL 0x2A // LoRa address low byte (hex)

#define SD_CARD 0

#define GPS_MODE 1
    #define URD_GPS_ENABLE (GPS_MODE && 1)

#define URD_BUZZER_ENABLE 0
#define URD_LED_ENABLE 1

#define FILE_NAME "GSLOG"

//------------------------------ LORA PARAMETERS ------------------------------//
#define LORA_CHAN 42 // LoRa channel (Standardized: 904 MHz)
#define LORA_ADDH 0x00
#define LORA_ADDL 0x2A // 42 decimal


//======================================================= END OF CONFIGURATIONS =================================================

//======================================================= PARAMETERS =================================================






//======================================================= END OF PARAMETERS =================================================

//------------------------------------------------- BOARD NAME -------------------------------------------------//
#if defined(PLACA_MAIN_OCTA_PTH)
  #define NOME_PLACA "PLACA MAIN OCTA PTH"
#elif defined(PLACA_ESSENTIALS_V1)
  #define NOME_PLACA "ESSENTIALS V1"
#elif defined(PLACA_MAIN_PTH)
  #define NOME_PLACA "PLACA MAIN PTH"
#elif defined(PLACA_MAIN_SMD)
  #define NOME_PLACA "PLACA MAIN SMD"
#elif defined(PLACA_MAIN_IC)
  #define NOME_PLACA "PLACA MAIN IC"
#endif

//------------------------------------------------- BOARDS VALIDATIONS -------------------------------------------------//


//------------------------------------------------- WARNINGS -------------------------------------------------//
