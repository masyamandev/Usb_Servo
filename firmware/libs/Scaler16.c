#include "Scaler16.h"

#define SCALE_PRECISSION (16)

inline uint16_t scaleLinear(uint32_t a, uint16_t x) {
	return (a * (uint32_t) x) >> SCALE_PRECISSION;
}

inline void initScaler(struct Scaler16 *sp, uint16_t minX, uint16_t maxX, uint16_t minY, uint16_t maxY) {
	uint32_t lX = maxX - minX;
	uint32_t lY = maxY - minY;
	sp->a = (lY << SCALE_PRECISSION) / lX;
	sp->b = minY - scaleLinear(sp->a, minX);
}

inline uint16_t scale(struct Scaler16 *sp, uint16_t x) {
	return scaleLinear(sp->a, x) + sp->b;
}
