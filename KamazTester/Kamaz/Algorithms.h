/*
���������� ����������
*/

#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include <API\api.h>

#include "alg_consts.h"

typedef enum
{
	ALG_OK,
	ALG_ERROR,
	ALG_CRITICAL_ERROR
} AlgResult;



//���������������  ������� ��� ���������
AlgResult A0(void);

//�������� ����������������� �������� ���������� ������� ��������� �8 � �9
AlgResult A1(void);

//��������� ������������ �������� �0 � ������������� � ������� ��������
AlgResult A2(void);

//��������� ���������� �������� � �������� ����������
AlgResult A3(void);

//�������� ������������� ��� �� ���������� ��������
AlgResult A4(void);

//�������� ������������� ��� �� ������
AlgResult A5(void);

//��������� �������� �������� ������� � �����
void A6(uint8_t wheels);

//������ ������� � �������� ������
void B0(int16_t requiredWheelsPressure);

//��������� �������� ������� � ������ (-��)
void B1(uint8_t wheels, uint16_t requiredPressure);

//�������� �������� ������� � ������ (-��)
void B2(uint8_t wheels, uint16_t requiredPressure);

//����������� ����������� ������
void G0(void);

void D0(uint8_t damagedWheels);

#endif
