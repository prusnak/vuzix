#ifndef __VUZIX_H_
#define __VUZIX_H_ 1

void vuzix_open(const char *filename);
void vuzix_close();
void vuzix_read_raw(float acc[3], float mag[3]);
void vuzix_read(float *pitch, float *roll, float *yaw);

#endif
