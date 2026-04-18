/**
 * @file platform_random.h
 * @brief Provides get_random_bytes
 *
 * get_random_bytes has two implementations, Windows and unix
 */
#ifdef _WIN32
  #include <windows.h>
  #include <wincrypt.h>
  static int get_random_bytes(char *buf, int n) {
      HCRYPTPROV prov;
      if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
          return -1;
      if (!CryptGenRandom(prov, (DWORD)n, (BYTE *)buf))
          return -1;
      CryptReleaseContext(prov, 0);
      return 0;
  }
#else
  #include <stdio.h>
  static int get_random_bytes(char *buf, int n) {
      FILE *f = fopen("/dev/urandom", "rb");
      if (!f) return -1;
      if ((int)fread(buf, 1, n, f) != n) { fclose(f); return -1; }
      fclose(f);
      return 0;
  }
#endif
