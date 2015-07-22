# ezErr

![language](https://img.shields.io/badge/Language-Objective--C-8E44AD.svg)
![MIT License](https://img.shields.io/github/license/mashape/apistatus.svg)
![Platform](https://img.shields.io/badge/platform-%20iOS%20-lightgrey.svg)

##Example
```Objective-C
[FooWithError: &error];
if (! ezErr(error, @"Foo failed"))
{
    NSLog(@"Foo succeeded);
}
```
If ```error``` is nil:
```
Foo succeeded
```
If ```error``` contains an NSError:
```
* * * * * * * * [NSError found]
* Detail        : Foo failed
* Description   : The princess is in another this castle.
* Method name   : -[ViewController viewDidLoad]
* File name     : ViewController.m
* Line number   : 48
* Main thread   : Yes
* Error domain  : SuperMarioWorld
* Error code    : -31
* * * * * * * * [End of ezErr log]
```

# Best practices around NSError 
If a Cocoa method returns both a BOOL success (or object) _AND_ an NSError, you should check the value of success or the existance of the object before looking at the NSError. 

To quote Apple docs, "When dealing with errors passed by reference, it’s important to test the return value of the method to see whether an error occurred... Don’t just test to see whether the error pointer was set to point to an error." This is because some Cocoa methods use the NSError you pass in as temporary memory, and will not reset your NSError to nil even upon success. Though I've never seen this phenomenon in a 3rd-party API, it's good to be safe. 
    
#Installation
Download ezErr.h and add it to your project. Import where needed.

#Requirements
*ARC

#Reach out
Message me on twitter at @thelastalias 

#License
MIT
