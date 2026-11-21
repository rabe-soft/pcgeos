# Optimizing your Application

This document contains some tips how to optimize your application. It was written in the
1990s and adopted for the freeGEOS project in 2026.

## For Speed and Heap Usage

The following are some points to consider for optimizing your Geos application.

- Don't keep blocks locked longer than necessary, as this fragments the memory and
  causes the memory manager grief.

  If you are in a loop or something that frequently accesses a block, keep the block
  locked, as frequent locking and unlocking takes up unnecessary CPU cycles.

  Make sure to always use movable blocks. `HF_FIXED` blocks are always bad because
  they fragment the fixed heap space.

- Static variables are stored in `dgroup`. `dgroup` doesn't move, so pointers to
  data in `dgroup` can be maintained without needing to lock or dereference them.

  The trade-off is that a large `dgroup` is more likely to cause memory fragmentation
  and thus give grief to the memory manager.

- When possible, use `ChunkArrays` rather than `HugeArrays`. While huge arrays allow
  arrays to be larger than 64K, they do so through a level of abstraction that requires
  more computing to access an item in the array. The same is true of DB items.

- When performing visual updates, use `VUM_DELAYED_VIA_APP_QUEUE`. This will allow all
  the updates to occur simultaneously, thus preventing redundant UI calculations.

- Processing a message call takes a minimum of three times as long as a routine call,
  so use routines in loops or other parts of code that require speed efficiency.

- Code that must run quickly can be run by a separate thread with the `ThreadPriority`
  set appropriately—something higher than `PRIORITY_FOCUS`.

  See the special information about `ThreadCreate()` in the documentation.

- When using graphics, gstrings are generally faster than bitmaps for images that are
  comprised mostly of geometric shapes.

  For drawing bitmaps, `GrFillBitmap()` is faster than `GrDrawBitmap()`,
  but the bitmap is drawn in monochrome.
