/// \file saInputFileWav.h
///

#include "saInputFileSelector.h"

#ifndef __SAINPUTFILEWAV_H__
#define __SAINPUTFILEWAV_H__

namespace SimpleAudio
{
  /// \brief Open input WAV file
  /// \param fileName file name
  /// \return A pointer to an saInputSource to read from
  saInputSource* saInputSourceOpenFileWav(const std::string& fileName);
}

#endif
