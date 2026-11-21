# Style Guide for PC/GEOS

This document describes guidelines for ensuring a consistent and
easy-to-understand user interface in PC/GEOS.

It was written in the 1990s and adopted for the freeGEOS project in 2026.

The original of this file is stored in `/Library/User/Doc` as `StyleGuide.doc`.
In this folder, you can find many more files explaining the concepts behind the GEOS user
interface and the associated objects.

## 1 Thoughts on Style Guide for PC/GEOS

Written by *Doug*.

We need a Style Guide.

I didnt always think this was going to be the case - one of the tenants of the
"Generic UI" concept is that the software manipulates the interface for the application
to conform to the style guide of the current specific UI. So, we dont need a style guide,
right?

Wrong. There are a number of problems that a Style Guide could help to correct:

1. The Generic UI concept itself works only to the degree that the developer sets up
   their "Generic Interface" correctly.

2. Weve taken some shortcuts on the path to the one true Generic UI. Basically, theres
   more of Motif & PM in the generic UI than might be possible.

   For the most part, this doesnt restrict new specific UIs from doing their own thing.
   It just means that PM & Motif can be simpler because they dont have to work so hard
   to enforce their own style guide rules.

   So, there are things like "Textual monikers should in general be in lower case, with the
   first character capitalized," and the standard ordering of main menus, that developers
   have to adhere to.

3. Applications, despite all the zillions of style-guide-compliant objects we provide,
   still have some of their very own code. (Oh, no! Howd that happen? :)

Taking a look at Microsofts new "Application Design Guide," I get the feeling that a Style Guide
for PC/GEOS could be put together fairly quickly. They devote exactly 4 pages to the "Principles
of User Interface Design" chapter, which consists of a few paragraphs and a bibliography.

Of the remaining 212 pages, Id estimate that a good two-thirds of it consists of details
that are hidden from developers in PC/GEOS, such as spacing between buttons in a reply bar,
the proper appearance for a print dialog, and so on.

Ill start collecting pertinent information as I run into it and place it in `/Library/User/Doc/StyleGuide.doc`.

## 2 Moniker Guidelines

Written by *Matt*.

Use the following guidelines when creating text monikers. "Text moniker," as it is used here, refers
to any text in menus, buttons, dialog boxes, and other areas of the screen that label elements
of the user interface.

### **2.1 Capitalize Every Word Except Certain Short Words**

Capitalize every word except:

- Coordinating conjunctions, such as "and," "but," and "or"
- Articles, such as "the," "a," and "an"
- Prepositions of three or fewer letters, such as "on," "in," and "of"

**Examples:**

- Baud Rate
- Characters per Second
- Rate of Insertion Point Blinking
- Speaker Volume

### **2.2 Capitalize Longer Prepositions**

Capitalize prepositions of four or more letters.

**Examples:**

- Remove Files With Folders
- Number From First Page

### **2.3 Capitalize the First and Last Words**

Capitalize the first and last words, regardless of their parts of speech.

**Examples:**

- About OS/90 Write
- Position Text At

### **2.4 Capitalize Hyphenated Compound Modifiers**

Capitalize the second word in hyphenated compound modifiers, except when the second 
word has two or fewer letters.

**Examples:**

- Use Large-Capacity Disk Device
- Copy From Built-in Disk Drive

> **Note:** A compound modifier is a modifier, such as an adjective, that consists
> of more than one word. For example, the adjectival modifier "large-capacity" in
> the phrase "large-capacity disk drive" is considered a compound modifier because
> it consists of more than one word and modifies the noun "disk drive."
>
> The modifier "large" in the noun phrase "large disk drive" is a singular
> modifier because it consists of only one word.

## 3 Message Guidelines

Written by *Matt*.

Any messages that the user sees should be clear, concise, immediately
understandable, and consistent with all other messages in the system.

### **3.1 Write Directly and Matter-of-Factly**

Address the user as a peer.

### **3.2 Write Mostly in the Active Voice**

Most sentences convey action, and the active voice is the best way to do so.

**Use:**

> Please insert the system disk into drive A.

**Not:**

> The system disk should be inserted into drive A. *(Passive voice)*

***Technical Note***
>
>You can think of the active voice as a way to write sentences so that they use active,
>empowering verbs to do their work rather than some weak form of the verb "to be."
>
>Technically, whether the grammatical subject of the sentence is the performer of the
>action or the recipient of an action determines whether the sentence is active or passive.
>
>In an active-voice sentence, the grammatical subject of the sentence performs an action. For example:
>
>> Ted threw the ball.
>
>Ted, the grammatical subject of the sentence, is the performer of the action - namely,
>throwing the ball.
>
>In contrast, in a passive-voice sentence, the grammatical subject of the sentence doesnt
>perform an action but is instead the recipient of one. For example:
>
>> The ball was thrown by Ted.
>
>The ball, the grammatical subject of the sentence, is the recipient of an action - namely,
>being thrown.
>
>In both example sentences, the actual performer of the action is the same: Ted throws the
>ball. However, only in the active-voice sentence is Ted the grammatical subject of the sentence.
>
>Sometimes a sentence has an implied subject, as in these examples:
>
>> Please insert the system disk.
>
>> The system disk should be inserted.
>
>Both sentences have an implied "you" as the subject:
>
>> [Will you] please insert the system disk.
>
>> The system disk should be inserted [by you].
>
>The same rules still apply.
>
>> **Rule of thumb:** If the main verb in a sentence contains some form of the verb "to be,"
>> and the sentence is not an identity statement - that is, of the form "x is y" - then the
>> sentence is probably in the passive voice.

