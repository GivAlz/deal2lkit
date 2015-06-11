#ifndef __dealii_sak_sak_data_h
#define __dealii_sak_sak_data_h

#include <deal.II/base/config.h>
#include <deal.II/base/exceptions.h>
#include <deal.II/base/subscriptor.h>

#include <boost/any.hpp>
#include <vector>
#include <algorithm>
#include <typeinfo>

#include <unordered_map>

using namespace dealii;

/**
 * Store any amount of any type of data accessible by an identifier string.
 *
 * It is a std::unordered_map<std::string, boost::any>
 *
 * Unordered maps are associative containers that store elements formed by
 * the combination of a key value and a mapped value, and which allows for
 * fast retrieval of individual elements based on their keys.
 *
 * unorderd_map containers are faster than std::map containers to access individual
 * elements by their key (i.e., the string)
 *
 * A typical usage of this class is the following
 * @begin code
 * SAKData data;
 * const unsigned int n_q = 5;
 * std::vector<double> v_double(n_q);
 * ...
 * data.add_copy<std::vector<double> >(v_double, "double_copy");
 * ...
 * auto &vd = data.get<std::vector<double> >("double_copy");

 * std::vector<int> v_int(n_q);
 * data.add_ref<std::vector<int> >(v_int, "int_ref");
 * v_int[0] = 7.0;
 * auto &vi = data.get<std::vector<int> >("int_ref");
 *
 * @end
 */

class SAKData : public Subscriptor
{
public:

  /**
   * @brief Add a copy of an object
   *
   * Add a copy of an object. The copied object is owned by the class.
   *
   */
  template <typename type>
  void add_copy (const type &entry, const std::string &name);

  /**
   * @brief Add a reference to an object
   *
   * Add a reference to an external object. The object is not owned by the class
   *
   */
  template <typename type>
  void add_ref (type &entry, const std::string &name);

  /**
   * @brief Access to stored data object by name.
   *
   * Find the object with given name, try to convert it to <tt>type</tt> and
   * return it. This function throws an exception if either the name does not
   * exist or if the conversion fails.
   *
   */
  template <typename type>
  type &get (const std::string &name);

  /**
   * @brief Read-only access to stored data object by name.
   *
   * Find the object with given name, try to convert it to <tt>type</tt> and
   * return it. This function throws an exception if either the name does not
   * exist or if the conversion fails.
   *
   */
  template <typename type>
  const type &get (const std::string &name) const;

  /// An entry with this name does not exist in the SAKData object.
  DeclException1(ExcNameNotFound, std::string,
                 << "No entry with the name " << arg1 << " exists.");

  /// The requested type and the stored type are different
  DeclException2(ExcTypeMismatch,
                 char *, char *,
                 << "The requested type " << arg1
                 << " and the stored type " << arg2
                 << " must coincide");


private:
  std::unordered_map<std::string, boost::any> mydata;
}; // end class

template <typename type>
void SAKData::add_copy (const type &entry, const std::string &name)
{
  mydata[name] = entry;
}

template <typename type>
void SAKData::add_ref (type &entry, const std::string &name)
{
  type *ptr = &entry;
  mydata[name] = ptr;
}

template <typename type>
type &SAKData::get(const std::string &name)
{
  Assert( mydata.find(name) != mydata.end(),
          ExcNameNotFound(name));

  type *p=NULL;

  if (mydata[name].type() == typeid(type *) )
    {
      p = boost::any_cast<type *>(mydata[name]);
    }
  else if (mydata[name].type() == typeid(type))
    {
      p = boost::any_cast<type>(&mydata[name]);
    }
  else
    {
      Assert(false,
             ExcTypeMismatch(typeid(type).name(),mydata[name].type().name()));
    }

  return *p;
}

template <typename type>
const type &SAKData::get(const std::string &name) const
{
  Assert( mydata.find(name) != mydata.end(),
          ExcNameNotFound(name));


  if (mydata.at(name).type() == typeid(type *) )
    {
      const type *p = boost::any_cast<type *>(mydata.at(name));
      return *p;
    }
  else if (mydata.at(name).type() == typeid(type))
    {
      const type *p = boost::any_cast<type>(&mydata.at(name));
      return *p;
    }
  else
    {
      Assert(false,
             ExcTypeMismatch(typeid(type).name(),mydata.at(name).type().name()));
      const type *p=NULL;
      return *p;
    }
}

#endif