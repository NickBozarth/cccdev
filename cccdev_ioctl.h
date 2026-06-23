


enum crypt_function {
    FUN_UNDEFINED,
    FUN_ENCRYPT,
    FUN_DECRYPT,
    FUN_HASH,
};


enum crypt_algorithm {
    ALG_UNDEFINED,


    /* 
     * USE WITH FUN_ENCRYPT OR FUN_DECRYPT OPTIONS
     * 
     * Symmetric encryption algorithms
     * Use the same key to encrypt and decrpyt
     */
    ALG_AES_GCM,
    ALG_TWOFISH,
    /* TOY FUNCTION! DO NOT USE IN PROD */
    ALG_CAESAR,
    /* TOY FUNCTION! DO NOT USE IN PROD */
    ALG_VIGENERE,


    /*
     * USE WITH FUN_ENCRYPT OR FUN_DECRYPT OPTIONS
     *
     * Asymmetric encryption algorithms
     * Use different keys derived from one another to encrypt/decrpyt
     */
    ALG_RSA,
    ALG_ECC,
    /* TOY FUNCTION! DO NOT USE IN PROD */
    ALG_TOY_RSA,


    /*
     * USE WITH FUN_HASH
     *
     * Hashing algorithms
     * Digests data into a unique* predictable hash
     */
    ALG_SHA2,
    ALG_MD5,
    /* TOY FUNCTION! DO NOT USE IN PROD */
    ALG_DJB2
};
