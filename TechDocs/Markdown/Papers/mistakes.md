# Common Mistakes of New GEOS Programmers

This document describes strategies for avoiding common problems that can occur, especially for
programmers who are new to GEOS programming. It was written in the
1990s and adopted for the freeGEOS project in 2026.

## 1. Use EC Coding to Catch Bugs

Since GEOS is multithreaded and unlocked memory blocks can be moved out from
under a pointer, debugging memory scribbling is quite difficult. Become familiar
with the EC routines that are available within GEOS and use them liberally.

When compiled as non-EC, these routines become empty stubs, so they will not
impact the performance of the shipping version of your application.

Related to this is stress-testing your application with **Swat**. By simply turning
on many of the EC flags, you can find many bugs that would otherwise take days
to find and duplicate.

Here is a partial list of flags to set. Please note that these flags are only
applicable to an EC target.

- **General Memory**

  - **`+unlockMove`**

    Force unlocked blocks to move whenever possible. This causes memory leaks to
    be more consistently reproducible. It makes the target run very slowly,
    but it's an extremely effective way to detect memory leaks.

  - **`+free`**

    Checks free blocks on the heap, which normally should contain `0xCC`. If they
    do not, a fatal error is generated, and information about the previous blocks
    on the heap is dumped.

    The previous block is useful because often you will simply be writing past the
    end of a block. Often you can figure out what this previous block is and then
    track the bug by looking at the code.

- **LMem** This applies to chunks, huge arrays, DB, and so on.

  - **`+lmem`**

    Checks the LMem internal structures. If you find that an `LMBH_handle`, or
    another header field, is messed up, this is frequently useful for finding out
    when it is happening.

  - **`+lmemMove`**

    Forces LMem blocks to move whenever possible.

  - **`+high`**

    Simply increases the level of LMem checking.

- **VM Files**

  - **`+vm`**

    Checks VM file consistency.

  - **`+vmemDiscard`**

    Forces clean VM blocks to be discarded when unlocked.

  - **`+analVM`**

    Performs extensive VM memory checking.

- **Miscellaneous**

  - **`+graphics`**

    Checks GStrings and similar graphics-related data.

  - **`+segment`**

    Performs extensive segment checking.

  - **`+text`**

    Performs extra error checking on Text objects.

## 2. Variables

- Global variables, static variables, and static strings take up space in
  `dgroup`. `dgroup` is a fixed block, so the bigger it gets, the more grief it
  causes the memory manager. Move as much static and global data as possible to
  object instance data or chunks in a data block.

- Local variables are stored on the stack. This is only a bad thing when you
  have a local variable that is a huge structure or array - tens or hundreds of
  bytes - because it could cause the stack to overflow.

  To reduce the size of variables on the stack, change the variable to a handle,
  `optr`, or `ptr` to a block or chunk that contains the structure.

- You can see the value of the stack-pointer register for each frame on the
  stack by entering the following command in Swat:

    backtrace -rsp

## 3. Memory

- Make sure to check for failed memory allocations or locks. This can be determined
  by checking for NullHandle or a null pointer.

- Do not let resources get too big. Break them into multiple resources. A separate resource is created for each code file. The optimal resource size for
 the memory manager is 6K to 8K.

- The same applies to object resources. Note that some objects store chunks in their own
  resource at runtime. The most important ones are:
  - GenText / VisText: They store their text in a chunk within their own resource. Text objects
    that can store much more text at runtime than at compile time should be placed in smaller
    resources.
  - GenDynamicList: The (currently visible) list items of the GenDynamicList are created at
    runtime within the GenDynamicList’s resource. Especially when graphical monikers are used,
    the resource can grow very large at runtime.
  - For all objects that are assigned graphical monikers at runtime, you should keep an eye on the
    resource size.

- In a method handler, the pointer **pself** points to the instance variables of the object whose method is currently being executed. These are located in a locked object block. During certain LMem operations, this block may be moved on the heap, causing `pself` to become invalid. It must then be dereferenced again using `ObjDerefGen()` or `ObjDerefVis()`.
  - Because every call to an object method can invalidate `pself`, the system handles this for us when one method calls another method.
  - At least in the following cases, you must dereference `pself` yourself.
   Keep this problem in mind; the list is certainly not complete.
    - Performing an LMem operation on the block of the current object.
    - Calling a routine that itself performs message calls. Automatic dereferencing of `pself` does not work here.
  - Never pass a pointer to an instance variable to a method. The method could move the object block before it accesses the (now invalid) pointer.

## 4. Pointers

Understand the difference between pointer variables and pointer parameters.

- A pointer is a 32-bit value (segment and offset) that references a specific address. The type of the pointer determines the type of the variable located at that address.

      int iVal;     /* an integer variable */
      int *iPtr;    /* pointer to an integer variable */
          iPtr = &iVal;  /* iPtr now contains the address of the integer variable */

- A pointer parameter is used to pass the address of a variable. The routine `MyRoutine()` expects the address of an integer variable.

      Boolean MyRoutine(int *ptrToInt);

  The correct usage is therefore:

      int intVar;
        boolVal = MyRoutine(&intVar); /* Pass the address of the variable intVar */

  The following code contains a common error.

      int intVar;
      int *IntVarPtr;          /* Pointer to an integer variable */
        intVarPtr = &intVar;   /* This is correct */
        boolVar = MyRoutine(&intVarPtr);  /* Incorrect. The address of the pointer is passed, not the address of the integer variable. */
        boolVar = MyRoutine(intVarPtr);  /* This is correct. We pass a pointer to an integer variable. */
        

