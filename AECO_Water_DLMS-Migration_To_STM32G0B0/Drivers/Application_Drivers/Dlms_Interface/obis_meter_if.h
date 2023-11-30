#ifndef _OBIS_METER_H_
#define _OBIS_METER_H_

#include <stdint.h>

typedef uint32_t errorcode_t;

#define	ERRCODE_OBJ_NOT_AVAILABLE	1UL
#define ERRCODE OBJ_NOT_SUPPORTED	2UL
#define ERRCODE_OBJ_NOT_RELIABLE	3UL
#define ERRCODE_INVALID_INPUT_PARAM	4UL
#define ERRCODE_UNDEFINED			5UL

#define OBJ_OCTET_STRING_MAX		128UL
typedef struct {
	uint8_t data[OBJ_OCTET_STRING_MAX];
	uint32_t len;
} octet_string_t;

#ifdef DEBUG_ENABLED
typedef struct {
	char str[16];
} errorcode_str_t;

const errorcode_str_t errorcode[] = {"",
									  "Not available",
								      "Not supported",
								      "Not reliable",
									  "Invalid parameter",
								      "undefined"};
print_error_code(errorcode_t errcode) {
	/* TODO: use debug channel to print debug messages */
}
#endif /* __DEBUG_ENABLED */

int32_t get_device_id(octet_string_t* device_id, errorcode_t *errcode);
int32_t get_firmware_identifier(octet_string_t *firmware_id, errorcode_t *errcode);

#endif /* _OBIS_METER_H_ */
