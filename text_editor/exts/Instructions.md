# About extensions

So, you want to make an extension huh? Well, you're at the right place!

note: Building extensions should only be done AFTER you complete the MP, and is a completely optional (though definitely beneficial) challenge. Since extensions use the callbacks you implement in editor, they may not work until after you complete the MP.

## CAUTIONARY WARNING

Extensions are allowed to do ANYTHING they want to. ONLY USE AN EXTENSION IF YOU HAVE COMPLETELY READ AND UNDERSTOOD THE CODE!!!!!!!

## Using extensions

To use an extension, place the extension's `.c` file in the `ext_src` directory. Then build the extensions with the instruction from the `Building extensions` section.

Run the editor with the flag `--enable-exts` to enable extensions. Make sure to provide this flag AFTER the file name. Without the flag extensions will NOT be loaded (this is useful if your extensions won't compile and you need to fix some things with your implementation of editor).

## Making my own extension

Extensions for the CS241 text editor are nothing but functions residing in familiar `.c` files! Let's take a look at an example.

```
// Author: Aneesh Durg
// Key_codes:b,y
// F_name:my_ext
// Description:"prints the word 'YAY!"
//---
void my_ext(Document *document, char** input_str, char** buffer, char k){
  (void) document;
  (void)buffer;
  if(k=='b'){
    asprintf(input_str, "YAY!");
    asprintf(&buffer[EXT_NO], "Secret String");
  }
  else{
    if(buffer[EXT_NO]){
      *input_str = strdup(buffer[EXT_NO]);
      free(buffer[EXT_NO]);
      buffer[EXT_NO] = NULL; 
    }
  }
}
```

This is the code from in `ext_src/other.c`

An extension is a function that does not return anything and accepts the following parameters:

*    A pointer to a document (document)
*    A pointer to the input string (input_str)
*    An array of char pointers (buffer)
*    The character with which your extension was called

# Extension header

Now, let's take a closer look at the structure of an extension. Each extension MUST have comments following a specific format before the function declaration. In the example these are:

```
// Author: Aneesh Durg
// Key_codes:b,y
// F_name:my_ext
// Description:"prints the word 'YAY!"
//---
```

We recommend just copy/pasting this header into you extensions and changing the requisite fields. 

The first field is the author's name, pretty self-explanatory. (You don't have to put your real name if you don't want to, just put a pseudonym)

The second field is a comma separated list of all the key codes you want your extension to be called with. These can be any ascii characters. The key codes are case sensitive. (note that the extension builder will prevent two extensions with conflicting key codes from being loaded at the same time). In this example, we can see that this extension can either be called by using `escape+b` or `escape+y`

The third field, `F_name`, is just the name of your extension's function (this is useful for the extension builder to identify which function is your extension in case you have helper functions)

The last field is just a description and is optional.

## How to define an extension

Let's look back at the function now.

```
void my_ext(Document *document, char** input_str, char** buffer, char k){
  (void) document;
  (void)buffer;
  if(k=='b'){
    asprintf(input_str, "YAY!");
    asprintf(&buffer[EXT_NO], "Secret String");
  }
  else{
    if(buffer[EXT_NO]){
      *input_str = strdup(buffer[EXT_NO]);
      free(buffer[EXT_NO]);
      buffer[EXT_NO] = NULL; 
    }
  }
}
```

The first two lines of the function are in the form `(void)parameter`. This just prevents the compiler from throwing a warning complaining that I haven't used those parameters. You may or may not need these lines. 

Since this extension can be activated with two key codes, the parameter `k` allows us to find out which key called the extension, and accordingly change the behavior. 

Let's talk about the buffer. The buffer is an array of char pointers, of which you're only allowed to use 1 entry. The entry for your extension is defined EXT_NO, which will be replaced by the extension builder with your extension id.

While there's nothing preventing you from using other entries of the buffer, you can't guarantee that your data will still be there when you go to access it later, and you may break someone else's extension. (If you really want to do this, try using values closer to the end of the buffer. hint: the buffer can hold a maximum of 100 entries)

Changing what `input_str` points to allows you to insert text at your current cursor position. 

I'll leave it an exercise to the reader to go through the code of `ext_src/example.c`. 

## What else can I do with extensions?

Remember that you have full access to the document API and your callbacks from `editor.c`. You can also access the line number from the variable `_line_num ` and the column number from `_cols`. To access the character index, you need to add `WHITESPACE` to `_cols`. Go through the code of `editor_main.c` to understand why.

If you want to change the cursor position, save your target line number and character index as a `location` in the variable `_search_line_buffer` and then call `refresh_display(document, 0, "")`.


You can also use `interactive_cmd` to get a string from the user (like we do with the search function). 

Learn more about `refresh_display` and `interactive_cmd` in the 'TUI functions' section.

You are also allowed to include any standard libraries you wish to, just provide the `#include` statement AFTER the header.

## Building extensions

Once you've written your extension and formatted it with the requisite commands, place it in the folder `ext_src` the run the command:

```
python3 build_exts.py
```

This runs the extension builder and allows the text editor to use your extensions.

then go back the the root directory of the MP and run:

```
make clean
make
```

and then run your extension with the `enable-exts` flag provided.

## TUI functions

You have access to all functions declared in `tui.h`, most importantly `refresh_display` and `interactive_cmd`

`refresh_display` does what you think it does. Providing a non-empty string for it's final parameter allows you to display a status message after the document is printed. (e.g. the 'Wrote file to ...' message when you save a file)

`interactive_cmd` takes a prompt string and then prompts the user to enter a line. 

## HELP!?!?!?

Since this isn't part of the MP we are NOT obligated to help you with building your own extensions, or if you run into a malicious extension!!! 

note: If you post a malicious extension on piazza we WILL take disciplinary action.

However, if you do get stuck or find a bug with the extension building section make a private post on piazza.







