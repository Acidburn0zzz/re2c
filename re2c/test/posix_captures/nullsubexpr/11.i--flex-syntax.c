/* Generated by re2c */

{
	YYCTYPE yych;
	if (YYLIMIT <= YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch (yych) {
	case 'a':
		yyt1 = yyt3 = YYCURSOR;
		goto yy3;
	case 'z':
		yyt1 = yyt3 = yyt5 = YYCURSOR;
		goto yy5;
	default:
		yyt2 = yyt3 = yyt4 = yyt5 = NULL;
		yyt1 = YYCURSOR;
		goto yy2;
	}
yy2:
	{
		const size_t yynmatch = 3;
		const YYCTYPE *yypmatch[yynmatch * 2];
		yypmatch[0] = yyt1;
		yypmatch[3] = yyt2;
		yypmatch[5] = yyt4;
		yypmatch[1] = YYCURSOR;
		yypmatch[2] = yyt3;
		yypmatch[4] = yyt5;
		{}
	}
yy3:
	++YYCURSOR;
	if (YYLIMIT <= YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch (yych) {
	case 'a':
		yyt3 = YYCURSOR;
		goto yy3;
	case 'z':
		yyt3 = yyt5 = YYCURSOR;
		goto yy5;
	default:
		yyt4 = yyt5 = NULL;
		yyt2 = YYCURSOR;
		goto yy2;
	}
yy5:
	++YYCURSOR;
	if (YYLIMIT <= YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch (yych) {
	case 'a':
		yyt3 = YYCURSOR;
		goto yy3;
	case 'z':
		yyt5 = YYCURSOR;
		goto yy5;
	default:
		yyt2 = yyt4 = YYCURSOR;
		goto yy2;
	}
}

re2c: warning: line 5: rule matches empty string [-Wmatch-empty-string]
re2c: warning: line 6: rule matches empty string [-Wmatch-empty-string]
re2c: warning: line 6: unreachable rule (shadowed by rule at line 5) [-Wunreachable-rules]