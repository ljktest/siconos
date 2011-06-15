#ifndef MEMBERS_HPP
#define MEMBERS_HPP

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/mpl/eval_if.hpp>


#define INTERNAL_SICONOS_SERIALIZATION_NVP(class,member)                \
  boost::serialization::make_nvp(BOOST_PP_STRINGIZE(member), class.member)

/* serialization is not splitted */
#define INTERNAL_SICONOS_IO_SERIALIZE(r,d,e) \
  ar & INTERNAL_SICONOS_SERIALIZATION_NVP(d,e);


#define INTERNAL_SICONOS_BOOST_REGISTER(CLASS) \
  namespace boost { namespace serialization {                           \
      struct Load##CLASS                                                \
      {                                                                 \
        typedef Load##CLASS type;                                       \
        template<typename Archive>                                      \
          void function(Archive& ar, CLASS& c, const unsigned int version) \
        { load(ar,c,version); };                                        \
      };                                                                \
      struct Save##CLASS                                                \
      {                                                                 \
        typedef Save##CLASS type;                                       \
        template<typename Archive>                                      \
          void function(Archive& ar, CLASS& c, const unsigned int version) \
        { save(ar,c,version); };                                        \
      };                                                                \
      template<class Archive>                                           \
      void serialize(Archive & ar, CLASS & c, const unsigned int version) \
      {                                                                 \
        typedef typename boost::mpl::eval_if<typename Archive::is_saving, \
          Load##CLASS, Save##CLASS>::type Serializer;                   \
        static Serializer serializer;                                   \
        serializer.function(ar,c,version);};                            \
    }}

/** base class members registration
    \param class name
    \param members sequence (as a boost sequence (member1)(member2) ...)
*/
#define SICONOS_IO_REGISTER(CLASS,MEMBERS)                              \
  template<class Archive>                                               \
  void save(Archive & ar, CLASS & c, const unsigned int version)        \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE, c, MEMBERS);   \
  };                                                                    \
  template<class Archive>                                               \
  void load(Archive & ar, CLASS & c, const unsigned int version)        \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE, c, MEMBERS);   \
  };                                                                    \
  INTERNAL_SICONOS_BOOST_REGISTER(CLASS)



/** derived class members registration
    \param derived class name
    \param base class name
    \param members sequence (as a boost sequence (member1)(member2) ...)
*/
#define SICONOS_IO_REGISTER_WITH_BASE(CLASS,BASE,MEMBERS)               \
  template<class Archive>                                               \
  void save(Archive & ar, CLASS & c, const unsigned int version)        \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE, c, MEMBERS);   \
    ar << boost::serialization::make_nvp(                               \
      BOOST_PP_STRINGIZE(CLASS),                                        \
      boost::serialization::base_object<BASE>(c) );                     \
  };                                                                    \
  template<class Archive>                                               \
  void load(Archive & ar, CLASS & c, const unsigned int version)        \
  {                                                                     \
    BOOST_PP_SEQ_FOR_EACH(INTERNAL_SICONOS_IO_SERIALIZE, c, MEMBERS);   \
    ar >> boost::serialization::make_nvp(                               \
      #CLASS,                                                           \
      boost::serialization::base_object<BASE>(c) );                     \
  };                                                                    \
  INTERNAL_SICONOS_BOOST_REGISTER(CLASS)

#endif
