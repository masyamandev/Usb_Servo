/*
 * Makes smooth scaling without float arithmetics.
 */

#include <stdint.h>

/*
 * Scaling values using formula: y = a * x + b;
 */
struct Scaler16 {
	uint32_t a;
	uint16_t b;
};

inline void initScaler(struct Scaler16 *sp, uint16_t minX, uint16_t maxX, uint16_t minY, uint16_t maxY);
inline uint16_t scale(struct Scaler16 *sp, uint16_t x);
