/// \file saOutputFileWav.h

#ifndef __SAOUTPUTFILEWAV_H__
#define __SAOUTPUTFILEWAV_H__

namespace SimpleAudio
{
  /// \brief Open output WAV file
  /// \param fileName file name
  /// \param frameRate frame rate (sample rate)
  /// \param channels channels (samples) per frame
  /// \return A pointer to an output WAV file to write to
  saOutputSink* saOutputSinkOpenFileWav(const std::string& fileName,
                                        const std::size_t frameRate,
                                        const std::size_t channels=1);
}

#endif
