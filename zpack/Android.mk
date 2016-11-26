LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := pack_static

LOCAL_MODULE_FILENAME := libpack

LOCAL_SRC_FILES := WriteCompressFile.cpp  \
zlib/adler32.c  \
zlib/compress.c  \
zlib/crc32.c  \
zlib/deflate.c  \
zlib/infback.c  \
zlib/inffast.c  \
zlib/inflate.c  \
zlib/inftrees.c  \
zlib/trees.c  \
zlib/uncompr.c  \
zlib/zutil.c  \
zpCompressedFile.cpp  \
zpack.cpp  \
zpFile.cpp  \
zpPackage.cpp  \
ZpWriteCompressFile.cpp  \
zpWriteFile.cpp

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                           $(LOCAL_PATH)/zlib
                           
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
														
LOCAL_CFLAGS += -DCOCOS2D_DEBUG
                    
include $(BUILD_STATIC_LIBRARY)

$(call import-module,cocos2dx)