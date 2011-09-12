#ifndef MEMBERS_HPP
#define MEMBERS_HPP

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>

#include <boost/graph/adj_list_serialize.hpp>

#include <boost/serialization/export.hpp>

#define INTERNAL_SICONOS_SERIALIZATION_NVP(object,member)               \
  ::boost::serialization::make_nvp(BOOST_PP_STRINGIZE(member), object.member)

/* serialization is not splitted */
#define INTERNAL_SICONOS_IO_SERIALIZE(r,o,m) \
  ar & INTERNAL_SICONOS_SERIALIZATION_NVP(o,m);

#define INTERNAL_SICONOS_IO_SERIALIZE_BASE(r,o,b)                       \
  ar & ::boost::serialization::make_nvp(                                \
    BOOST_PP_STRINGIZE(b),                                              \
    ::boost::serialization::base_object<b>(o) );                        \
 
/** base class members registration
    \param class name
    \param members sequence (as a boost sequence (member1)(member2) ...)
*/
#define SICONOS_IO_REGISTER(CLASS,MEMBERS)                              \
  template<class Archive>                                               \
  void siconos_io(Archive & ar, CLASS & o, const unsigned int version) \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE, o, MEMBERS);   \
  };                                                                    \
  namespace boost { namespace serialization {                           \
      template<class Archive>                                           \
      void serialize(Archive & ar, CLASS & o, const unsigned int version) \
      {                                                                 \
        siconos_io(ar,o,version);                                       \
      };                                                                \
    }}

/** derived class members registration
    \param derived class name
    \param base class name
    \param members sequence (as a boost sequence (member1)(member2) ...)
*/
#define SICONOS_IO_REGISTER_WITH_BASE(CLASS,BASE,MEMBERS)               \
  template<class Archive>                                               \
  void siconos_io(Archive & ar, CLASS & o, const unsigned int version)  \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE, o, MEMBERS);   \
    ar &  ::boost::serialization::make_nvp(                             \
      BOOST_PP_STRINGIZE(BASE),                                         \
      ::boost::serialization::base_object<BASE>(o) );                   \
  };                                                                    \
  namespace boost { namespace serialization {                           \
      template<class Archive>                                           \
      void serialize(Archive & ar, CLASS & o, const unsigned int version) \
      {                                                                 \
        siconos_io(ar,o,version);                                       \
      };                                                                \
    }}

/** derived class with multiple inheritance registration
    \param derived class name
    \param base class name
    \param members sequence (as a boost sequence (member1)(member2) ...)
*/
#define SICONOS_IO_REGISTER_WITH_BASES(CLASS,BASES,MEMBERS)             \
  template<class Archive>                                               \
  void siconos_io(Archive & ar, CLASS & o, const unsigned int version)  \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE, o, MEMBERS);   \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE_BASE, o, BASES); \
  };                                                                    \
  namespace boost { namespace serialization {                           \
      template<class Archive>                                           \
      void serialize(Archive & ar, CLASS & o, const unsigned int version) \
      {                                                                 \
        siconos_io(ar,o,version);                                       \
      };                                                                \
    }}


#endif
