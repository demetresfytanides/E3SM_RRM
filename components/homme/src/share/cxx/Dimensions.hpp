/********************************************************************************
 * HOMMEXX 1.0: Copyright of Sandia Corporation
 * This software is released under the BSD license
 * See the file 'COPYRIGHT' in the HOMMEXX/src/share/cxx directory
 *******************************************************************************/

#ifndef HOMMEXX_DIMENSIONS_HPP
#define HOMMEXX_DIMENSIONS_HPP

#include <Kokkos_Core.hpp>

#include "Types.hpp"
#include "Config.hpp"

namespace Homme {

static constexpr int VECTOR_SIZE = Scalar::size();

// Until whenever CUDA supports constexpr properly
#ifdef HOMMEXX_ENABLE_GPU

  #ifdef CAM
    #define QSIZE_D PCNST
  #endif

  #define NUM_PHYSICAL_LEV    PLEV
  #define NUM_TIME_LEVELS     3
  #define Q_NUM_TIME_LEVELS   2

  #define NUM_LEV             NUM_PHYSICAL_LEV
  #define NUM_LEV_P           (NUM_LEV + 1)
  #define NUM_INTERFACE_LEV   NUM_LEV_P
#else

  #ifdef CAM
    static constexpr const int QSIZE_D = PCNST;
  #endif

  static constexpr const int NUM_PHYSICAL_LEV = PLEV;
  static constexpr const int NUM_LEV =
      (NUM_PHYSICAL_LEV + VECTOR_SIZE - 1) / VECTOR_SIZE;

  static constexpr const int NUM_INTERFACE_LEV = NUM_PHYSICAL_LEV + 1;
  static constexpr const int NUM_LEV_P =
      (NUM_INTERFACE_LEV + VECTOR_SIZE - 1) / VECTOR_SIZE;

  static constexpr const int NUM_TIME_LEVELS = 3;
  static constexpr const int Q_NUM_TIME_LEVELS = 2;
#endif // GPU_BUILD

template<int PHYSICAL_LENGTH>
struct ColInfo {
private:
  template<int LENGTH>
  struct Helper {
    static constexpr int NumPacks   = (LENGTH + VECTOR_SIZE - 1) / VECTOR_SIZE;
    static constexpr int LastPackLen = LENGTH - (NumPacks-1)*VECTOR_SIZE;

    static constexpr int LastPack    = NumPacks - 1;
    static constexpr int LastPackEnd = LastPackLen - 1;
  };
public:

  static constexpr int NumPacks    = Helper<PHYSICAL_LENGTH>::NumPacks   ;
  static constexpr int LastPack    = Helper<PHYSICAL_LENGTH>::LastPack   ;
  static constexpr int LastPackLen = Helper<PHYSICAL_LENGTH>::LastPackLen;
  static constexpr int LastPackEnd = Helper<PHYSICAL_LENGTH>::LastPackEnd;
};

} // namespace TinMan

#endif // HOMMEXX_DIMENSIONS_HPP
