#ifndef __API_CAMERA_H
#define __API_CAMERA_H
int OsCameraInit();
int OsCameraStopPreview();
int OsCameraStartPreview();
int OsCameraDecode(unsigned char * sweep_code_buf, unsigned int* sweep_code_len, int* sweep_code_type, int timeout);
int OsCameraGetIsPreview();
int OsCameraDeinit();
void OsCameraSetPreviewShow(int flag);
#endif

