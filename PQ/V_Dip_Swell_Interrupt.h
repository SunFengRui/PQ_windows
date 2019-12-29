#pragma once
#include "time.h"
#include <windows.h>
#include <stdio.h>

#define DeclaredInputVoltageUdin     1.41421356  
#define VoltagedipThreshold (0.9*DeclaredInputVoltageUdin)
#define VoltageswellThreshold (1.1*DeclaredInputVoltageUdin)
#define VoltageinterruptThreshold (0.1*DeclaredInputVoltageUdin)

void A_voltagedipcalculation(void);
void A_voltageswellcalculation(void);
void A_voltageinterruptioncalculation(void);
void A_voltagedipswellinterruptiondetection(void);

void B_voltagedipcalculation(void);
void B_voltageswellcalculation(void);
void B_voltageinterruptioncalculation(void);
void B_voltagedipswellinterruptiondetection(void);

void C_voltagedipcalculation(void);
void C_voltageswellcalculation(void);
void C_voltageinterruptioncalculation(void);
void C_voltagedipswellinterruptiondetection(void);