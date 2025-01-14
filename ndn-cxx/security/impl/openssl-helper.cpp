/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2021 Regents of the University of California.
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

#include "ndn-cxx/security/impl/openssl-helper.hpp"
//added_GM, by liupenghui
#if 1
#include <iostream>
#endif

namespace ndn {
namespace security {
namespace detail {

const EVP_MD*
digestAlgorithmToEvpMd(DigestAlgorithm algo)
{
  switch (algo) {
  case DigestAlgorithm::SHA224:
    return EVP_sha224();
  case DigestAlgorithm::SHA256:
    return EVP_sha256();
  case DigestAlgorithm::SHA384:
    return EVP_sha384();
  case DigestAlgorithm::SHA512:
    return EVP_sha512();
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL && !defined(OPENSSL_NO_BLAKE2)
  case DigestAlgorithm::BLAKE2B_512:
    return EVP_blake2b512();
  case DigestAlgorithm::BLAKE2S_256:
    return EVP_blake2s256();
#endif
#if OPENSSL_VERSION_NUMBER >= 0x10101001L
  case DigestAlgorithm::SHA3_224:
    return EVP_sha3_224();
  case DigestAlgorithm::SHA3_256:
    return EVP_sha3_256();
  case DigestAlgorithm::SHA3_384:
    return EVP_sha3_384();
  case DigestAlgorithm::SHA3_512:
    return EVP_sha3_512();
//added_GM, by liupenghui
//openssl ver 1.1.1 or later support SM3 
#if 1
  case DigestAlgorithm::SM3:
    return EVP_sm3();
#endif	
#endif
  default:
    return nullptr;
  }
}

int
getEvpPkeyType(EVP_PKEY* key)
{
//added_GM, by liupenghui
#if 1
  int keyType =   
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
		EVP_PKEY_type(key->type);
#else
		EVP_PKEY_base_id(key);
#endif
#if 0
  std::cout << "getEvpPkeyType EVP_PKEY_base_id keyType:"<<keyType<<"\n"<< std::endl; 
  std::cout << "getEvpPkeyType EVP_PKEY_id keyType:"<<EVP_PKEY_id(key)<<"\n"<< std::endl; 
#endif
  
  if (EVP_PKEY_EC == keyType) {
	if(EVP_PKEY_id(key) == EVP_PKEY_SM2) {
		//for debug, don't delete it
    #if 0 
		 std::cout << "getEvpPkeyType keyType:EVP_PKEY_SM2.\n"<< std::endl; 
	#endif
		 keyType = EVP_PKEY_SM2;	
	}
  }
  
  return keyType;	 
  
#else
  return
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
	EVP_PKEY_type(key->type);
#else
	EVP_PKEY_base_id(key);
#endif
#endif
}


EvpMdCtx::EvpMdCtx()
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  : m_ctx(EVP_MD_CTX_create())
#else
  : m_ctx(EVP_MD_CTX_new())
#endif
{
  if (m_ctx == nullptr)
    NDN_THROW(std::runtime_error("EVP_MD_CTX creation failed"));
}

EvpMdCtx::~EvpMdCtx()
{
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
  EVP_MD_CTX_destroy(m_ctx);
#else
  EVP_MD_CTX_free(m_ctx);
#endif
}

EvpPkeyCtx::EvpPkeyCtx(EVP_PKEY* key)
  : m_ctx(EVP_PKEY_CTX_new(key, nullptr))
{
  if (m_ctx == nullptr)
    NDN_THROW(std::runtime_error("EVP_PKEY_CTX creation failed"));
}

EvpPkeyCtx::EvpPkeyCtx(int id)
  : m_ctx(EVP_PKEY_CTX_new_id(id, nullptr))
{
  if (m_ctx == nullptr)
    NDN_THROW(std::runtime_error("EVP_PKEY_CTX creation failed"));
}

EvpPkeyCtx::~EvpPkeyCtx()
{
  EVP_PKEY_CTX_free(m_ctx);
}

Bio::Bio(Bio::MethodPtr method)
  : m_bio(BIO_new(method))
{
  if (m_bio == nullptr)
    NDN_THROW(std::runtime_error("BIO creation failed"));
}

Bio::~Bio()
{
  BIO_free_all(m_bio);
}

bool
Bio::read(span<uint8_t> buf) const noexcept
{
  BOOST_ASSERT(buf.size() <= std::numeric_limits<int>::max());
  int n = BIO_read(m_bio, buf.data(), static_cast<int>(buf.size()));
  return n >= 0 && static_cast<size_t>(n) == buf.size();
}

bool
Bio::write(span<const uint8_t> buf) noexcept
{
  BOOST_ASSERT(buf.size() <= std::numeric_limits<int>::max());
  int n = BIO_write(m_bio, buf.data(), static_cast<int>(buf.size()));
  return n >= 0 && static_cast<size_t>(n) == buf.size();
}

} // namespace detail
} // namespace security
} // namespace ndn
