LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:=numbaseball
LOCAL_PRELINK_MODULE:=false
LOCAL_SRC_FILES:=numbaseball.c
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)