### **3.3 Ask a Question When You Want the User to Make a Choice**

**Use:**

> Do you wish to save the file "My Document" before quitting?

**Not:**

> The file will be saved before quitting.

- ***Do Not Add Redundant Imperatives***

  Do not add redundant instructions to a question.

  **Not:**

  > Do you wish to save the file "My Document" before quitting? Click "Yes" to save the file.
  > Click "No" to quit without saving.

- ***Never Use Imperatives in Place of a Question***

  **Not:**

  > Click "Yes" to save the file "My Document" before quitting. Click "No" to quit
  > without saving the file.

- ***Use the Second-Person Indicative***

  Ask questions using the second person indicative ("you").

  **Examples:**

  > Do you wish to delete the file "My Document"?

  > Do you wish to allocate more memory to print this document?

  > The disk "Your Stuff" has documents on it. Do you wish to erase them?

  **Not:**

  > Should I delete the file "My Document"?

  > The disk will be formatted.

- ***Prefer Questions with Yes/No Responses***

  Whenever possible, use questions that have "Yes" or "No" responses.

  When "Yes" and "No" result in a progressive action - that is, when they both do something - you
  should also provide a "Cancel" option.

  However, when "No" does not result in a progressive action - that is, when it is synonymous
  with "Cancel" - the "Cancel" option is not necessary.

- ***Keep Questions Concise***

  Assume the user can figure out how to respond to a question without the benefit of an explanation.
  In other words, leave elaborations to the manual.

  Ask straightforward and concise questions, provide simple sets of responses, and give users a safe
  way out in case they dont know how to respond - for example, a nondestructive "No" or a "Cancel" option.

### **3.4 Use the Imperative Mood for Actions**

Use the second-person imperative mood when you want the user to perform an action. The imperative
mood takes the form of a command or direction.

**Examples:**

> Please insert the disk "Steves Stuff" into Drive A.

> You may now turn off your computer.

### **3.5 Avoid the Verb "To Be"**

Avoid the verb "to be" in all its forms. As the most common verb in the English language,
it is also the weakest. Use active, empowering verbs instead.

**Use:**

> An error occurred while reading the disk.

**Not:**

> There was an error while reading the disk.

### **3.6 Avoid Chummy or Pretentious Language**

Be neither chummy nor pretentious. Avoid sophomoric interjections like "Voila!" and
technical jargon with which the user may not be familiar.

### **3.7 Use Short Sentences and Words**

Use short sentences and short words. They aid quick comprehension.

### **3.8 Keep the Subject and Verb Together**

Place the subject of the sentence near the verb and put them both near the beginning
of the sentence. Avoid long subjects.

### **3.9 Avoid Anticipatory Phrases**

Avoid anticipatory phrases that delay the start of a sentence. In particular, avoid the following:

- It is...
- There are...
- It is necessary...
- It is important...

### **3.10 Use Sentence-Style Capitalization**

**Use:**

> An error occurred while reading the disk.

**Not:**

> An Error Occurred While Reading the Disk.

### **3.11 Use Periods After Complete Sentences**

Use a period at the end of statements that form complete sentences.

### **3.12 Do Not Use Periods After Sentence Fragments**

Do not use periods at the end of sentence fragments.

### **3.13 Do Not Mix Fragments and Full Sentences**

Do not mix sentence fragments and full sentences in the same message.
The exception to this rule is when a sentence fragment acts as a title.

### **3.14 Use One Space After Punctuation**

Place only one space after standard punctuation marks.

### **3.15 Avoid Using Technical Terms as Verbs**

Avoid using technical adjectives and nouns as verbs - for example, "interface."

### **3.16 Avoid Certain Abbreviations**

- `etc.` - use **and so on** or **and so forth** instead.
- `e.g.` - use **for example** instead.
- `i.e.` - use **that is** instead.

## 4 Dialog Guidelines

Written by *Andrew*.

### **4.1 Use Standard Dialog Boxes**

Whenever possible, use one of the `StandardDialogBoxType` options and `UserStandardDialog()`.
These are standardized boxes and messages for many common file-operation errors.

### **4.2 Avoid Custom Dialog Boxes**

Whenever you cant use one of the `StandardDialogBoxType` options, use `UserStandardDialog()` anyway,
but pass your own error-message strings.

Do **not** create a custom dialog box in your `.ui` resource. Use the standard one.

### **4.3 Write Informative Error Messages**

When you create your own error-message string, be simple but also verbose. Give the user
as much information as possible.

If the disk is full, dont display a box saying:

> Error Creating File

Instead, display a message such as:

> There is not enough room on the disk to create Foo Document. Use GeoManager to delete files.

Always attempt to give the user an idea of what they can do to resolve the problem. For example:

> The spelling dictionary file is missing. Try re-installing PC/GEOS.

It can be a pain to give good error messages to the user, but it is one of the most
important aspects of a truly user-friendly environment.

For some great error-message strings, look at the `SDS_*` strings in:

`pcgeos/Library/SpecUI/CommonUI/CMain/cmainUIDocumentControl.ui`
