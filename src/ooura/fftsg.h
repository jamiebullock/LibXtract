/* FFT functions */
void cdft(int n, int isgn, double *a, int *ip, double *w);
void rdft(int n, int isgn, double *a, int *ip, double *w);
void ddct(int n, int isgn, double *a, int *ip, double *w);
void ddst(int n, int isgn, double *a, int *ip, double *w);
void dfct(int n, double *a, double *t, int *ip, double *w);
void dfst(int n, double *a, double *t, int *ip, double *w);

/* Auxiliary functions */
void makewt(int nw, int *ip, double *w);
void bitrv2(int n, int *ip, double *a);
void bitrv2conj(int n, int *ip, double *a);
void cftfsub(int n, double *a, double *w);
void cftbsub(int n, double *a, double *w);
void makect(int nc, int *ip, double *c);
void rftfsub(int n, double *a, int nc, double *c);
void rftbsub(int n, double *a, int nc, double *c);
void dctsub(int n, double *a, int nc, double *c);
void dstsub(int n, double *a, int nc, double *c);
void cft1st(int n, double *a, double *w);
void cftmdl(int n, int l, double *a, double *w);
