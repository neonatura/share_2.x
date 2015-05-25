
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  @endcopyright
 */

/*
 *  The RSA public-key cryptosystem
 *
 *  Copyright (C) 2006-2014, ARM Limited, All Rights Reserved
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
 *  RSA was designed by Ron Rivest, Adi Shamir and Len Adleman.
 *
 *  http://theory.lcs.mit.edu/~rivest/rsapaper.pdf
 *  http://www.cacr.math.uwaterloo.ca/hac/about/chap8.pdf
 */

#define __MEM__SHMEM_CRYPT_RSA_C__

#include "share.h"

#define polarssl_printf printf

/*
 * Initialize an RSA context
 */
void shrsa_init( shrsa_context *ctx,
               int padding,
               int hash_id )
{
    memset( ctx, 0, sizeof( shrsa_context ) );

    shrsa_set_padding( ctx, padding, hash_id );

#if defined(RSA_THREADING_C)
    polarssl_mutex_init( &ctx->mutex );
#endif
}

/*
 * Set padding for an existing RSA context
 */
void shrsa_set_padding( shrsa_context *ctx, int padding, int hash_id )
{
    ctx->padding = padding;
    ctx->hash_id = hash_id;
}

/*
 * Generate an RSA keypair
 */
int shrsa_gen_key( shrsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 unsigned int nbits, int exponent )
{
    int ret;
    shmpi P1, Q1, H, G;

    if( f_rng == NULL || nbits < 128 || exponent < 3 )
        return( RSA_ERR_RSA_BAD_INPUT_DATA );

    shmpi_init( &P1 ); shmpi_init( &Q1 ); shmpi_init( &H ); shmpi_init( &G );

    /*
     * find primes P and Q with Q < P so that:
     * GCD( E, (P-1)*(Q-1) ) == 1
     */
    MPI_CHK( shmpi_lset( &ctx->E, exponent ) );

    do
    {
        MPI_CHK( shmpi_gen_prime( &ctx->P, ( nbits + 1 ) >> 1, 0,
                                f_rng, p_rng ) );

        MPI_CHK( shmpi_gen_prime( &ctx->Q, ( nbits + 1 ) >> 1, 0,
                                f_rng, p_rng ) );

        if( shmpi_cmp_shmpi( &ctx->P, &ctx->Q ) < 0 )
            shmpi_swap( &ctx->P, &ctx->Q );

        if( shmpi_cmp_shmpi( &ctx->P, &ctx->Q ) == 0 )
            continue;

        MPI_CHK( shmpi_mul_shmpi( &ctx->N, &ctx->P, &ctx->Q ) );
        if( shmpi_msb( &ctx->N ) != nbits )
            continue;

        MPI_CHK( shmpi_sub_int( &P1, &ctx->P, 1 ) );
        MPI_CHK( shmpi_sub_int( &Q1, &ctx->Q, 1 ) );
        MPI_CHK( shmpi_mul_shmpi( &H, &P1, &Q1 ) );
        MPI_CHK( shmpi_gcd( &G, &ctx->E, &H  ) );
    }
    while( shmpi_cmp_int( &G, 1 ) != 0 );

    /*
     * D  = E^-1 mod ((P-1)*(Q-1))
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    MPI_CHK( shmpi_inv_mod( &ctx->D , &ctx->E, &H  ) );
    MPI_CHK( shmpi_mod_shmpi( &ctx->DP, &ctx->D, &P1 ) );
    MPI_CHK( shmpi_mod_shmpi( &ctx->DQ, &ctx->D, &Q1 ) );
    MPI_CHK( shmpi_inv_mod( &ctx->QP, &ctx->Q, &ctx->P ) );

    ctx->len = ( shmpi_msb( &ctx->N ) + 7 ) >> 3;

cleanup:

    shmpi_free( &P1 ); shmpi_free( &Q1 ); shmpi_free( &H ); shmpi_free( &G );

    if( ret != 0 )
    {
        shrsa_free( ctx );
        return( RSA_ERR_RSA_KEY_GEN_FAILED + ret );
    }

    return( 0 );
}

/*
 * Check a public RSA key
 */
int shrsa_check_pubkey( const shrsa_context *ctx )
{
    if( !ctx->N.p || !ctx->E.p )
        return( RSA_ERR_RSA_KEY_CHECK_FAILED );

    if( ( ctx->N.p[0] & 1 ) == 0 ||
        ( ctx->E.p[0] & 1 ) == 0 )
        return( RSA_ERR_RSA_KEY_CHECK_FAILED );

    if( shmpi_msb( &ctx->N ) < 128 ||
        shmpi_msb( &ctx->N ) > MPI_MPI_MAX_BITS )
        return( RSA_ERR_RSA_KEY_CHECK_FAILED );

    if( shmpi_msb( &ctx->E ) < 2 ||
        shmpi_cmp_shmpi( &ctx->E, &ctx->N ) >= 0 )
        return( RSA_ERR_RSA_KEY_CHECK_FAILED );

    return( 0 );
}

/*
 * Check a private RSA key
 */
int shrsa_check_privkey( const shrsa_context *ctx )
{
    int ret;
    shmpi PQ, DE, P1, Q1, H, I, G, G2, L1, L2, DP, DQ, QP;

    if( ( ret = shrsa_check_pubkey( ctx ) ) != 0 )
        return( ret );

    if( !ctx->P.p || !ctx->Q.p || !ctx->D.p )
        return( RSA_ERR_RSA_KEY_CHECK_FAILED );

    shmpi_init( &PQ ); shmpi_init( &DE ); shmpi_init( &P1 ); shmpi_init( &Q1 );
    shmpi_init( &H  ); shmpi_init( &I  ); shmpi_init( &G  ); shmpi_init( &G2 );
    shmpi_init( &L1 ); shmpi_init( &L2 ); shmpi_init( &DP ); shmpi_init( &DQ );
    shmpi_init( &QP );

    MPI_CHK( shmpi_mul_shmpi( &PQ, &ctx->P, &ctx->Q ) );
    MPI_CHK( shmpi_mul_shmpi( &DE, &ctx->D, &ctx->E ) );
    MPI_CHK( shmpi_sub_int( &P1, &ctx->P, 1 ) );
    MPI_CHK( shmpi_sub_int( &Q1, &ctx->Q, 1 ) );
    MPI_CHK( shmpi_mul_shmpi( &H, &P1, &Q1 ) );
    MPI_CHK( shmpi_gcd( &G, &ctx->E, &H  ) );

    MPI_CHK( shmpi_gcd( &G2, &P1, &Q1 ) );
    MPI_CHK( shmpi_div_shmpi( &L1, &L2, &H, &G2 ) );
    MPI_CHK( shmpi_mod_shmpi( &I, &DE, &L1  ) );

    MPI_CHK( shmpi_mod_shmpi( &DP, &ctx->D, &P1 ) );
    MPI_CHK( shmpi_mod_shmpi( &DQ, &ctx->D, &Q1 ) );
    MPI_CHK( shmpi_inv_mod( &QP, &ctx->Q, &ctx->P ) );
    /*
     * Check for a valid PKCS1v2 private key
     */
    if( shmpi_cmp_shmpi( &PQ, &ctx->N ) != 0 ||
        shmpi_cmp_shmpi( &DP, &ctx->DP ) != 0 ||
        shmpi_cmp_shmpi( &DQ, &ctx->DQ ) != 0 ||
        shmpi_cmp_shmpi( &QP, &ctx->QP ) != 0 ||
        shmpi_cmp_int( &L2, 0 ) != 0 ||
        shmpi_cmp_int( &I, 1 ) != 0 ||
        shmpi_cmp_int( &G, 1 ) != 0 )
    {
        ret = RSA_ERR_RSA_KEY_CHECK_FAILED;
    }

cleanup:
    shmpi_free( &PQ ); shmpi_free( &DE ); shmpi_free( &P1 ); shmpi_free( &Q1 );
    shmpi_free( &H  ); shmpi_free( &I  ); shmpi_free( &G  ); shmpi_free( &G2 );
    shmpi_free( &L1 ); shmpi_free( &L2 ); shmpi_free( &DP ); shmpi_free( &DQ );
    shmpi_free( &QP );

    if( ret == RSA_ERR_RSA_KEY_CHECK_FAILED )
        return( ret );

    if( ret != 0 )
        return( RSA_ERR_RSA_KEY_CHECK_FAILED + ret );

    return( 0 );
}

/*
 * Check if contexts holding a public and private key match
 */
int shrsa_check_pub_priv( const shrsa_context *pub, const shrsa_context *prv )
{
    if( shrsa_check_pubkey( pub ) != 0 ||
        shrsa_check_privkey( prv ) != 0 )
    {
        return( RSA_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( shmpi_cmp_shmpi( &pub->N, &prv->N ) != 0 ||
        shmpi_cmp_shmpi( &pub->E, &prv->E ) != 0 )
    {
        return( RSA_ERR_RSA_KEY_CHECK_FAILED );
    }

    return( 0 );
}

/*
 * Do an RSA public key operation
 */
int shrsa_public( shrsa_context *ctx,
                const unsigned char *input,
                unsigned char *output )
{
    int ret;
    size_t olen;
    shmpi T;

    shmpi_init( &T );

    MPI_CHK( shmpi_read_binary( &T, input, ctx->len ) );

    if( shmpi_cmp_shmpi( &T, &ctx->N ) >= 0 )
    {
        shmpi_free( &T );
        return( RSA_ERR_RSA_BAD_INPUT_DATA );
    }

#if defined(RSA_THREADING_C)
    polarssl_mutex_lock( &ctx->mutex );
#endif

    olen = ctx->len;
    MPI_CHK( shmpi_exp_mod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
    MPI_CHK( shmpi_write_binary( &T, output, olen ) );

cleanup:
#if defined(RSA_THREADING_C)
    polarssl_mutex_unlock( &ctx->mutex );
#endif

    shmpi_free( &T );

    if( ret != 0 )
        return( RSA_ERR_RSA_PUBLIC_FAILED + ret );

    return( 0 );
}

/*
 * Generate or update blinding values, see section 10 of:
 *  KOCHER, Paul C. Timing attacks on implementations of Diffie-Hellman, RSA,
 *  DSS, and other systems. In : Advances in Cryptology—CRYPTO’96. Springer
 *  Berlin Heidelberg, 1996. p. 104-113.
 */
static int shrsa_prepare_blinding( shrsa_context *ctx, shmpi *Vi, shmpi *Vf,
                 int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret, count = 0;

#if defined(RSA_THREADING_C)
    polarssl_mutex_lock( &ctx->mutex );
#endif

    if( ctx->Vf.p != NULL )
    {
        /* We already have blinding values, just update them by squaring */
        MPI_CHK( shmpi_mul_shmpi( &ctx->Vi, &ctx->Vi, &ctx->Vi ) );
        MPI_CHK( shmpi_mod_shmpi( &ctx->Vi, &ctx->Vi, &ctx->N ) );
        MPI_CHK( shmpi_mul_shmpi( &ctx->Vf, &ctx->Vf, &ctx->Vf ) );
        MPI_CHK( shmpi_mod_shmpi( &ctx->Vf, &ctx->Vf, &ctx->N ) );

        goto done;
    }

    /* Unblinding value: Vf = random number, invertible mod N */
    do {
        if( count++ > 10 )
            return( RSA_ERR_RSA_RNG_FAILED );

        MPI_CHK( shmpi_fill_random( &ctx->Vf, ctx->len - 1, f_rng, p_rng ) );
        MPI_CHK( shmpi_gcd( &ctx->Vi, &ctx->Vf, &ctx->N ) );
    } while( shmpi_cmp_int( &ctx->Vi, 1 ) != 0 );

    /* Blinding value: Vi =  Vf^(-e) mod N */
    MPI_CHK( shmpi_inv_mod( &ctx->Vi, &ctx->Vf, &ctx->N ) );
    MPI_CHK( shmpi_exp_mod( &ctx->Vi, &ctx->Vi, &ctx->E, &ctx->N, &ctx->RN ) );

done:
    if( Vi != &ctx->Vi )
    {
        MPI_CHK( shmpi_copy( Vi, &ctx->Vi ) );
        MPI_CHK( shmpi_copy( Vf, &ctx->Vf ) );
    }

cleanup:
#if defined(RSA_THREADING_C)
    polarssl_mutex_unlock( &ctx->mutex );
#endif

    return( ret );
}

/*
 * Do an RSA private key operation
 */
int shrsa_private( shrsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 const unsigned char *input,
                 unsigned char *output )
{
    int ret;
    size_t olen;
    shmpi T, T1, T2;
    shmpi *Vi, *Vf;

    /*
     * When using the Chinese Remainder Theorem, we use blinding values.
     * Without threading, we just read them directly from the context,
     * otherwise we make a local copy in order to reduce locking contention.
     */
#if defined(RSA_THREADING_C)
    shmpi Vi_copy, Vf_copy;

    shmpi_init( &Vi_copy ); shmpi_init( &Vf_copy );
    Vi = &Vi_copy;
    Vf = &Vf_copy;
#else
    Vi = &ctx->Vi;
    Vf = &ctx->Vf;
#endif

    shmpi_init( &T ); shmpi_init( &T1 ); shmpi_init( &T2 );

    MPI_CHK( shmpi_read_binary( &T, input, ctx->len ) );
    if( shmpi_cmp_shmpi( &T, &ctx->N ) >= 0 )
    {
        shmpi_free( &T );
        return( RSA_ERR_RSA_BAD_INPUT_DATA );
    }

    if( f_rng != NULL )
    {
        /*
         * Blinding
         * T = T * Vi mod N
         */
        MPI_CHK( shrsa_prepare_blinding( ctx, Vi, Vf, f_rng, p_rng ) );
        MPI_CHK( shmpi_mul_shmpi( &T, &T, Vi ) );
        MPI_CHK( shmpi_mod_shmpi( &T, &T, &ctx->N ) );
    }

#if defined(RSA_THREADING_C)
    polarssl_mutex_lock( &ctx->mutex );
#endif

#if defined(RSA_RSA_NO_CRT)
    MPI_CHK( shmpi_exp_mod( &T, &T, &ctx->D, &ctx->N, &ctx->RN ) );
#else
    /*
     * faster decryption using the CRT
     *
     * T1 = input ^ dP mod P
     * T2 = input ^ dQ mod Q
     */
    MPI_CHK( shmpi_exp_mod( &T1, &T, &ctx->DP, &ctx->P, &ctx->RP ) );
    MPI_CHK( shmpi_exp_mod( &T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ ) );

    /*
     * T = (T1 - T2) * (Q^-1 mod P) mod P
     */
    MPI_CHK( shmpi_sub_shmpi( &T, &T1, &T2 ) );
    MPI_CHK( shmpi_mul_shmpi( &T1, &T, &ctx->QP ) );
    MPI_CHK( shmpi_mod_shmpi( &T, &T1, &ctx->P ) );

    /*
     * T = T2 + T * Q
     */
    MPI_CHK( shmpi_mul_shmpi( &T1, &T, &ctx->Q ) );
    MPI_CHK( shmpi_add_shmpi( &T, &T2, &T1 ) );
#endif /* RSA_RSA_NO_CRT */

    if( f_rng != NULL )
    {
        /*
         * Unblind
         * T = T * Vf mod N
         */
        MPI_CHK( shmpi_mul_shmpi( &T, &T, Vf ) );
        MPI_CHK( shmpi_mod_shmpi( &T, &T, &ctx->N ) );
    }

    olen = ctx->len;
    MPI_CHK( shmpi_write_binary( &T, output, olen ) );

cleanup:
#if defined(RSA_THREADING_C)
    polarssl_mutex_unlock( &ctx->mutex );
    shmpi_free( &Vi_copy ); shmpi_free( &Vf_copy );
#endif
    shmpi_free( &T ); shmpi_free( &T1 ); shmpi_free( &T2 );

    if( ret != 0 )
        return( RSA_ERR_RSA_PRIVATE_FAILED + ret );

    return( 0 );
}

/*
 * Add the message padding, then do an RSA operation
 */
int shrsa_pkcs1_encrypt( shrsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t ilen,
                       const unsigned char *input,
                       unsigned char *output )
{
    switch( ctx->padding )
    {
#if defined(RSA_PKCS1_V15)
        case RSA_PKCS_V15:
            return shrsa_shrsaes_pkcs1_v15_encrypt( ctx, f_rng, p_rng, mode, ilen,
                                                input, output );
#endif

#if defined(RSA_PKCS1_V21)
        case RSA_PKCS_V21:
            return shrsa_shrsaes_oaep_encrypt( ctx, f_rng, p_rng, mode, NULL, 0,
                                           ilen, input, output );
#endif

        default:
            return( RSA_ERR_RSA_INVALID_PADDING );
    }
}

/*
 * Do an RSA operation, then remove the message padding
 */
int shrsa_pkcs1_decrypt( shrsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t *olen,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t output_max_len)
{
    switch( ctx->padding )
    {
#if defined(RSA_PKCS1_V15)
        case RSA_PKCS_V15:
            return shrsa_shrsaes_pkcs1_v15_decrypt( ctx, f_rng, p_rng, mode, olen,
                                                input, output, output_max_len );
#endif

#if defined(RSA_PKCS1_V21)
        case RSA_PKCS_V21:
            return shrsa_shrsaes_oaep_decrypt( ctx, f_rng, p_rng, mode, NULL, 0,
                                           olen, input, output,
                                           output_max_len );
#endif

        default:
            return( RSA_ERR_RSA_INVALID_PADDING );
    }
}

/*
 * Do an RSA operation to sign the message digest
 */
int shrsa_pkcs1_sign( shrsa_context *ctx,
                    int (*f_rng)(void *, unsigned char *, size_t),
                    void *p_rng,
                    int mode,
                    md_type_t md_alg,
                    unsigned int hashlen,
                    const unsigned char *hash,
                    unsigned char *sig )
{
    switch( ctx->padding )
    {
#if defined(RSA_PKCS1_V15)
        case RSA_PKCS_V15:
            return shrsa_shrsassa_pkcs1_v15_sign( ctx, f_rng, p_rng, mode, md_alg,
                                              hashlen, hash, sig );
#endif

#if defined(RSA_PKCS1_V21)
        case RSA_PKCS_V21:
            return shrsa_shrsassa_pss_sign( ctx, f_rng, p_rng, mode, md_alg,
                                        hashlen, hash, sig );
#endif

        default:
            return( RSA_ERR_RSA_INVALID_PADDING );
    }
}

/*
 * Do an RSA operation and check the message digest
 */
int shrsa_pkcs1_verify( shrsa_context *ctx,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng,
                      int mode,
                      md_type_t md_alg,
                      unsigned int hashlen,
                      const unsigned char *hash,
                      const unsigned char *sig )
{
    switch( ctx->padding )
    {
#if defined(RSA_PKCS1_V15)
        case RSA_PKCS_V15:
            return shrsa_shrsassa_pkcs1_v15_verify( ctx, f_rng, p_rng, mode, md_alg,
                                                hashlen, hash, sig );
#endif

#if defined(RSA_PKCS1_V21)
        case RSA_PKCS_V21:
            return shrsa_shrsassa_pss_verify( ctx, f_rng, p_rng, mode, md_alg,
                                          hashlen, hash, sig );
#endif

        default:
            return( RSA_ERR_RSA_INVALID_PADDING );
    }
}

/*
 * Copy the components of an RSA key
 */
int shrsa_copy( shrsa_context *dst, const shrsa_context *src )
{
    int ret;

    dst->ver = src->ver;
    dst->len = src->len;

    MPI_CHK( shmpi_copy( &dst->N, &src->N ) );
    MPI_CHK( shmpi_copy( &dst->E, &src->E ) );

    MPI_CHK( shmpi_copy( &dst->D, &src->D ) );
    MPI_CHK( shmpi_copy( &dst->P, &src->P ) );
    MPI_CHK( shmpi_copy( &dst->Q, &src->Q ) );
    MPI_CHK( shmpi_copy( &dst->DP, &src->DP ) );
    MPI_CHK( shmpi_copy( &dst->DQ, &src->DQ ) );
    MPI_CHK( shmpi_copy( &dst->QP, &src->QP ) );

    MPI_CHK( shmpi_copy( &dst->RN, &src->RN ) );
    MPI_CHK( shmpi_copy( &dst->RP, &src->RP ) );
    MPI_CHK( shmpi_copy( &dst->RQ, &src->RQ ) );

    MPI_CHK( shmpi_copy( &dst->Vi, &src->Vi ) );
    MPI_CHK( shmpi_copy( &dst->Vf, &src->Vf ) );

    dst->padding = src->padding;
    dst->hash_id = src->hash_id;

cleanup:
    if( ret != 0 )
        shrsa_free( dst );

    return( ret );
}

/*
 * Free the components of an RSA key
 */
void shrsa_free( shrsa_context *ctx )
{
    shmpi_free( &ctx->Vi ); shmpi_free( &ctx->Vf );
    shmpi_free( &ctx->RQ ); shmpi_free( &ctx->RP ); shmpi_free( &ctx->RN );
    shmpi_free( &ctx->QP ); shmpi_free( &ctx->DQ ); shmpi_free( &ctx->DP );
    shmpi_free( &ctx->Q  ); shmpi_free( &ctx->P  ); shmpi_free( &ctx->D );
    shmpi_free( &ctx->E  ); shmpi_free( &ctx->N  );

#if defined(RSA_THREADING_C)
    polarssl_mutex_free( &ctx->mutex );
#endif
}


