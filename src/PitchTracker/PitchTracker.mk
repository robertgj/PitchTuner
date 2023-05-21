PitchTracker_PROGRAMS = \
PitchTracker_test ButterworthFilter_test AutomaticGainControl_test

PROGRAMS += $(PitchTracker_PROGRAMS)

VPATH += src/PitchTracker

STATIC_LIBRARIES += libPitchTracker

libPitchTracker_CXX_SOURCES := PitchTracker.cc PreProcessor.cc
$(call add_extra_CXXFLAGS_macro, $(libPitchTracker_CXX_SOURCES), \
-Isrc/SimpleAudio -Isrc/SimpleAudio/include)

PitchTracker_test_CXX_SOURCES := PitchTracker_test.cc
$(call add_extra_CXXFLAGS_macro, $(PitchTracker_test_CXX_SOURCES), \
-Isrc/SimpleAudio -Isrc/SimpleAudio/include)
PitchTracker_test_STATIC_LIBRARIES := libPitchTracker.a libSimpleAudio.a

ButterworthFilter_test_CXX_SOURCES := ButterworthFilter_test.cc
$(call add_extra_CXXFLAGS_macro, $(ButterworthFilter_test_CXX_SOURCES), \
-Isrc/SimpleAudio -Isrc/SimpleAudio/include)
ButterworthFilter_test_STATIC_LIBRARIES := libSimpleAudio.a

AutomaticGainControl_test_CXX_SOURCES := AutomaticGainControl_test.cc
$(call add_extra_CXXFLAGS_macro, $(AutomaticGainControl_test_CXX_SOURCES), \
-Isrc/SimpleAudio -Isrc/SimpleAudio/include)
AutomaticGainControl_test_STATIC_LIBRARIES := libSimpleAudio.a

$(call add_extra_LIBS_macro, $(PitchTracker_PROGRAMS), -lasound -lstdc++)
