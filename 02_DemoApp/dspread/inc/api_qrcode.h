#ifndef _API_QRCODE_H
#define _API_QRCODE_H


enum API_QRCODE_ERROR_LEVEL {
	api_qrcodegen_Ecc_LOW = 0,  // The QR Code can tolerate about  7% erroneous codewords
	api_qrcodegen_Ecc_MEDIUM,  // The QR Code can tolerate about 15% erroneous codewords
	api_qrcodegen_Ecc_QUARTILE,  // The QR Code can tolerate about 25% erroneous codewords
	api_qrcodegen_Ecc_HIGH,  // The QR Code can tolerate about 30% erroneous codewords
};

int OsQrcodeShow(const char* content, int startX, int startY, int width, int height, int error_level);
#endif