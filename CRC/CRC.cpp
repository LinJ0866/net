#include <stdio.h>

#define uchar unsigned char
#define uint unsigned int
#define FACTOR8 0x07
#define FACTOR16 0x1021

// �ֽڷ�ת 
uchar Reverse(uchar Data)
{
	uchar rev = 0x00;
	for(int i=0; i<8; i++)
	{
		rev = (rev << 1) + (Data & 0x01);
		Data >>= 1;
	}
	return rev;
}

/****************************Info********************************************** 
 * Name:    CRC-8        x8+x2+x1+1 
 * Width:   8
 * Poly:    0x07 
 * Init:    0x0000 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 *****************************************************************************/ 
void calc_CRC8(uchar *pData, uint len)
{
	uchar i;
	uchar crc = 0x00;
	while(len--)
	{
		crc ^= *pData++;
		for (i=8; i>0; --i)
		{
			if(crc & 0x80)
				crc = (crc << 1) ^ FACTOR8;
			else
				crc = (crc << 1);
		}
	}
	printf("CRC-8У������0x%x\n", crc);
}

/****************************Info********************************************** 
 * Name:    CRC-16/CCITT        x16+x12+x5+1 
 * Width:   16
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   True 
 * Refout:  True 
 * Xorout:  0x0000 
 *****************************************************************************/ 
void calc_CRC16_CCITT(uchar *pData, uint len)
{
	short int crc = 0x0000;
	while(len--)
	{
		*pData = Reverse(*pData);
		crc ^= (short int)(*pData++ << 8);
		for (int i=8; i>0; --i)
		{
			if(crc & 0x8000)
				crc = (crc << 1) ^ FACTOR16;
			else
				crc = (crc << 1);
		}
	}
	printf("CRC-16/CCITTУ������0x%02x%02x\n", Reverse(crc), Reverse((crc & 0xff00) >> 8));
}

/****************************Info********************************************** 
 * Name:    CRC-16/XMODEM        x16+x12+x5+1 
 * Width:   16
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000  
 *****************************************************************************/ 
void calc_CRC16_XMODEM(uchar *pData, uint len)
{
	short int crc = 0x0000;
	while(len--)
	{	
		crc ^= (short int)(*pData++ << 8);
		for (int i=8; i>0; --i)
		{
			if(crc & 0x8000)
				crc = (crc << 1) ^ FACTOR16;
			else
				crc = (crc << 1);
		}
	}
	printf("CRC-16/XMODEMУ������0x%04hx\n", crc);
}

/*****�Զ���CRC16*********/ 
void calc_CRC16(uchar *pData, uint len, short int POLY, short int INIT, short int XOROUT, int REFIN, int REFOUT)
{
	printf("\n**********Info************************\n\n");
	printf(" * Poly:    0x%04hx\n\n * Init:    0x%04hx\n\n * Xorout:  0x%04hx\n\n * Refin:   %s\n\n * Refout:   %s\n\n", POLY, INIT, XOROUT, REFIN?"True":"False", REFOUT?"True":"False"); 
	
	short int crc = 0x0000;
	while(len--)
	{
		if(REFIN)
			*pData = Reverse(*pData);
		
		*pData ^= INIT;		
		crc ^= (short int)(*pData++ << 8);
		for (int i=8; i>0; --i)
		{
			if(crc & 0x8000)
				crc = (crc << 1) ^ POLY;
			else
				crc = (crc << 1);
		}
	}
	crc ^= XOROUT;
	
	printf("\n----------------------------------\n");
	if(REFOUT)
		printf("У������0x%02x%02x\n", Reverse(crc), Reverse((crc & 0xff00) >> 8));
	else
		printf("У������0x%04hx\n", crc);
	
}

int main()
{
	printf("-----------------------------------------------\n");
	printf("               CRCУ�鹤��\n");
	printf("-----------------------------------------------\n\n");
	printf("(*) ��������ҪУ������֣�\n");
	printf("    PS:���������Ϊ16���ƣ���q���������磺31 32 33 34 q\n");
	
	uchar a[20];
	uchar *pa = a;
	int c=0;
	while(scanf("%x", pa++) && *pa != 'q')
		c++;
	
	printf("\n(*) ��ѡ��У���㷨��\n");
	printf("\n   [1] CRC8   [2] CRC16_CCITT\n\n   [3] CRC16_XMODEM   [4] ������Ҫ\n\n   [5] �Զ���CRC16\n");
	
	char t[4];
	scanf("%*c%s", t);
	while(t[0]<='0' && t[0]>'4')
	{
		printf("������������������..\n");
		scanf("%s", t);
	}
	printf("\n----------------\n");
	
	switch(t[0])
	{
		case '1': calc_CRC8(a, c); break;
		case '2': calc_CRC16_CCITT(a, c); break;
		case '3': calc_CRC16_XMODEM(a, c); break;
		case '4': calc_CRC8(a, c); calc_CRC16_XMODEM(a, c); calc_CRC16_CCITT(a, c); break;
		case '5': 
			short int POLY=0x1021, INIT=0x0000, XOROUT=0x00;
			int REFIN=0, REFOUT=0;
			printf("�밴����˳�����������\n\n");
			printf("         POLY        INIT      XOROUT     REFIN  REFOUT\n");
			printf("����    0x1021      0x0000     0x0000       0       0(0Ϊfalse��1Ϊtrue)\n\n");
			scanf("%hx%hx%hx%d%d", &POLY, &INIT, &XOROUT, &REFIN, &REFOUT);
			calc_CRC16(a, c, POLY, INIT, XOROUT, REFIN, REFOUT);
	}
	
	return 0;	
}
