#ifndef __API_H__
#define __API_H__


#include <stdint.h>

#define K1_Valve  1
#define K2_Valve  2
#define K3_Valve  4
#define K4_Valve  8
#define K5_Valve  16
#define K6_Valve  32
#define K7_Valve  64
#define K8_Valve  128
#define K9_Valve  256
#define K10_Valve 512

#define Valves_All  0x03FF
#define Valves_None 0

#define Wheels_All 0x3F

#define VALVE_OPEN   1
#define VALVE_CLOSE  0


#define TickType_t int


#define COMMAND_LENGTH                      8       //����� ���� � �������
#define RESULT_LENGTH                       8       //����� ���� � ������
#define CAN_CMD_Circuit_Request             1
#define portMAX_DELAY						0xffffffffUL

/////////////////////////// RTOS /////////////////////////////////////////
void vTaskDelay(int delay);
TickType_t xTaskGetTickCount(void);


/////////////////////////// VALVES ///////////////////////////////////////
//���������� ������ �� ������
uint16_t ValveFromNumber(uint8_t valveNumber);

//���������� ����� �� �������
uint8_t NumberFromValve(uint16_t valve);

//��������� � ��������� ������� �� ������ ������� �����
void SetValves(uint16_t valves);

//���������/��������� ���� ������
void SetValve(uint16_t valve, uint8_t state);


/////////////////////////// VALVES POWER /////////////////////////////////
uint16_t GetCircuitStatus(void);


/////////////////////////// ADC //////////////////////////////////////////
int16_t GetAdcValue(void);


/////////////////////////// CAN //////////////////////////////////////////
long CAN_GetContour4Response(void* pvBuffer, TickType_t xTicksToWait);
void CAN_Send_Command(uint8_t* buffer);

#endif