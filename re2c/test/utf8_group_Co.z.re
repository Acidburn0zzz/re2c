#include <stdio.h>
bool scan(const char * start, const char * const limit)
{
#	define YYCTYPE unsigned char
#	define YYCURSOR start
Co:
	/*!re2c
		re2c:yyfill:enable = 0;
				Co = [\ue000-\uf8ff\U000f0000-\U000ffffd\U00100000-\U0010fffd];
		Co { goto Co; }
		[^] { return YYCURSOR == limit; }
	*/
}
int main ()
{
	if (!scan (buffer_Co, buffer_Co + sizeof (buffer_Co) - 1))
		printf("test 'Co' failed\n");
}