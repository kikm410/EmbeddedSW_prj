#include <jni.h>
#include "android/log.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define LOG_TAG "MyTag"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)


JNIEXPORT jint JNICALL Java_com_example_numbaseball_MainActivity_openDevice(JNIEnv *env, jobject this){
	int fd = open("/dev/numbaseball", O_RDWR);

	if (fd == -1) {
		LOGV("Error opening device file\n");
		return -1;
	}

	return fd;
}


JNIEXPORT void JNICALL Java_com_example_numbaseball_MainActivity_startNumbaseball(JNIEnv *env, jobject this, jint fd){
	write(fd, NULL, 0);

	return;
}



JNIEXPORT void JNICALL Java_com_example_numbaseball_MainActivity_closeDevice(JNIEnv *env, jobject this, jint fd){
	close(fd);

	return;
}