- Pointers to pointers
  
  Some routines expect a pointer to a pointer. For example, `HugeArrayLock()` expects, among other things, the address of a void pointer. It modifies the pointer so that it points to the locked huge array element. To do this, it needs the pointer's address.

      dword   HugeArrayLock(VMFileHandle vmFile, VMBlockHandle vmBlock, dword elemNum, 
              void **elemPtr,       /* Address of a pointer variable.
                                     * Pointer to element is written here */
              word *elemSize);      /* Address of a word variable.
                                     * Size of element is written here */
                                      
  The correct usage is therefore as follows. `HugeArrayLock()` is one of the rare cases where typecasting makes sense. We assume that all other parameters are set up properly.

      MyDataStruct *elemPtr;    /* A pointer to my own structure */
      word         elemSize; 
        retVal = HugeArrayLock(vmFile, vmBlock, elemNum, 
                (void*) &elemPtr,       /* pass the address of the pointer variable and cast it to void* */
                &elemSize);             /* pass the address of the word variable */

  After calling `HugeArrayLock()` in this way, elemPtr points to the locked huge array element and elemSize contains the size of this element.

- Remember this pitfall in C syntax: the asterisk refers only to the identifier that follows it. The following lines of code each define an integer pointer `a` and an integer variable `b`.
  
        int *a, b;
        int* a, b;        /* No, b is not a pointer. */
        int b, *a;

- Resolve all compiler warnings related to pointers. Use explicit type casting only if you are absolutely sure that you know what you are doing.

## 5. Threads

- Do not allow your application's UI thread to make calls to its process thread. **Never!**
  This can result in a deadlock because the process thread must occasionally make calls to the
  UI thread.

- All objects in a Gen tree must be run by the same thread, usually the UI thread.

- Use a semaphore for synchronization if you want to access the same data structures from different threads.
  Otherwise, another thread might modify the data **while** you are reading or writing it.

- Never use `UserDoDialog()` on the UI thread. `UserDoDialog()` blocks the current thread. If you
  call `UserDoDialog()` on the UI thread, visual updates will not work while the dialog box is open.

## 6. Files

Always check whether a file was opened or created successfully. You can simply check whether the
returned FileHandle is NullHandle.

If the file could not be opened or created and you try to access it, the system will crash.

## 7. Using pself, oself and self

Understand the relationships between **pself**, **oself**, and **self**.

When an object method is executed, the block containing the instance variables is locked.

- **pself** is the pointer to the instance variables. It allows access to the instance variables, for example:

      pself->FBI_drag.gstate = GrEditBitmap(global.tempFile, pself->FBI_drag.bmp, NULL);

- **oself** is the **optr** (MemHandle + ChunkHandle) of the object whose method is currently being executed. It allows an object to send messages to itself, for example:

      gstate = @call oself::MSG_VIS_VUP_CREATE_GSTATE();

- **self** is a hybrid. Depending on the context, the compiler evaluates it as `oself` or as `pself`.

      gstate = @call self::MSG_VIS_VUP_CREATE_GSTATE(); /* same as oself */
      @self->IDI_speed=0;                               /* same as pself; Note the @ character at the beginning */

- Because **pself** is a pointer, it can become invalid if the object block it points to is moved. This can happen, for example, due to LMem operations, such as creating a new chunk in the object block. In this case, **pself** must be dereferenced again.

      pself = ObjDerefGen(oself);   /* for GenericClass objects */
      pself = ObjDerefVis(oself);   /* for VisClass objects */

## 8. UI and Generic Object Trees

- GenProcessClass has no instance data. **None!** It is a hybrid class that has no space
  allocated for instance data. If you try to give it instance data, you are guaranteed
  to get memory-scribbling errors.

  Remember to use the `neverSaved` flag on the ProcessClass `@classdecl`. Do not
  use this flag on objects that have instance data unless the object is marked ignoreDirty.

- Do not overuse `HINT_DEFAULT_FOCUS` or `HINT_DEFAULT_TARGET`. Only one object at
  a given level in the Gen tree can have the default focus or target.

- Data resources should be marked with the `data` flag

      @start AppMonikerResource, data;

## 9. Localization

- The resource containing chunks to be localized must be marked with the `lmem` flag.
  This can be done in the .gp file by adding `lmem` to the resource declaration.

- String chunks without an explicit length specification, such as
  `@chunk char Info[] = “Operation was successful”;`, can be localized; chunks with
  an explicit length specification, such as `@chunk char MonikerText[16] = “GI_visMoniker”;`,
  cannot be localized.

## 10. Graphics

Understand the difference between a **GString** and a **GState**.

- A GString is a data structure containing a sequence of graphics commands. See `gstring.h`.
- A GState contains information that keeps track of how to draw, including colors,
  scaling, the current text font, and so on.
- In simple terms: a GString contains **what** is drawn, and a GState is the location **where**
  it is drawn. So: **GrDrawGstring()** draws a *GString* into a *GState*.

## 11. Miscellaneous

- For standard menus such as File or Edit, use:

      ATTR_GEN_INTERACTION_GROUP_TYPE = GIGT_????_MENU

- Close all files in use and release all memory blocks when you no longer need them. Set the corresponding handles or pointers to zero.
The `MSG_GEN_PROCESS_CLOSE_APPLICATION` handler is often a suitable place to do this for global resources.

- The document messages (???_DETACH_UI, ??_WRTE_CACHED_DATA, etc.) are some times executed after the `MSG_GEN_PROCESS_CLOSE_APPLICATION` handler has finished. Therefore, always check whether the resources used (file handles, memory) are still available (not null).

- Instance variables that contain a structure should be initialized with zeros when the class is defined.

      @class MyContentClass, VisContentClass;
          @instance GeodeToken MCI_token= { "    ", 0 };
          @instance Rectangle  MCI_rubberBand = { 0, 0, 0, 0 };
      @endc;
