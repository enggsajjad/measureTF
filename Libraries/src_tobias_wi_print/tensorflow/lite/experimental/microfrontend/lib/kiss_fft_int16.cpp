#include "tensorflow/lite/experimental/microfrontend/lib/kiss_fft_common.h"

#define FIXED_POINT 16
namespace kissfft_fixed16 {
#include "C:/Users/Sajjad/Documents/PlatformIO/Projects/firmware_helloprint/lib/tflite_micro_stock/third_party/kissfft/kiss_fft.c"
#include "C:/Users/Sajjad/Documents/PlatformIO/Projects/firmware_helloprint/lib/tflite_micro_stock/third_party/kissfft/tools/kiss_fftr.c"
}  // namespace kissfft_fixed16
#undef FIXED_POINT
