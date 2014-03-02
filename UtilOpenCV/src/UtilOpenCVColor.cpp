#include "stdafx.h"
#include "UtilOpenCVColor.h"


//////////////////////////////////////////////
//  RGB <==> HSV
//////////////////////////////////////////////
_EXPORT UCvPixel32 ucvHSV2RGB(double h, double s, double v)
{
	if(h >= 360.0){
		h -= 360.0;
	}
	if(s == 0.0){
		UCvPixel32 color;
		color.b = color.g = color.r = 255*v;
		color.a = 255;
		return color;
	}
	else{
		UCvPixel32 color;
		int r=0,g=0,b=0;

		int hi = (int)floor((h/60.0))%6;
		//int hi = (int)floor(h/60.0);
		double f = h/60.0 - hi;
		double m = v * (1.0 - s) * 255.0;
		double n = v * (1.0 - s * f) * 255.0;
		double k = v * (1.0 - s * (1-f)) * 255.0;
		double vd = v * 255.0;

		switch(hi){
			case 0:
				r = vd; g = k;  b = m;
				break;
			case 1:
				r = n;  g = vd; b = m;
				break;
			case 2:
				r = m;  g = vd; b = k;
				break;
			case 3:
				r = m;  g = n;  b = vd;
				break;
			case 4:
				r = k;  g = m;  b = vd;
				break;
			case 5:
				r = vd; g = m;  b = n;
				break;
		}
		if(r > 255) r = 255;
		if(g > 255) g = 255;
		if(b > 255) b = 255;

		//color = 0xFF000000 | ((r << 16) & 0x00FF0000) | ((g << 8) & 0x0000FF00) | b & 0x000000FF;
		//color = 0x55000000 | (r<<16) | (g<<8) | b;
		color.b = b;
		color.g = g;
		color.r = r;
		color.a = 255;
		return color;
	}
}

_EXPORT double ucvRGB2V(UCvPixel32 color)
{
	int r = color.r;
	int g = color.g;
	int b = color.b;
	int max = max(r, max(g, b));

	return (double)max / 255.0;
}

_EXPORT void ucvRGB2HSV(UCvPixel32 color, double *h, double *s, double *v)
{
	int r = color.r;
	int g = color.g;
	int b = color.b;

	int max = max(r, max(g, b));
	int min = min(r, min(g, b));

	*s = (max != 0) ? ((double)max - min)/max : 0.0;
	*v = (double)max / 255.0;

	if(max - min == 0){
		//*h = 0.0;
		//h‚Í•Ï‰»‚È‚µ
		return;
	}
	else{
		if(max == r){
			*h = 60.0 * (((double)g - b)/(max - min));
		}
		else if(max == g){
			*h = 60.0 * (2.0 + ((double)b - r)/(max - min));
		}
		else if(max == b){
			*h = 60.0 * (4.0 + ((double)r - g)/(max - min));
		}
	}
	
	if(*h < 0){
		*h += 180;
		//*h *= -1.0;
		//*h += 270;
	}

}



//////////////////////////////////////////////
//  RGB <==> HLS
//////////////////////////////////////////////
static inline int hsl_value_int(double n1, double n2, double hue)
{
	double value;
	if(hue > 255)
		hue -= 255;
	else if(hue < 0)
		hue += 255;

	if(hue < 42.5)
		value = n1 + (n2 - n1) * (hue / 42.5);
	else if(hue < 127.5)
		value = n2;
	else if(hue < 170)
		value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
	else
		value = n1;

  return (value * 255.0)+0.5;
}

_EXPORT UCvPixel32 ucvHLS2RGB(int h, int l, int s)
{
	UCvPixel color;
	if(s == 0){
		color.r = l;
		color.g = l;
		color.b = l;
		color.a = 255;
	}
	else{
		double m1, m2;

		if(l < 128)
			m2 = (l* (255 + s)) / 65025.0;
		else
			m2 = (l + s - (l * s) / 255.0) / 255.0;

		m1 = (l / 127.5) - m2;
		color.r = hsl_value_int(m1, m2, h + 85);
		color.g = hsl_value_int(m1, m2, h);
		color.b = hsl_value_int(m1, m2, h - 85);
		color.a = 255;
	}

	return color;
}

_EXPORT int ucvRGB2L(UCvPixel32 color)
{
	int imin, imax;
	if(color.r > color.g){
		imax = max(color.r, color.b);
		imin = min(color.g, color.b);
	}
	else{
		imax = max(color.g, color.b);
		imin = max(color.r, color.b);
	}
	return (imax + imin)/2.0 + 0.5;
}

_EXPORT void ucvRGB2HLS(UCvPixel32 color, int* hue, int* light, int* saturation)
{
	double h, s, l;
	int imin, imax;
	int delta;

	if(color.r > color.g){
		imax = max(color.r, color.b);
		imin = min(color.g, color.b);
	}
	else{
		imax = max(color.g, color.b);
		imin = min(color.r, color.b);
	}

	l = (imax + imin) / 2.0;

	if(imax == imin){
		s = 0.0;
		h = 0.0;
	}
	else{
		delta = (imax - imin);

		if(l < 128)
			s = 255 * (double)delta / (double)(imax + imin);
		else
			s = 255 * (double)delta / (double)(511 - imax - imin);

		if(color.r == imax)
			h = (color.g - color.b) / (double)delta;
		else if(color.g == imax)
			h = 2 + (color.b - color.r) / (double)delta;
		else
			h = 4 + (color.r - color.g) / (double)delta;

		h = h * 42.5;

		if(h < 0)
			h += 255;
		else if(h > 255)
			h -= 255;
	}

	(*hue) = h + 0.5;
	(*light) = l + 0.5;
	(*saturation) = s + 0.5;
}