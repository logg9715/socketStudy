// ntoh_hton.c
#include        <stdio.h>

main() 
{
	short   hostData = 0x1234;	// 실제 저장공간에 34 12로 저장됨
	short   netData;
	char    *ch;

	printf("original data : 0x1234\n" );

	ch = (char *) &hostData;

	if ( ch[0] == 0x12 && ch[1] == 0x34 )
		printf("host(big-endian) : ");
	else if ( ch[1] == 0x12 && ch[0] == 0x34 )
		printf("host(little-endian) : ");	// 보통 이게 표시됨 

	printf("0x%x%x\n", ch[0], ch[1]);		// 3412
	printf("%x \n", hostData);				// 1234
	netData = htons(hostData);

	ch = (char *) &netData;
	printf("network data(big-endian) : 0x%x%x\n", ch[0], ch[1]);	//1234
	printf("hostdata : %x, netdata : %x\n", hostData, netData);	// 1234 3412
}
