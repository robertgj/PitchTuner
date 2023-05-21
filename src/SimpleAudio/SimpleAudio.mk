include src/SimpleAudio/alsa/alsa.mk

# Programs in this directory

saSample_PROGRAMS= \
saSampleStdInt_test \
saSample_test \
saSampleTo_test \
saSampleTest_test \
saSampleTestTo_test

libSimpleAudio_PROGRAMS= \
saCircBuff_test \
saInputDeviceInfo_test \
saInputFileSelector_test \
saOutputFileSelector_test \
saInputStreamFile_test \
saOutputStreamFile_test 

SimpleAudio_PROGRAMS= \
$(saSample_PROGRAMS) \
$(libSimpleAudio_PROGRAMS)

PROGRAMS+=$(SimpleAudio_PROGRAMS)

# Change for oss or win32
VPATH += src/SimpleAudio src/SimpleAudio/alsa

STATIC_LIBRARIES += libSimpleAudio

libSimpleAudio_CXX_SOURCES:=\
  saSample.cc \
  saDeviceInfo.cc \
  saDeviceInfoDetail.cc \
  saInputDevice.cc \
  saInputFileAscii.cc \
  saInputFileWav.cc \
  saInputFileSelector.cc \
  saInputStream.cc \
  saOutputDevice.cc \
  saOutputStream.cc \
  saOutputFileWav.cc \
  saOutputFileSelector.cc \
  saWavHeader.cc

saSampleStdInt_test_CXX_SOURCES := saSampleStdInt_test.cc 

saSample_test_CXX_SOURCES := saSample_test.cc saSample.cc

saSampleTo_test_CXX_SOURCES := saSampleTo_test.cc saSample.cc

saSampleTest_test_CXX_SOURCES := saSampleTest_test.cc saSampleTest.cc

saSampleTestTo_test_CXX_SOURCES := saSampleTestTo_test.cc saSampleTest.cc

saCircBuff_test_CXX_SOURCES := saCircBuff_test.cc 

saInputDeviceInfo_test_CXX_SOURCES := saInputDeviceInfo_test.cc 

saInputFileSelector_test_CXX_SOURCES := saInputFileSelector_test.cc 

saOutputFileSelector_test_CXX_SOURCES := saOutputFileSelector_test.cc

saInputStreamFile_test_CXX_SOURCES := saInputStreamFile_test.cc

saOutputStreamFile_test_CXX_SOURCES := saOutputStreamFile_test.cc 

$(call add_extra_CXXFLAGS_macro, $(libSimpleAudio_CXX_SOURCES), \
-Isrc/SimpleAudio -Isrc/SimpleAudio/include)

$(foreach test, $(SimpleAudio_PROGRAMS), \
$(eval $(call add_extra_CXXFLAGS_macro, $(${test}_CXX_SOURCES), \
-Isrc/SimpleAudio -Isrc/SimpleAudio/include)))

$(foreach test, $(SimpleAudio_PROGRAMS), \
$(eval $(call add_extra_LIBS_macro, ${test}, -lstdc++)))

$(foreach test, $(libSimpleAudio_PROGRAMS), \
$(eval $(call add_extra_LIBS_macro, ${test}, -lasound)))

$(foreach test, $(libSimpleAudio_PROGRAMS), \
$(eval ${test}_STATIC_LIBRARIES := libSimpleAudio.a))

