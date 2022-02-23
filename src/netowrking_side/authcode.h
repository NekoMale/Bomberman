#include <stdio.h>
#include <time.h>

const char* authcode_params1 = "AEFT059";
const char* authcode_params2 = "BGHPZ";
const char* authcode_params3 = "12PPMN";
const char* authcode_params4 = "YJ3UVOQR";
const char* authcode_params5 = "786CDIKLSXW";

char* __ng_authcode_generate(const char* ip_address)
{
	srand(time(NULL));
	char authcode[14];
	for (int i = 0; i < 13; ++i)
	{
		char c;
		int r = rand();
		if (i > 9)
		{
			r = r % strlen(ip_address);
			c = ip_address[r];
		}
		else if (i % 5 == 0)
		{
			r = r % strlen(authcode_params1);
			c = authcode_params1[r];
		}
		else if (i % 5 == 1)
		{
			r = r % strlen(authcode_params2);
			c = authcode_params2[r];
		}
		else if (i % 5 == 2)
		{
			r = r % strlen(authcode_params3);
			c = authcode_params3[r];
		}
		else if (i % 5 == 3)
		{
			r = r % strlen(authcode_params4);
			c = authcode_params4[r];
		}
		else if (i % 5 == 4)
		{
			r = r % strlen(authcode_params5);
			c = authcode_params5[r];
		}
		authcode[i] = c;
	}
	return authcode;
}

int __ng_authcode_is_valid(char* ip_address, char* authcode)
{
	for (int i = 0; i < strlen(authcode); ++i)
	{
		int r, found = -1;
		char c = authcode[i];
		if (i > 9)
		{
			for (int j = 0; j < strlen(ip_address); ++j)
			{
				if (ip_address[j] == c)
				{
					found = 0;
					break;
				}
			}
		}
		else if (i % 5 == 0)
		{
			for (int j = 0; j < strlen(authcode_params1); ++j)
			{
				if (authcode_params1[j] == c)
				{
					found = 0;
					break;
				}
			}
		}
		else if (i % 5 == 1)
		{
			for (int j = 0; j < strlen(authcode_params2); ++j)
			{
				if (authcode_params2[j] == c)
				{
					found = 0;
					break;
				}
			}
		}
		else if (i % 5 == 2)
		{
			for (int j = 0; j < strlen(authcode_params3); ++j)
			{
				if (authcode_params3[j] == c)
				{
					found = 0;
					break;
				}
			}
		}
		else if (i % 5 == 3)
		{
			for (int j = 0; j < strlen(authcode_params4); ++j)
			{
				if (authcode_params4[j] == c)
				{
					found = 0;
					break;
				}
			}
		}
		else if (i % 5 == 4)
		{
			for (int j = 0; j < strlen(authcode_params5); ++j)
			{
				if (authcode_params5[j] == c)
				{
					found = 0;
					break;
				}
			}
		}
		if (found == -1)
		{
			return -1;
		}
	}
	return 0;
}