/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQLX_COLLATIONS_H
#define MYSQLX_COLLATIONS_H

#include "common.h"

/*
  Import Lists of built-in character sets and collations supported by MySQL
  Server.
*/

#include "mysql_charsets.h"
#include "mysql_collations.h"


namespace mysqlx {

/*
  Enumeration of known character sets. For each character set CS listed
  in CDK_CS_LIST() macro, there is CharacterSet::CS constant in this
  enumeration.
*/


enum class CharacterSet : unsigned short
{
  #undef CS
  #define CS_ENUM(CS)  CS,
  CDK_CS_LIST(CS_ENUM)
};


#define CS_NAME_SWITCH(CS)  case CharacterSet::CS: return #CS;

/**
  Returns name of a character set given by its id.

  @ingroup devapi_res
*/

inline
const char* characterSetName(CharacterSet id)
{
  switch (id)
  {
    CDK_CS_LIST(CS_NAME_SWITCH)
  default:
    THROW("Unknown character set id");
  }
}


/**
  Structure that provides information about character set collation.

  For each known collation COLL over character set CS, there is static object
  `Collation<CS>::%COLL` of type `CollationInfo` which describes
  collation COLL. For example `Collation<CharacterSet::latin1>::%swedish_ci` is
  an `CollationInfo` object which describes the `swedish_ci` collation over
  `latin1` character set.

  @ingroup devapi_res
*/

struct CollationInfo
{
  /// Numeric collation id, as used by MySQL server.

  unsigned id() const { return m_id; }

  /// String name of a collation, such as "latin1_generic_ci".

  const char *getName() const { return m_name; }

  /// Id of the character set of this collation.

  CharacterSet getCharacterSet() const { return m_cs; }

  /**
    Returns true if given collation is case sensitive. Binary
    collations are assumed to be case sensitive.
  */

  bool  isCaseSensitive() const { return m_case != case_ci; }

  /// Returns true if this is binary collation.

  bool  isBinary() const { return m_case == case_bin; }


  bool  operator==(const CollationInfo &other) const
  {
    return m_id == other.m_id;
  }

  bool  operator!=(const CollationInfo &other) const
  {
    return !operator==(other);
  }

private:

  enum coll_case { case_bin, case_ci, case_cs };

  CharacterSet m_cs;
  unsigned m_id;
  coll_case m_case;
  const char *m_name;

public:

  struct Access;
  friend Access;
};


template <CharacterSet CS> struct Collation;


/*
  The Collation<CS>::COLL constants are generated from information provided by
  COLLATION_XXX() macros.Each line X(CS, ID, COLL, CASE) in the expansion of
  a COLLATION_XXX() macro declares collation with name COLL for character set
  CS. ID is the MySQL id number for the collation. CASE is one of ci, cs or bin
  and indicates whether it is case insensitive, sensitive or binary collation,
  respectively.
*/

/*
  Generate declarations of Collation<CS>::COLL constants using
  COLLATINS_XXX() macros. The list CS_LIST() is processed using
  COLL_DECL() macro, which for each character set CS declares
  specialization Collation<CS> of the Collation<> template and
  declares collation constants within this specialization.

  The collation constant declarations are generated by processing
  COLLATIONS_CS() list with COLL_CONST() macro. The name of each
  constant is generated by COLL_CONST_NAME() macro. The convention
  used is that case insensitive and case sensitive collations get
  a _ci or _cs suffix, respectively, while binary collation constant
  have no suffix to the collation name.

  Collation constants declared here are defined in file result.cc.
*/

#define COLL_DECL(CS) \
template<> struct Collation<CharacterSet::CS> \
{ COLLATIONS_##CS(COLL_CONST) }; \

#define COLL_CONST(CS,ID,COLL,CASE) \
static PUBLIC_API const CollationInfo COLL_CONST_NAME(COLL,CASE);

#define COLL_CONST_NAME(COLL,CASE) COLL_CONST_NAME_##CASE(COLL)

#define COLL_CONST_NAME_bin(COLL) COLL
#define COLL_CONST_NAME_ci(COLL)  COLL##_ci
#define COLL_CONST_NAME_cs(COLL)  COLL##_cs

CDK_CS_LIST(COLL_DECL)

}  // mysqlx

#endif
