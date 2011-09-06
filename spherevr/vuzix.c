#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int fd = 0;

struct {
    char b0;
    char b1;
    char b2;
    short ax;
    short ay;
    short az;
    short mx;
    short my;
    short mz;
    char b15;
} __attribute__ ((__packed__)) packet;

short aminx = -1, amaxx = 1;
short aminy = -1, amaxy = 1;
short aminz = -1, amaxz = 1;
short mminx = -1, mmaxx = 1;
short mminy = -1, mmaxy = 1;
short mminz = -1, mmaxz = 1;
#define ROUND_SIZE 27
float rax[ROUND_SIZE], ray[ROUND_SIZE], raz[ROUND_SIZE];
float rmx[ROUND_SIZE], rmy[ROUND_SIZE], rmz[ROUND_SIZE];
static int ridx;


void vuzix_open(const char *filename)
{
    fd = open(filename, O_RDONLY);
    memset(rax, 0, sizeof(rax));
    memset(ray, 0, sizeof(ray));
    memset(raz, 0, sizeof(raz));
    memset(rmx, 0, sizeof(rmx));
    memset(rmy, 0, sizeof(rmy));
    memset(rmz, 0, sizeof(rmz));
    ridx = 0;
}

void vuzix_close()
{
    close(fd);
}

int cmp(const void *p1, const void *p2)
{
    return *(short *)p2 - *(short *)p1;
}

void vuzix_read_raw(float acc[3], float mag[3])
{
    float tmp[ROUND_SIZE];
    if (!fd) return;
    if (sizeof(packet) != read(fd, &packet, sizeof(packet))) return;
    // set new limits
    if (aminx > packet.ax) aminx = packet.ax;
    if (amaxx < packet.ax) amaxx = packet.ax;
    if (aminy > packet.ay) aminy = packet.ay;
    if (amaxy < packet.ay) amaxy = packet.ay;
    if (aminz > packet.az) aminz = packet.az;
    if (amaxz < packet.az) amaxz = packet.az;
    if (mminx > packet.mx) mminx = packet.mx;
    if (mmaxx < packet.mx) mmaxx = packet.mx;
    if (mminy > packet.my) mminy = packet.my;
    if (mmaxy < packet.my) mmaxy = packet.my;
    if (mminz > packet.mz) mminz = packet.mz;
    if (mmaxz < packet.mz) mmaxz = packet.mz;
    // normalize
    rax[ridx] = ((1.0f * packet.ax - aminx - (amaxx - aminx) / 2) / (amaxx - aminx) * 2);
    ray[ridx] = ((1.0f * packet.ay - aminy - (amaxy - aminy) / 2) / (amaxy - aminy) * 2);
    raz[ridx] = ((1.0f * packet.az - aminz - (amaxz - aminz) / 2) / (amaxz - aminz) * 2);

    rmx[ridx] = ((1.0f * packet.mx - mminx - (mmaxx - mminx) / 2) / (mmaxx - mminx) * 2);
    rmy[ridx] = ((1.0f * packet.my - mminy - (mmaxy - mminy) / 2) / (mmaxy - mminy) * 2);
    rmz[ridx] = ((1.0f * packet.mz - mminz - (mmaxz - mminz) / 2) / (mmaxz - mminz) * 2);

    ridx = (ridx + 1) % ROUND_SIZE;
    // find medians
    memcpy(tmp, rax, sizeof(rax));
    qsort(tmp, ROUND_SIZE, sizeof(tmp[0]), cmp);
    acc[0] = tmp[ROUND_SIZE/2];
    memcpy(tmp, ray, sizeof(ray));
    qsort(tmp, ROUND_SIZE, sizeof(tmp[0]), cmp);
    acc[1] = tmp[ROUND_SIZE/2];
    memcpy(tmp, raz, sizeof(raz));
    qsort(tmp, ROUND_SIZE, sizeof(tmp[0]), cmp);
    acc[2] = tmp[ROUND_SIZE/2];
    memcpy(tmp, rmx, sizeof(rmx));
    qsort(tmp, ROUND_SIZE, sizeof(tmp[0]), cmp);
    mag[0] = tmp[ROUND_SIZE/2];
    memcpy(tmp, rmy, sizeof(rmy));
    qsort(tmp, ROUND_SIZE, sizeof(tmp[0]), cmp);
    mag[1] = tmp[ROUND_SIZE/2];
    memcpy(tmp, rmz, sizeof(rmz));
    qsort(tmp, ROUND_SIZE, sizeof(tmp[0]), cmp);
    mag[2] = tmp[ROUND_SIZE/2];
}

void vuzix_read(float *pitch, float *roll, float *yaw)
{
    const float scale = 1.0f;
    float acc[3] = { 0.0f, 0.0f, 1.0f };
    float mag[3] = { 0.0f, 1.0f, 0.0f };

    vuzix_read_raw(acc, mag);

    float acc_roll  = atan2(acc[0], sqrt(acc[1] * acc[1] + acc[2] * acc[2])) * 180 / M_PI;
    float acc_pitch = atan2(acc[1], sqrt(acc[0] * acc[0] + acc[2] * acc[2])) * 180 / M_PI;

    float xh = mag[0] * cos((-acc_pitch) / 180.0 * M_PI) + mag[1] * sin(acc_roll / 180.0 * M_PI) * sin((-acc_pitch) / 180.0 * M_PI) - mag[2] * sin(-acc_pitch / 180.0 * M_PI) * cos((-acc_roll) / 180.0 * M_PI);
    float yh = mag[1] * cos(acc_roll / 180.0 * M_PI) + mag[2] * sin(acc_roll / 180.0 * M_PI);

    float len = sqrt(xh * xh + yh * yh);
    xh = xh / len;
    yh = yh / len;

    *pitch = scale * acc_pitch;
    *roll = scale * acc_roll;
    *yaw = -scale * atan2(xh, yh) * 180 / M_PI;
}
