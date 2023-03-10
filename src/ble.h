#include <BLEUtils.h>

#define SERVICE_MK312 "C88758C3-413A-48CD-8AB5-9C79DFD98535"

#define CHARACTERISTIC_LEVEL_A "CB7EFDF3-8066-40A5-91B7-8C1E1F498830"
#define CHARACTERISTIC_LEVEL_B "F1AFDE05-D324-49DA-B336-766434634BF3"
#define CHARACTERISTIC_MA "A6C54F45-7F46-4F21-8598-DAF474986B0F"
#define CHARACTERISTIC_MODE "398D747C-0A5E-4678-959C-C696D8147C09"
#define CHARACTERISTIC_POWER_LEVEL "11565DAB-9C96-41B9-BD72-C6621142FCF3"
#define CHARACTERISTIC_BATTERY_LEVEL "F43590D3-2CD7-40E0-BB22-6017B1C8DB81"

#define STR_MODE_POWERON "POWERON"
#define STR_MODE_UNKNOWN "UNKNOWN"
#define STR_MODE_WAVES "WAVES"
#define STR_MODE_STROKE "STROKE"
#define STR_MODE_CLIMB "CLIMB"
#define STR_MODE_COMBO "COMBO"
#define STR_MODE_INTENSE "INTENSE"
#define STR_MODE_RHYTHM "RHYTHM"
#define STR_MODE_AUDIO1 "AUDIO1"
#define STR_MODE_AUDIO2 "AUDIO2"
#define STR_MODE_AUDIO3 "AUDIO3"
#define STR_MODE_SPLIT "SPLIT"
#define STR_MODE_RANDOM1 "RANDOM1"
#define STR_MODE_RANDOM2 "RANDOM2"
#define STR_MODE_TOGGLE "TOGGLE"
#define STR_MODE_ORGASM "ORGASM"
#define STR_MODE_TORMENT "TORMENT"
#define STR_MODE_PHASE1 "PHASE1"
#define STR_MODE_PHASE2 "PHASE2"
#define STR_MODE_PHASE3 "PHASE3"
#define STR_MODE_USER1 "USER1"
#define STR_MODE_USER2 "USER2"
#define STR_MODE_USER3 "USER3"
#define STR_MODE_USER4 "USER4"
#define STR_MODE_USER5 "USER5"
#define STR_MODE_USER6 "USER6"
#define STR_MODE_USER7 "USER7"

#define STR_POWERLEVEL_LOW "LOW"
#define STR_POWERLEVEL_NORMAL "NORMAL"
#define STR_POWERLEVEL_HIGH "HIGH"

void setupBluetooth();