#ifndef EAMXX_ATMOSPHERE_PYTHON_PROCESS_HPP
#define EAMXX_ATMOSPHERE_PYTHON_PROCESS_HPP

#include "share/atm_process/atmosphere_process.hpp"

#include <nanobind/nanobind.h>

namespace scream
{

class AtmospherePythonProcess : public AtmosphereProcess
{
public:
  template<typename T>
  using strmap_t = std::map<std::string,T>;
  template<typename Framework>
  using nb_field = nanobind::ndarray<Framework>

  using numpy = nanobind::numpy;
  using torch = nanobind::torch;
  using cupy  = nanobind::cupy;


  AtmospherePythonProcess (const ekat::Comm& comm, const ekat::ParameterList& params)
   : AtmosphereProcess(comm,params)
  {
    // Nothing to do here
  }

protected:
  void set_required_field_impl (const Field& f) override {
    set_field<true>(m_numpy_fields_in,f);
    set_field<true>(m_torch_fields_in,f);
    set_field<true>(m_cupy_fields_in,f);
  }
  void set_computed_field_impl (const Field& f) override {
    set_field<false>(m_numpy_fields_out,f);
    set_field<false>(m_torch_fields_out,f);
    set_field<false>(m_cupy_fields_out,f);
  }

  template<bool ReadOnly,typename Framework>
  void set_field (strmap_t<nb_field<Framework>>& map,const Field& f)
  {
    using void_ptr = typename std::conditional<ReadOnly,const void*,void*>::type;

    const auto& fh  = f.get_header();
    const auto& fid = fh.get_identifier();

    // Get array shape and strides (nb requires size_t for shape and int64_t for strides)
    // NOTE: since the field may be padded or a subfield, so strides do not necessarily
    //       match the field dims. Also, the strides must be grabbed from the
    //       actual view, since the layout doesn't know them.
    std::vector<int64_t> strides;
    std::vector<size_t>  shape;
    for (auto d : fid.get_layout().dims()) {
      shape.push_back(d);
    }

    // Also the ptr must be grabbed from the view, to make sure it points to the 1st
    // element of the field. In case f is a subfield, this is not the same as the
    // internal data pointer of the field.
    nb::dlpack::dtype dt;
    void_ptr* data;
    switch (fid.data_type()) {
      case DataType::IntType:
        dt = nb::dtype<int>();
        get_ptr_and_strides<int>(strides,data,f);
        break;
      case DataType::FloatType:
        dt = nb::dtype<float>();
        get_ptr_and_strides<float>(strides,data,f);
        break;
      case DataType::DoubleType:
        dt = nb::dtype<double>();
        get_ptr_and_strides<double>(strides,data,f);
        break;
      default:
        EKAT_ERROR_MSG ("Unrecognized/unsupported data type.\n");
    }

    // NOTE: you MUST set the parent handle, or else you won't have view semantic
    auto this_obj = nb::cast(this);
    return nb_field<Framework>(data, shape_t, shape, nb::handle(this_obj), strides.data(), dt);
  }

  template<typename T, typename Ptr>
  void get_ptr_and_strides (std::vector<int64_t>& strides, Ptr& p, const Field& f)
  {
    constexpr bool is_const = std::is_const<typename std::remove_pointer<T>::type>::value;
    using value_type = typename std::conditional<is_const,const T, T>::type;

    const auto& fl  = f.get_header().get_identifier().get_layout();
    strides.resize(fl.rank());
    switch (fl.rank()) {
      case 1:
        {
          auto v = f.get_view<value_type*>();
          strides[0] = v.stride_0();
          p = v.data();
          break;
        }
      case 2:
        {
          auto v = f.get_view<value_type**>();
          strides[0] = v.stride_0();
          strides[1] = v.stride_1();
          p = v.data();
          break;
        }
        break;
      case 3:
        {
          auto v = f.get_view<value_type***>();
          strides[0] = v.stride_0();
          strides[1] = v.stride_1();
          strides[2] = v.stride_2();
          p = v.data();
          break;
        }
        break;
      default:
        EKAT_ERROR_MSG ("Error! Rank " + std::to_string(fl.rank()) + " not supported.\n");
    }
  }

  strmap_t<nb_field<numpy>> m_numpy_fields_in;
  strmap_t<nb_field<torch>> m_torch_fields_in;
  strmap_t<nb_field<cupy>>  m_cupy_fields_in;

  strmap_t<nb_field<numpy>> m_numpy_fields_out;
  strmap_t<nb_field<torch>> m_torch_fields_out;
  strmap_t<nb_field<cupy>>  m_cupy_fields_out;
};

} // namespace scream

#endif // EAMXX_ATMOSPHERE_PYTHON_PROCESS_HPP
