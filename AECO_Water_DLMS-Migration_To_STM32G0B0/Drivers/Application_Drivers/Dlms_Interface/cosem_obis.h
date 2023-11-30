
#ifndef _OBIS_INIT_H_
#define _OBIS_INIT_H_

#include <stdint.h>
#include "Includes/cosem.h"

typedef struct {
	void	*priv;
} obis_settings_t;

int32_t init_obis(dlmsServerSettings* settings_ptr);

#endif /* _OBIS_INIT_H_ */
