#include "stdafx.h"

#include "OrMaskSynthesizer.h"

void OrMaskSynthesizer::Synth(
	IplImageExt *over_image, int posX, int posY, int width, int height,
	const IplImageExt *under_image, int startX, int startY) const
{
	if(posX < 0){
		width = width - posX;
		posX = 0;
	}
	if(posY < 0){
		height = height - posY;
		posY = 0;
	}

	int x,y;
	int8_t dat1, dat2;
	for(y=0; y<height; y++){
		for(x=0; x<width; x++){
			if(under_image->GetMaskData(startX + x, startY + y, &dat2)){
				over_image->GetMaskData(posX + x, posY + y, &dat1);

				over_image->SetMaskData(
					posX + x,
					posY + y,
					max(dat1, dat2));
			}
			else{
				x = under_image->GetNextBlockPosX(startX + x) - startX;
			}
		}
	}
}