
#include "sdk.h"

typedef struct {
	char key;
	int keyIndex;
	int keyArraySize;
	char keyArray[32];
}keymap;


keymap map[][11] = {
		{
		{'1',0,1,{'1'}},
		{'2',0,1,{'2'}},
		{'3',0,1,{'3'}},
		{'4',0,1,{'4'}},
		{'5',0,1,{'5'}},
		{'6',0,1,{'6'}},
		{'7',0,1,{'7'}},
		{'8',0,1,{'8'}},
		{'9',0,1,{'9'}},
		{'0',0,1,{'0'}},
		{'.',0,1,{'.'}}
		},

		{
		{'1',0,2,{ 'q','z'}},
		{'2',0,3,{'a','b','c'}},
		{'3',0,3,{'d','e','f'}},
		{'4',0,3,{'g','h','i'}},
		{'5',0,3,{'j','k','l'}},
		{'6',0,3,{'m','n','o'}},
		{'7',0,3,{'p','r','s'}},
		{'8',0,3,{'t','u','v'}},
		{'9',0,3,{'w','x','y'}},
		{'0',0,27,{' ', ',','@','+','-','*','_','/','!','?','%','<','>','&','[',']','{','}','(',')','$','#','\\','^','|','~','`'}},
		{'.',0,1,{'.'}}
		},
		{
		{'1',0,2,{ 'Q','Z'}},
		{'2',0,3,{'A','B','C'}},
		{'3',0,3,{'D','E','F'}},
		{'4',0,3,{'G','H','I'}},
		{'5',0,3,{'J','K','L'}},
		{'6',0,3,{'M','N','O'}},
		{'7',0,3,{'P','R','S'}},
		{'8',0,3,{'T','U','V'}},
		{'9',0,3,{'W','X','Y'}},
		{'0',0,27,{' ', ',','@','+','-','*','_','/','!','?','%','<','>','&','[',']','{','}','(',')','$','#','\\','^','|','~','`'}},
		{'.',0,1,{'.'}}
		},

};



char keymap_get_nextChar(int keypadMode, unsigned int key) {
	int i = 0, j = 0;
	char res;
	int mapsize = sizeof(map[0]) / sizeof(map[0][0]);
	if (key == '0') {
		map[keypadMode][9].keyIndex = (map[keypadMode][9].keyIndex + 1) % map[keypadMode][9].keyArraySize;
		res = map[keypadMode][9].keyArray[map[keypadMode][9].keyIndex];
	}
	else {
		map[keypadMode][key - '1'].keyIndex = (map[keypadMode][key - '1'].keyIndex + 1) % map[keypadMode][key - '1'].keyArraySize;

		res = map[keypadMode][key - '1'].keyArray[map[keypadMode][key - '1'].keyIndex];
	}
	return res;


}


char keymap_get_curChar(int keypadMode, unsigned int key) {
	int i = 0, j = 0;
	char res;
	int mapsize = sizeof(map[0]) / sizeof(map[0][0]);
	if (key == '0') {
		res = map[keypadMode][9].keyArray[map[keypadMode][9].keyIndex];
	}
	else {
		API_LOG_DEBUG("keyindex:%d\r\n", map[keypadMode][key - '1'].keyIndex);
		res = map[keypadMode][key - '1'].keyArray[map[keypadMode][key - '1'].keyIndex];
	}
	return res;
}


void keymap_reset_step() {
	int i = 0, j = 0;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 11; j++)
			map[i][j].keyIndex = 0;
	}
}
