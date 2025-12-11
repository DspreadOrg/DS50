#ifndef __API_RSA_H
#define __API_RSA_H





int OsRsaSignSha256(char* RSA_N, char* RSA_E, char* RSA_D, char* RSA_P, char* RSA_Q, unsigned char* rsa_plaintext,
	int PT_LEN, unsigned char* signout, int signbuflen);

int OsRsaParsePublicKey(char* keyfile, char* n_str, unsigned int n_strlen,
	char* e_str, unsigned int e_strlen);


int OsRsaParsePrivateKey(char* keyfile, char* n_str, int n_strlen,
	char* p_str, int p_strlen, char* q_str, int q_strlen,
	char* d_str, int d_strlen, char* e_str, int e_strlen,
	char* dp_str, int dp_strlen, char* dq_str, int dq_strlen,
	char* qp_str, int qp_strlen);
#endif