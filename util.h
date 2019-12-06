#ifndef _UTIL_H_
#define _UTIL_H_

unsigned int setup_shader(const char *fname);
void set_uniform1f(unsigned int prog, const char *name, float val);
void set_uniform2f(unsigned int prog, const char *name, float v1, float v2);
void set_uniform4f(unsigned int prog, const char *name, float v1, float v2, float v3, float v4);
void set_uniform1i(unsigned int prog, const char *name, int val);
void set_uniform1fv(unsigned int prog, const char *name, int n, float *);
void set_uniform3fv(unsigned int prog, const char *name, int n, float *);

#endif	/* _UTIL_H_ */

