#include "pspimport.s"

	IMPORT_START	"sceDmac", 0x40010011
	IMPORT_FUNC	"sceDmac", 0x617F3FE6, sceDmacMemcpy

	IMPORT_START	"scePower", 0x40010011
	IMPORT_FUNC	"scePower", 0xFEE03A2F, scePowerGetCpuClockFrequency
	IMPORT_FUNC	"scePower", 0xFDB5BFE9, scePowerGetCpuClockFrequencyInt
	IMPORT_FUNC	"scePower", 0x478FE6F5, scePowerGetBusClockFrequency
	IMPORT_FUNC	"scePower", 0xBD681969, scePowerGetBusClockFrequencyInt
	IMPORT_FUNC	"scePower", 0x737486F2, scePowerSetClockFrequency
	IMPORT_FUNC	"scePower", 0x469989AD, scePower_469989AD
	IMPORT_FUNC	"scePower", 0xEBD177D6, scePower_EBD177D6
