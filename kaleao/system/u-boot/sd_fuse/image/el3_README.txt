Note)
 There are two el3 binary files in this directory.
 One is "el3.bin" file for non-secured Exynos7420 chip,
 the other is "el3_chopped.bin" for secured Exynos7420 chip.
 The last one has to need signing procedure with your customer key.

Caution)
 Please check necessarily if your Exynos7420 chip is secured chip or not
  before flashing el3 binary file into your target board.
 If you flash "el3_chopped.bin" file into non-secured chip,
  your target board cannot boot up.
 When using "el3_chopped.bin" file,
  do NOT flash directly it into target board without signing procedure.

