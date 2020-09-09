#include <cstdint>

//
// it's taken a few days but the (almost!) complete list of challenge and responses that was published has,
// (after much head scratching, hair pulling etc ..) enabled me to work out the functional relationship
//

uint16_t calculate_response_code( uint16_t challenge )
{
	int n = challenge % 11;
	uint16_t m = (challenge << n) | (challenge >> (16 - n));
	uint16_t x = ((challenge + 38550) ^ m);
	return x % 65336; // this value is correct (perhaps a typo for 65536 in the originial?)
}
