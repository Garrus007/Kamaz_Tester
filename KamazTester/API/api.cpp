#include "api.h"


/////////////////////////// RTOS /////////////////////////////////////////
void vTaskDelay(int delay)
{
	throw "not_implemented";
}

TickType_t xTaskGetTickCount(void)
{
	throw "not_implemented";
}


/////////////////////////// VALVES ///////////////////////////////////////
//���������� ������ �� ������
uint16_t ValveFromNumber(uint8_t valveNumber)
{
	throw "not_implemented";
}

//���������� ����� �� �������
uint8_t NumberFromValve(uint16_t valve)
{
	throw "not_implemented";
}

//��������� � ��������� ������� �� ������ ������� �����
void SetValves(uint16_t valves)
{
	throw "not_implemented";
}

//���������/��������� ���� ������
void SetValve(uint16_t valve, uint8_t state)
{
	throw "not_implemented";
}


/////////////////////////// VALVES POWER /////////////////////////////////
uint16_t GetCircuitStatus(void)
{
	throw "not_implemented";
}


/////////////////////////// ADC //////////////////////////////////////////
int16_t GetAdcValue(void)
{
	throw "not_implemented";
}


/////////////////////////// CAN //////////////////////////////////////////
long CAN_GetContour4Response(void* pvBuffer, TickType_t xTicksToWait)
{
	throw "not_implemented";
}

void CAN_Send_Command(uint8_t* buffer)
{
	throw "not_implemented";
}