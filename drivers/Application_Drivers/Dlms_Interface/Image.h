/*
 * Image.h
 *
 * Created: 4/15/2023 1:07:11 PM
 *  Author: asamir
 */ 


#ifndef IMAGE_H_
#define IMAGE_H_

int image_transfer_preRead_callback(gxValueEventArg *e,gxImageTransfer *imageTransfer);
int image_transfer_preWrite_callback(gxValueEventArg *e,gxImageTransfer *imageTransfer);
int image_transfer_preAction_callback(gxValueEventArg *e,gxImageTransfer *imageTransfer);

#endif /* IMAGE_H_ */