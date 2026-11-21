# Creating Applications with Multiple Source Files

This document outlines the guidelines for creating multi-source-file GEOS
applications that will compile and run correctly. It was written in the
1990s and adopted for the freeGEOS project in 2026.

You are not required to follow the guidelines presented here. However,
they provide a useful framework.

## Advantages

Splitting your GOC application into multiple source files is a good idea for
several reasons:

1. Since a code resource (memory block) is generated for each source file,
   you can control the size of your code resources. Remember that in GEOS,
   resources should be kept between six and eight kilobytes in size for optimal
   paging.

2. Class definitions (`@class`/`@endc`), if placed in `.goh` files, can then be
   made available as a library for your own development and for other
   developers.

3. Compilation time can be reduced, since PMAKE only needs to recompile those
   files which have changed.

4. Normal C code can be separated from GOC code to improve code readability and
   organization.

## Tree Structure

The following folder structure has proven to be effective. For subfolder names, only the first letter should be capitalized, with the exception of “UI”.

- **MyProject**

  This is the top-level folder for your project. It contains the files *myproject.goh*, *global.goh*, *myproject.gp*, *local.mk* (if you have one), and *myproject.rev*, as well as any additional header files as needed.

- **MyProject/Art**
  
  This is where the program's graphical elements, such as the VisMonikers, are located. Typically, the
  `@visMoniker` chunks are stored in .goh files which are included by appui.goc. This folder is also
  the right place to store the sources for the graphics, such as graphic files or Icon Editor documents.

- **MyProject/UI**

  The program's static objects are declared in this directory. This is typically done in the
  appui.goc file. You can use the **mkext** tool to generate a header file that contains an
  @extern statement for each object and each chunk in this file. For large projects, the
  appui.goc file can be split into multiple files, as described below.

- **MyProject/Main**, **MyProject/Code** or similar

  This is where you store your actual code files. Depending on the complexity of the program,
  you may have several of these folders.

- You may create additional folders if appropriate for your project.
  In the PC/GEOS repository, help files are not stored within the project itself, but in the
  folder *pcgeos/Tools/build/product/bbxensem/Help* or *pcgeos/Tools/build/product/bbxensem/HelpSource*.
  For your own projects, you can, of course, also use your own **MyProject/Help** folder.

- **Important**: If you have code files in subfolders, you should no longer have any code files
  in the top-level folder. The compiler cannot handle that.

## The Source Files

### **Application Header - `myproject.goh`**

This header file will declare all `#define`d constants and `typedef`d data
types. It will also contain all `@class`/`@endc` GOC class definitions. Place
the following kinds of statements in this file:

    #define
    typedef
    @define
    @class
    @endc

### **Global Header - `global.goh`**

This header file will declare as `extern` all the global variables. Any globally used routines
can have their prototype declarations here as well.

    extern dataType globalVar;
    returnType MyGloballyUsedRoutine(dataType arg1, dataType arg2);

If needed, the `@extern` declarations for the `@visMoniker` chunks in the `.goh`
files in the `Art` folder can also be stored here.

The `@extern` declarations for objects and chunks from the UI/appui.goc file should not be
stored here. They are located in
the file UI/appui.goh, which can be easily generated and updated using the **mkext** tool.

### **Static Objects and Chunks - `appui.goc`**

The standard PC/GEOS paradigm is to put all static object definitions in
this one file. Remember that different resources will be defined within this
file.

Since the Glue linker does not create links between objects in separate source
files, you must keep children in the same source file as their parent. In other words, all objects that are statically connected should be in the same file.

However, objects that are linked only within an object subtree, such as document
or display templates, can be stored in separate `.goc` files. These resources
are typically duplicated at runtime, using the kernel routine
`ObjDuplicateResource()`, and then dynamically added to the application's object
tree, using `MSG_GEN_ADD_CHILD()`.
Don’t forget to create a separate .goh for every separate .goc file,
for example by using the **mkext** tool.

Below, we'll explain how you can still split `appui.goc` into
multiple files if it has become too large or too cluttered.

**Contents of `appui.goc`**

1. Include all the necessary header files. Case sensitivity matters, and you must use the path separator / (not \\). Otherwise, the program will not compile under Linux.

         @include <stdapp.goh>
         @include <Objects/gViewCC.goh>   /* An example of case sensitivity */
         #include <whatever.h>
         @include "myproject.goh"

2. You may define all the global variables that you declared as `extern` in the
   `global.goh` file:

         dataType globalVar1;
         dataType globalVar2;

   There is no rule specifying which file global variables must be declared in.
   The appui.goc file is a location that normally does not contain any code, and
   this makes it easy to find the global declarations.

3. Define static objects in user-defined object resources. You should declare
   your resources using the GOC `@start` and `@end` keywords, and define your
   objects using the `@object` keyword:

         @start NewResource;

         @object WhatEverClass WhatEverObject = {
             [ ...instance data initializations... ]
         }

         [ ...more objects... ]

         @end NewResource;

4. Define static data chunks in user-defined data resources. You should
   declare your resources again using the GOC keywords `@start` and `@end`, but
   add the `data` flag.

        @start DataResource1, data;
        @chunk char NoError[] = "The operation was successful."
        @end DataResource1;

5. Include the monikers from your Art folder. This is usually done in a separate moniker resource.

         @start MonikerResource, data;
           @include "Art/appMoniker.goh"
         @end MonikerResource;

    Monikers that are used directly by an object (without the `list` statement) must be stored
    in the same resource as the object that uses them. This is also true for monikers that are
    intended for use with the message `MSG_GEN_USE_VIS_MONIKER`, because this message is
    passed only the chunk handle of the moniker.

    Monikers must be defined or included before they are used.

         @start Interface;
           @include "Art/configMoniker.goh";            /* first include the file, then use the moniker */
           @object GenTriggerClass ConfigButton = {
              GI_visMoniker = @ConfigMoniker;
              [ ... ]
           }    
         @end Interface;

