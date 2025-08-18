#ifndef __LONGFLOAT_H
#define __LONGFLOAT_H

typedef struct {
	long i;
	float f;
} LONGFLOAT;

long MyGetI( LONGFLOAT * pNum );
float MyGetf( LONGFLOAT * pNum );
LONGFLOAT * MySet( LONGFLOAT * pNum, long i, float f );
LONGFLOAT * MyCopy( LONGFLOAT * pDes, LONGFLOAT * pSrc );
LONGFLOAT * MyConvert( LONGFLOAT * pNum, float value );
LONGFLOAT * MyStore( LONGFLOAT * pDes, LONGFLOAT * pSrc );
LONGFLOAT * MyStoref( LONGFLOAT * pDes, float value );
LONGFLOAT * MyStorei( LONGFLOAT * pDes, long value );
LONGFLOAT * MySum( LONGFLOAT * pDes, LONGFLOAT * pSrc1, LONGFLOAT * pSrc2 );
LONGFLOAT * MySumf( LONGFLOAT * pDes, LONGFLOAT * pSrc, float value );
LONGFLOAT * MyRemove( LONGFLOAT * pDes, LONGFLOAT * pSrc );
LONGFLOAT * MyRemovef( LONGFLOAT * pDes, float value );
LONGFLOAT * MyRemovei( LONGFLOAT * pDes, long value );
float MySub_FMM( LONGFLOAT * pSrc1, LONGFLOAT * pSrc2 );
float MySub_FMF( LONGFLOAT * pSrc, float value );
float MySub_FML( LONGFLOAT * pSrc, long value );

#endif
