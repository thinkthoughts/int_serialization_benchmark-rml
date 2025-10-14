#if !defined(CHAMPAGNE_LEMIRE_PORTABILITYUTILS_H)
#define CHAMPAGNE_LEMIRE_PORTABILITYUTILS_H


#ifdef _MSC_VER
  #define CHAMPAGNE_LEMIRE_VISUAL_STUDIO 1
  #ifdef __clang__
    // clang under visual studio
    #define CHAMPAGNE_LEMIRE_CLANG_VISUAL_STUDIO 1
  #else
    // just regular visual studio (best guess)
    #define CHAMPAGNE_LEMIRE_REGULAR_VISUAL_STUDIO 1
  #endif // __clang__
#endif   // _MSC_VER



#if defined(CHAMPAGNE_LEMIRE_VISUAL_STUDIO)
  #define champagne_lemire_really_inline __forceinline // really inline in release mode

#else // CHAMPAGNE_LEMIRE_REGULAR_VISUAL_STUDIO
  #if defined(__OPTIMIZE__) || defined(NDEBUG)
    #define champagne_lemire_really_inline inline __attribute__((always_inline))
  #else
    #define champagne_lemire_really_inline inline
  #endif
#endif


#endif