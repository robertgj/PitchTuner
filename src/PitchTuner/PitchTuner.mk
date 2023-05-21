PitchTuner_PROGRAMS = PitchTuner

PROGRAMS += $(PitchTuner_PROGRAMS)

VPATH += src/PitchTuner

PitchTuner_CXX_SOURCES := PitchTuner.cc PitchTunerFrame.cc \
guiMeter.cc guiOptionsDialog.cc

PitchTuner_STATIC_LIBRARIES := libPitchTracker.a libSimpleAudio.a

$(call add_extra_CXXFLAGS_macro, $(PitchTuner_CXX_SOURCES), \
$(shell wx-config --cxxflags) -Wno-inline -Wno-conversion -Wno-double-promotion \
-Isrc/SimpleAudio -Isrc/SimpleAudio/include -Isrc/PitchTracker -Isrc/PitchTuner)

$(call add_extra_LIBS_macro, $(PitchTuner_PROGRAMS), \
$(shell wx-config --libs) -lasound -lstdc++)