### **Static Objects and Chunks `@extern` Declaration - `appui.goh`**

The `@extern` declarations for objects and chunks, declared in the `appui.goc` file are stored here:

    @extern object ObjectName;
    @extern chunk ChunkName;

This file can be easily generated and updated using the **mkext** tool.

### **Splitting appui.goc into separate files**

You can also store objects that are related to each other, such as those that
are part of a menu or complex dialog boxes, in a separate `.goc` source file.
When you do this, you must dynamically add these menu object trees to the
`GenPrimary` (or the appropriate object) in your `appui.goc`
file. Here is roughly how to do this:

- In the `process.goc` file (`GenProcessClass` methods):

      @method MyProcessClass, MSG_GEN_PROCESS_OPEN_APPLICATION {
          if (!(attachFlags & AAF_RESTORING_FROM_STATE)) {
              /*
               * Do only if not restoring from state.
               * The 2 in the second parameter is the position
               * among the primary's children. I.e. MyOptionsMenu
               * will be the third child of the primary.
               */
              @send MyPrimary::MSG_GEN_ADD_CHILD(
                  MyOptionsMenu,
                  (CCF_MARK_DIRTY | 2)
              );
              @call MyOptionsMenu::MSG_GEN_SET_USABLE(
                  VUM_DELAYED_VIA_APP_QUEUE
              );
              @callsuper();
          }
      }

- If you are adding controllers, you will probably have to add them
  to the GCN lists in the application object. Use `MSG_META_GCN_LIST_ADD` to do
  this:

      @call application::MSG_META_GCN_LIST_ADD(
          @MyControllerObject,
          GAGCNLT_SELF_LOAD_OPTIONS,
          MANUFACTURER_ID_GEOWORKS
      );

- With this taken care of, you can have your `appui.goc` file contain the objects
  that are related to the application and primary objects. Then you can put the
  sometimes large object trees related to a menu in another source file. This
  keeps the compile time shorter and the code easier to organize and read.

An easy way to split the `appui.goc` file into separate files is to store
data resources - that is, resources that contain only data chunks rather than
objects - in a separate file, since data chunks are never connected to an object tree.

Resources that include monikers from the `Art` folder should not be moved to
separate .goc files; otherwise, you must declare the moniker names somewhere as `@extern`,
for example, in the global.goh file. Otherwise, the compiler will not recognize
the names of the monikers.

Don’t forget to create a separate .goh file for every separate .goc file, for example using the **mkext** tool.

### **Methods and Routines - `process.goc`, `method1.goc`, `method2.goc`, etc.**

These files hold the class declarations and method definitions for the classes. Remember that each
`.goc` source file becomes a code resource for your application. Therefore, it
is a good idea to group related methods and routines in the same file. This can
help reduce the paging that your application will generate by using locality of
code reference.

**Contents of `process.goc`, `method1.goc`, `method2.goc`, etc.**

Usually, the code file that contains the class declaration (`@classdecl`) for the program's
process class is named `process.goc`.

1. Include the necessary files:

       @include <stdapp.goh>
       #include <whatever.h>
       @include <whatever.goh>
       @include "myproject.goh"
       @include "global.goh"
       @include "UI/appui.goh"

2. Declare prototypes for the functions local to this file. Remember that the prototypes for
   globally used routines were declared in `global.goh`.

        dataType RoutineDefinedInThisFile(dataType var);

3. Declare the classes whose methods are to be implemented in this file using the
   GOC keyword `@classdecl`:

         @classdecl WhatEverClass;

   There is no rule specifying which class must be declared in which file. However,
   it is a good idea to choose a filename that makes it clear which classes are
   implemented in it or what their purpose is.

4. Implement the methods for these classes using the GOC `@method` keyword, and define
   your related routines. Do not forget to define your globally used routines somewhere.

         @method AppWhatEverClass, MSG_APP_WHATEVER_DO_IT {
             [ ...code... ]
         }

         dataType RoutineDefinedInThisFile(dataType var) {
             [ ...code... ]
         }

5. In larger projects, it may be necessary to implement methods in separate .goc
   files - for example, because the code resource would otherwise become too large.
   To do this, follow these steps:

   - Declare the methods using the `@extern method` keywords in the file that contains the
     `@classdecl` keyword for this class. If you do not do this, the
     code will not cause a compile error, but it will never be executed:

         @extern method WhatEverClass, MSG_WHAT_EVER_DO_IT_IN_A_SEPARATE_FILE;

   - In the separate `.goc` files, declare the methods using the GOC `@extern method`
     keywords.

         @extern method AppWhatEverClass, MSG_WHAT_EVER_DO_IT_IN_A_SEPARATE_FILE {
             [ ...code... ]
         }

6. If the resource is still small enough, you can declare additional classes (using `@classdecl`)
and methods (using `@method` or `@extern method`) in the same file.

### **Plain C Code - `code1.c`, `code2.c`, etc.**

These files contain normal C code. No GOC compiler directives (keywords beginning
with `@`) are used in them.

Placing C code into separate files is good because they will not have to run
through the GOC preprocessor, which will reduce compilation time. Remember that
each `.c` source file becomes a code resource for your application.

Therefore, it is a good idea to group related routines in the same file. This
can reduce the paging generated by your application by improving code locality.
