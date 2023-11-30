/*
* Image.c
*
* Created: 4/15/2023 1:06:04 PM
*  Author: asamir
*/
/*//Allocate space for image tranfer.*/
#include "Includes/cosem.h"
#include "utilities.h"
/**TODO:: handle external flash  | handle secure element **/

/* #include "External_Flash/Eflash.h"
 * #include "Secure_Element/secure.h"
#include "External_Flash/f_update.h" */

uint32_t last_flash = 0x1000;

uint32_t newImageFirmwareLocationInFlash = 0x1000;

uint32_t newImageDecryptedFirmwareLocationInFlash = 0x97000;

/*
// sara todo chek what is needed

// Get the start address and size of a specific command's data from bulk data
// Returns 1 if the command data was found, 0 otherwise
*/
int get_command_data(const uint8_t* bulk_data, uint32_t bulk_size, uint8_t command_id, uint32_t** data_ptr, uint32_t* data_size_ptr, uint32_t* pos) {
	uint32_t index = 0;
	while (index < bulk_size) {
		/*// Check if the current command number matches the desired command number*/
		uint8_t current_command_id = bulk_data[index];
		index++;
		if (current_command_id != command_id) {
			/*// Skip this command's data and move on to the next command*/
			uint8_t data_size = bulk_data[index];
			index += (data_size + 1);
			continue;
		}
		
		uint32_t data_size;

		/*// Get the size of the command's data*/
		if(current_command_id == 0x06)
		{
			data_size = (bulk_data[index + 0] << 24) |
			(bulk_data[index + 1] << 16) |
			(bulk_data[index + 2] << 8) |
			bulk_data[index + 3];
			index+=4;
		}
		else
		{
			data_size = bulk_data[index];
			index++;
		}
		
		*pos = index;
		
		/*// Set the start address and size of the command's data*/
		*data_ptr = (uint8_t*)&bulk_data[index];
		*data_size_ptr = data_size;
		

		return 1;
	}

	/*// Command data not found*/
	return 0;
}

int image_transfer_preRead_callback(gxValueEventArg *e,gxImageTransfer *imageTransfer){
	/*//image_transfer_callback(e,&imageTransfer,IMAGE_ACTIVATE_INFO);*/
	e->value.vt = DLMS_DATA_TYPE_INT8;
	e->handled = 1;
	if(e->index == 4){
		e->value.bVal = imageTransfer->imageTransferStatus;
	}
	else if(e->index == 5){
		e->value.bVal = imageTransfer->imageTransferEnabled;
	}else if(e->index == 6){
		e->value.bVal = 1;
	}
	return 0;
}

int image_transfer_preWrite_callback(gxValueEventArg *e,gxImageTransfer *imageTransfer){
	e->handled = 1 ;
	if(e->index == 5){
		imageTransfer->imageTransferEnabled = e->value.bVal;
	}
	return 0;
}
int image_transfer_preAction_callback(gxValueEventArg *e,gxImageTransfer *imageTransfer){
	/*//image_transfer_callback(e,&imageTransfer,IMAGE_ACTIVATE_INFO);*/
	
	/*int ret = 0;*/
	/*int pos=0;*/
	e->handled = 1;
	
	/*//Image name and size to transfer*/
	if (e->index == 1)
	{
		/*//secure_setup();*/
		imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_NOT_INITIATED;
		/*TODO:: we have to rease more than 150 sector to avoid if the received image is more than 600KB and we can use erase block instead */
		for (uint16_t i = 0 ; i < 150; i++)
		{
			/**TODO:: Handle external flash **/
			/* ext_flash_EraseSector(i); */
		}
		last_flash = newImageFirmwareLocationInFlash;
		/**TODO:: Handle external flash **/
		/* init_bootsign(); */

		/*//There is only one image.*/
		/*gxImageActivateInfo* info;*/
		imageTransfer->imageActivateInfo.size = 1;
		
		imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_INITIATED;
	}
	/*//Transfers one block of the Image to the server*/
	else if (e->index == 2)
	{
		/*//	return;*/
		/*uint32_t index;
		uint16_t blockSize;*/
		
		gxByteBuffer* toarr =(gxByteBuffer* ) e->parameters.Arr;
		
		/**TODO:: Handle external flash **/
		/* ext_flash_WriteBytes(toarr->data, last_flash, toarr->size); */

		last_flash+=toarr->size;
		
		
		bb_clear( (e->parameters.Arr));
		
		#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)/*//If Windows or Linux*/
		imageActionStartTime = time(NULL);
		#endif
		
		return (0);
	}
	/*//Verifies the integrity of the Image before activation.*/
	else if (e->index == 3)
	{
		
		
		imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_INITIATED;
		/*//  logic*/
		
		uint8_t bulk_data[100];
		/**TODO:: Handle external flash **/
		/* ext_flash_ReadBytes(newImageFirmwareLocationInFlash, 100, bulk_data); */
/*//		uint32_t* sign_data_ptr;*/
		uint8_t* sign_data_ptr;
		uint32_t sign_data_size;
		uint32_t pos;
		if (get_command_data(bulk_data, sizeof(bulk_data), 0x05, &sign_data_ptr, &sign_data_size, &pos))
		{
			uint32_t* bin_data_ptr;
			uint32_t bin_data_size;
			if (get_command_data(bulk_data, sizeof(bulk_data), 0x06, &bin_data_ptr, &bin_data_size, &pos))
			{
				#if 0
				if(FirmwareVerify(newImageFirmwareLocationInFlash + pos, bin_data_size, sign_data_ptr))
				{
					imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_SUCCESSFUL;
				}
				else
				{
					imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED;
				}
				#endif
				#if 1
				/*TODO:: you have to integrate External flash and Secure element library*/
					/*fw_update.enable = true;
					*verification main parameters *
					fw_update.flash_addr=newImageFirmwareLocationInFlash + pos;
					fw_update.data_size=bin_data_size;

					memcpy(fw_update.sig,sign_data_ptr,64);
					fw_update.state = SECURE_INITIATE;*/
				#endif
			}
			else
			{
				imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED;
			}
		}
		else
		{
			imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_VERIFICATION_FAILED;
		}

		
	}
	/*//Activates the Image.*/
	else if (e->index == 4)
	{
		
		
		imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_INITIATED;
		/*//  logic
		//#include "cryptoauthlib.h"*/
		
		uint8_t* bin_data_ptr;
		uint32_t bin_data_size;
		uint8_t bulk_data[100];
		uint32_t pos;
		/**TODO:: Handle external flash **/
		/* ext_flash_ReadBytes(newImageFirmwareLocationInFlash, 100, bulk_data); */
		
		if (get_command_data(bulk_data, sizeof(bulk_data), 0x06, &bin_data_ptr, &bin_data_size, &pos))
		{
			/*//if(decryped_Bin_in_flash( newImageDecryptedFirmwareLocationInFlash, newImageFirmwareLocationInFlash + pos ,bin_data_size) == 1 ) {*/
			if(1){
				imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_SUCCESSFUL;
				/** TODO:: look for how to swap two stack pointers for ST family****/
				/*_swap_prime_stack_pointer(newImageFirmwareLocationInFlash , bin_data_size);*/
			}
			else{
				imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_FAILED;
			}
			
			}else{
			imageTransfer->imageTransferStatus = DLMS_IMAGE_TRANSFER_STATUS_ACTIVATION_FAILED;
		}
			
		
	
	}
	return (0);
}



