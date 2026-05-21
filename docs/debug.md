## 🔧 Debug Configuration

To enable or disable all `printf` debug outputs, edit `RFM69HCW.h`.

Simply comment or uncomment the following line:

```c
/*
 * Activate And Deactivate all DEBUG PRINTF and Delay in one time !
 * You only need to comment or uncomment the line below
 */
#define RFM69_DEBUG_ENABLED

#ifdef RFM69_DEBUG_ENABLED
    #define RFM69_printf(color, prefix, ...) \
        printf(color prefix X " : " __VA_ARGS__)
	#define RFM69_printfs(...) printf(__VA_ARGS__)
#else
    #define RFM69_printf(color, prefix, ...)
	#define RFM69_printfs(...)
#endif

#ifdef RFM69_DEBUG_ENABLED
    #define RFM69_Delay(...) HAL_Delay(__VA_ARGS__)
#else
    #define RFM69_Delay(...)
#endif
```

#### Example

```c
RFM69_printf(R, "!Incorrect", "must be in Standby Mode\r\n");
RFM69_printf(R, "!ABORD", "\r\n");
```

### ANSI color codes

```c
#define R "\033[31m"  // Rouge
#define G "\033[32m"  // Vert
#define J "\033[33m"  // Jaune
#define B "\033[34m"  // Bleu
#define M "\033[35m"  // Magenta
#define C "\033[36m"  // Cyan
#define W "\033[37m"  // Blanc
#define X "\033[0m"   // Reset
```
