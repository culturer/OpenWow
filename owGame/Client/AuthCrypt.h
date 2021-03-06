/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2009-2010 TrinityZero <http://www.trinityzero.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _AUTHCRYPT_H
#define _AUTHCRYPT_H

#include "BigNumber.h"
#include "ARC4.h"

#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5
class AuthCrypt
{
    public:
        AuthCrypt();
        ~AuthCrypt();

        const static size_t CRYPTED_SEND_LEN = 4;
        const static size_t CRYPTED_RECV_LEN = 6;

        void Init();

        //void SetKey(BigNumber *);
        void SetKey(uint8 *key, size_t len);

        void DecryptRecv(uint8 *, size_t);
        void EncryptSend(uint8 *, size_t);

        bool IsInitialized() { return _initialized; }

        static void GenerateKey(uint8 *, BigNumber * K);

    private:
        std::vector<uint8> _key;
        uint8 _send_i, _send_j, _recv_i, _recv_j;
        bool _initialized;
};
#else

class AuthCrypt
{
public:
	AuthCrypt();

	void Init(BigNumber* K);
	void DecryptRecv(uint8 *, size_t);
	void EncryptSend(uint8 *, size_t);

	bool IsInitialized() const { return _initialized; }

private:
	ARC4 _clientDecrypt;
	ARC4 _serverEncrypt;
	bool _initialized;
};

#endif
#endif

