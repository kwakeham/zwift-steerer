uint16_t calculate_response_code( uint16_t challenge )
{
	int n = challenge % 11;
	uint16_t m = (challenge << n) | (challenge >> (16 - n));
	uint16_t x = ((challenge + 38550) ^ m);
	return x % 65336;
}
