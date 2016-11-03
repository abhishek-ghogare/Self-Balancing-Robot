/*
 * image.h
 *
 *  Created on: Sep 21, 2016
 *      Author: Thor
 */

#ifndef IMAGE_H_
#define IMAGE_H_

unsigned char img[128][8]; // 128 columns, 8 pages

void SetPixel(unsigned int x, unsigned int y) {
	unsigned int page, pix;
	if (x>128 || y>64) return;

	page = y/8;
	pix = y%8;

	img[x][page] |= 1<<pix;
}

void ResetPixel(unsigned int x, unsigned int y) {
	unsigned int page, pix;
	if (x>128 || y>64) return;

	page = y/8;
	pix = y%8;

	img[x][page] &= ~(0x01<<pix);
}

void ClearImage() {
	int i,j;
	for (i=0;i<128;++i)
		for (j=0;j<8;++j)
			img[i][j]=0x00;
}

void SetByte (unsigned int startx, unsigned int y, unsigned char byte) {
	int i;
	for (i=0;i<8;++i) {
		if (byte & (0x01<<i))
			SetPixel(startx+7-i, y);
		else
			ResetPixel(startx+7-i, y);
	}
}

#endif /* IMAGE_H_ */
