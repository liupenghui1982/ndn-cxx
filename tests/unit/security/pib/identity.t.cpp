/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "ndn-cxx/security/pib/identity.hpp"
#include "ndn-cxx/security/pib/impl/identity-impl.hpp"
#include "ndn-cxx/security/pib/impl/pib-memory.hpp"
#include "ndn-cxx/security/pib/pib.hpp"

#include "tests/boost-test.hpp"
#include "tests/unit/security/pib/pib-data-fixture.hpp"

namespace ndn {
namespace security {
namespace pib {
namespace tests {

using namespace ndn::security::tests;

BOOST_AUTO_TEST_SUITE(Security)
BOOST_AUTO_TEST_SUITE(Pib)
BOOST_FIXTURE_TEST_SUITE(TestIdentity, PibDataFixture)

BOOST_AUTO_TEST_CASE(ValidityChecking)
{
  Identity id;
  BOOST_CHECK(!id);
  BOOST_CHECK_EQUAL(static_cast<bool>(id), false);

  auto identityImpl = make_shared<detail::IdentityImpl>(id1, make_shared<PibMemory>(), true);
  id = Identity(identityImpl);
  BOOST_CHECK(id);
  BOOST_CHECK_EQUAL(!id, false);
}

// pib::Identity is a wrapper of pib::detail::IdentityImpl.  Since the functionalities of
// IdentityImpl have already been tested in detail/identity-impl.t.cpp, we only test the shared
// property of pib::Identity in this test case.
BOOST_AUTO_TEST_CASE(SharedImpl)
{
  auto identityImpl = make_shared<detail::IdentityImpl>(id1, make_shared<pib::PibMemory>(), true);
  Identity identity1(identityImpl);
  Identity identity2(identityImpl);
  BOOST_CHECK_EQUAL(identity1, identity2);
  BOOST_CHECK_NE(identity1, Identity());
  BOOST_CHECK_EQUAL(Identity(), Identity());

//added_GM, by liupenghui
#if 1
  KeyType keyType = KeyType::NONE;
  identity1.addKey(id1Key1, id1Key1Name, keyType);
#else
  identity1.addKey(id1Key1, id1Key1Name);
#endif

  BOOST_CHECK_NO_THROW(identity2.getKey(id1Key1Name));
  identity2.removeKey(id1Key1Name);
  BOOST_CHECK_THROW(identity1.getKey(id1Key1Name), pib::Pib::Error);

//added_GM, by liupenghui
#if 1
  identity1.setDefaultKey(id1Key1, id1Key1Name, keyType);
#else
  identity1.setDefaultKey(id1Key1, id1Key1Name);
#endif
  BOOST_CHECK_NO_THROW(identity2.getDefaultKey());
}

BOOST_AUTO_TEST_SUITE_END() // TestIdentity
BOOST_AUTO_TEST_SUITE_END() // Pib
BOOST_AUTO_TEST_SUITE_END() // Security

} // namespace tests
} // namespace pib
} // namespace security
} // namespace ndn
