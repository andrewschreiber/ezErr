# ezErr

![language](https://img.shields.io/badge/Language-Objective--C-8E44AD.svg)
![MIT License](https://img.shields.io/github/license/mashape/apistatus.svg)
![Platform](https://img.shields.io/badge/platform-%20iOS%20-lightgrey.svg)

##Example
```Objective-C
[self fooWithError: &error];
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
* Description   : The princess is in another castle.
* Method name   : -[ViewController fooWithError]
* File name     : ViewController.m
* Line number   : 48
* Main thread   : Yes
* Error domain  : SuperMarioWorld
* Error code    : -31
* * * * * * * * [End of ezErr log]
```
 On error, ezErr will also post a notification named kEzErrNotification containing a userInfo dictionary with all the error info.
```
    kEzErrCodeKey = "-31";
    kEzErrDateKey = "2015-07-22 09:40:53 +0000";
    kEzErrDetailKey = "Foo failed";
    kEzErrDomainKey = SuperMarioWorld;
    kEzErrFileKey = "ViewController.m";
    kEzErrFunctionKey = "-[ViewController fooWithError]";
    kEzErrLineKey = 48;
    kEzErrThreadKey = 1;
```

###Pattern 1
Replace this:
```Objective-C
if(error)
{
    NSLog(@"I really don't mind writing this pattern for the 100th time. Error: %@", error);
}
else
{
    [self bar];
}
```
with this:
```Objective-C
if (! ezErr(error, @"My fingers feel great!))
{
    [self bar];
}
```

###Pattern 2
Replace this:
```Objective-C
if(error)
{
    NSLog(@"It's important I can cmd-control-f this text to find where this happened. Error: %@", error");
    return;
}
```
with this:
```Objective-C
ezErrReturn(error, @"Here's some useful information");
```

###Pattern 3
Replace this:
```Objective-C
if(error)
{
    NSLog(@"All those moments lost.. like tears in the rain... . Error: %@", error);
    callback(error, nil);
    return;
}
```
with this:
```Objective-C
NSString *analyticsHOOOO = self.keyVariable;
ezErrBlockReturn(error, analyticsHOOOO, callback(error, nil));
```

# An afterword: Best practices around NSError 
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
