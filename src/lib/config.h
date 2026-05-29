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

#define SD_CARD 0

#define GPS_MODE 1
    #define URD_GPS_ENABLE (GPS_MODE && 1)

#define URD_BUZZER_ENABLE 0
#define URD_LED_ENABLE 1

#define FILE_NAME "GSLOG"


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
