# ezErr
Never NSLog an error again!

ezErr are three macros that replace NSError-handling boilerplate with *detailed logs*, *error data export*, *tighter conditionals*, and *more safety*. 

## Logging
```Objective-C
NSDictionary *userInfo = @{ NSLocalizedDescriptionKey : @"This is an example localized Description"};
NSError *err = [NSError errorWithDomain:@"testDomain" 
                                   code:4 
                                  userInfo:userInfo];
    
ezErr(err, @"Demoing ezErr") 
// The 2nd argument is an optional NSString for context-relevant info
```
Calling ezErr logs this into the console:

![](http://i.imgur.com/Ht7rGDa.png)

## Export
ezErr will also post to defaultCenter a notification, kEzErrNotification, with the error data as the userInfo dictionary. Useful for analytics or informing the user.
```Objective-C
    kEzErrCodeKey = 4;
    kEzErrDateKey = "2015-07-20 14:47:38 +0000";
    kEzErrDetailKey = "Demoing ezErr";
    kEzErrDomainKey = testDomain;
    kEzErrFileKey = "ViewController.m";
    kEzErrFunctionKey = "-[ViewController viewDidLoad]";
    kEzErrLineKey = 33;
    kEzErrThreadKey = 1;
```

## Tighter conditionals
ezErr can be embedded in an if statement, and will still log and export if the NSError is valid.
```Objective-C
[myObject fooWithError:&error];
if (! ezErr(error, @"No foo for you"))
{
     bar();
}
```

### Skip the if
```Objective-C
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
// Idiomatic
    if (error){
      NSLog(@"Connection did not succeed. Error description:%@ . Error domain :%@". Error code:%@", error.localizedDescription, error.domain, error.code);
      }
 
 //ezErr
    ezErr(error, @"Connection did not succeed");
 }
```

### Return on error with ezErrReturn
```Objective-C
[DatabaseAPI getThingyFromDodad: myDodad callback:^(Thingy * myThingy, NSError *error) {
 
// Idiomatic
 if (!myThing){
    NSLog(@"Couldn't get thing from dodad: %@. Got error: %@", myDodad, [error localizedDescription]);
    return;
 }
 
// ezErrReturn
 if (!myThing){
    ezErrReturn(error, [NSString stringWithFormat:@"Couldn't get thing from dodad: %@",myDodad);
     }
}
```
### Call a block and return on error with ezErrBlockReturn
```Objective-C
-(void)cachedAuthenticateToTumBookWithCallback:(AsyncCallback)authCallback
 {
     [DatabaseAPI fetchTumBookInfoWithCallback:^(NSError *err)
     {
         // Idiomatic
         if (err)
         {
             NSLog(@"Error with TumBook info from cache. Error: %@", [err localizedDescription]);
             authCallback(err, NO);
             return;
         }
 
        // ezErrBlockReturn. The block will be executed after the notification, but before the return.
        ezErrBlockReturn(err, @"TumBook info from cache", authCallback(err, NO));
 
        //...
     }
 }
```
All three macros will do logging and exporting if passed a valid NSError.

## Safety first
ezErr will not execute malformed NSError instances.
```Objective-C
NSError *unsafeError = [NSError new];
// or unsafeError = [NSError errorWithDomain:nil code:5 userInfo: nil];

NSLog(@"Wonder what's in here ... %@", [unsafeError localizedDescription]);
// Crashes, SIGKILL

ezErr(unsafeError, @"Hmmm");
// Does nothing
```

# Best practices around NSError 
If a Cocoa method returns both a BOOL success (or object) _AND_ an NSError, you should check the value of success or the existance of the object before looking at the NSError. 
To quote Apple docs, "When dealing with errors passed by reference, it’s important to test the return value of the method to see whether an error occurred... Don’t just test to see whether the error pointer was set to point to an error." This is because some Cocoa methods use the NSError you pass in as temporary memory, and will not reset your NSError to nil even upon success. Though I've never seen this phenomenon in a 3rd-party API, it's good to be safe. If the method only returns an NSError, however, you are expected to check it directly.
    
