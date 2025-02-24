/*
 * Add any #include statements pointing to headers needed by this process.
 * Typical headers would be
 *     eamxx_$PROCESS_process_interface.hpp  (the eamxx interface header file for this process)
 *     physics/share/physics_constants.hpp   (a header storing all physical constants in eamxx)
 *     $PROCESS.hpp                          (a header file for the process itself)
 */

namespace scream
{

/*-----------------------------------------------------------------------------------------------
 * The Constructor for this interface
 *
 * Inputs:
 *     comm - an EKAT communication group
 *     params - a parameter list of options for the process.
 */

AP_TEMPLATE::AP_TEMPLATE (const ekat::Comm& comm, const ekat::ParameterList& params)
 : AtmosphereProcess(comm,params)
{
  // The 'params' variable will hold all runtime options.  
  // Note that `params.get<X>("Y") is the syntax to get the parameter labeled "Y" from the list
  // which is of type X.  
  //
  // ex:
  // m_Y = params.get<X>("Y");
}

/*-----------------------------------------------------------------------------------------------
 * set_grids(grids_manager)
 *
 * set_grids establishes which grid this process will be run on.  
 * It is also where all fields (variables) that the process will need access to.
 *
 * Inputs:
 *     grids_manager - a grids manager object which stores all grids used in this simulation
 */
void AP_TEMPLATE::
set_grids (const std::shared_ptr<const GridsManager> grids_manager)
{
  // Some typical namespaces used in set_grids,
  //   using namespace ShortFieldTagsNames;
  //   using PC = scream::physics::Constants<Real>;

  // Specify which grid this process will act upon, typical options are "Dynamics" or "Physics".
  auto m_grid = grids_manager->get_grid("Physics");

  // When declaring a field for use we need:
  //
  // to set the units.  These can be found from ekat::units::Units
  // For example,
  //   constexpr auto nondim = ekat::units::Units::nondimensional();
  // 
  // to set the layout, which is how the field is configured in space
  // For example,
  //   const auto layout = m_grid->get_3d_scalar_layout(true);

  // We can now add all fields used by this process and designate how they will be used,
  // The format for declaring a field is:
  //   add_field<MODE>(NAME,LAYOUT,UNITS,GRID_NAME);
  // where,
  //   MODE is either "Required", "Computed" or "Updated".
  //     "Required" = input only  (process cannot change this field)
  //     "Computed" = output only (field doesn't have to exist, process will set values for this field)
  //     "Updated"  = input and output (process is able to update field which should already exist)
  //   NAME is the name of the field as a string
  //   LAYOUT is the physical layout of the field on the grid, e.g. 2D, 3D.  See layout example above.
  //   UNITS are the units of the field, see units example above
  //   GRID_NAME is the name of the grid this process is run on, typically "m_grid->name()"
  //
  //   *Note, there is a special add_field call for tracers which has a different signature.  The format is,
  //     add_tracer<MODE>(NAME,GRID,UNITS)
  //   where,
  //     NAME and UNITS are as above
  //     GRID is the actual grid, typically "m_grid"

}

/*-----------------------------------------------------------------------------------------------
 * intialize_impl(run_type)
 *
 * called once for each process at initialization of EAMxx.  This impl can be defined with any
 * actions or functions that are needed at initialization. 
 *
 * Inputs:
 *     run_type - an enum which describes the run type.  Initial or Restart
 *
 * can also be empty
 */
void AP_TEMPLATE::initialize_impl (const RunType /* run_type */)
{
  // NOTE: run_type tells us if this is an initial or restarted run,
}

/*-----------------------------------------------------------------------------------------------
 * run_impl(dt)
 *
 * The main run call for the process.  This is where most of the interface to the underlying process
 * takes place.  This impl is called every timestep. 
 *
 * Inputs:
 *     dt - the timestep for this run step.
 */
void AP_TEMPLATE::run_impl (const double dt)
{
  // Typically here the developer would retrieve data from fields,
  // For example,
  //   const auto X = get_input_field(NAME_X);
  //   const auto Y = get_output_field(NAME_Y);

  // Another typical step is to issue a call to the underlying process code.

}

/*-----------------------------------------------------------------------------------------------
 * finalize_impl()
 *
 * Called at the end of the simulation, handles all finalization of the process.
 *
 * In most cases this is left blank, as EAMxx takes care of most finalization steps.
 * But just in case a process has specific needs the option is available.
 */
void AP_TEMPLATE::finalize_impl ()
{
  // Usually blank
}
/*-----------------------------------------------------------------------------------------------*/

} // namespace scream
