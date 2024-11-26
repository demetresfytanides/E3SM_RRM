#include "scream_rrtmgp_interface.hpp"
#include "physics/share/physics_constants.hpp"

namespace scream {

void init_kls ()
{
#ifdef RRTMGP_ENABLE_KOKKOS
  // Initialize kokkos
  if(!Kokkos::is_initialized()) { Kokkos::initialize(); }
#endif
}

void finalize_kls()
{
#ifdef RRTMGP_ENABLE_KOKKOS
  //Kokkos::finalize(); We do the kokkos finalization elsewhere
#endif
}

}  // namespace scream
