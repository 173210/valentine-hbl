#include "pspimport.s"

#ifdef F_scePower_0000
	IMPORT_START	"scePower", 0x40010011
#elif defined(F_scePower_0001)
	IMPORT_FUNC	"scePower", 0xFEE03A2F, scePowerGetCpuClockFrequency
#elif defined(F_scePower_0002)
	IMPORT_FUNC	"scePower", 0xFDB5BFE9, scePowerGetCpuClockFrequencyInt
#elif defined(F_scePower_0003)
	IMPORT_FUNC	"scePower", 0x478FE6F5, scePowerGetBusClockFrequency
#elif defined(F_scePower_0004)
	IMPORT_FUNC	"scePower", 0xBD681969, scePowerGetBusClockFrequencyInt
#elif defined(F_scePower_0005)
	IMPORT_FUNC	"scePower", 0x737486F2, scePowerSetClockFrequency
#elif defined(F_scePower_0006)
	IMPORT_FUNC	"scePower", 0x469989AD, scePower_469989AD
#elif defined(F_scePower_0007)
	IMPORT_FUNC	"scePower", 0xEBD177D6, scePower_EBD177D6
#endif
