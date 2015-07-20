# ezErr
Never NSLog an error again.

ezErr replaces NSError-handling boilerplate with *detailed logs*,  *error data export*, and *tighter conditionals*. 

## Logging
```
NSDictionary *userInfo = @{ NSLocalizedDescriptionKey : @"This is an example localized Description"};
NSError *err = [NSError errorWithDomain:@"testDomain" 
                                   code:4 
                                  userInfo:userInfo];
    
ezErr(err, @"Demoing ezErr") // The 2nd argument is an optional NSString* for context-relevant info
```
Calling ezErr logs this into the console:

![](http://i.imgur.com/Ht7rGDa.png)

## Export
ezErr will also post a notification, kEzErrNotification, with the error data as the userInfo dictionary. Useful for analytics or informing the user.
```
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
ezErr will log and export if and only if the NSError is a valid instance, removing the need for if checks on errors. 
```
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    if (error){
      NSLog(@"Connection did not succeed. Error description:%@ . Error domain :%@". Error code:%@", error.localizedDescription, error.domain, error.code);
      }
 }
 
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    ezErr(error, @"Connection did not succeed");
 }
```

ezErr is safe from malformed NSError instances.
```
NSError *unsafeError = [NSError new];
// or unsafeError = [NSError errorWithDomain:nil code:5 userInfo: nil];

NSLog(@"Wonder what's in here ... %@", [unsafeError localizedDescription]);
// Crashes, SIGKILL

ezErr(unsafeError, @"Hmmm");
// Does nothing
```

ezErr can be used as a conditional
```
[myObject fooWithError:&error];

if (! ezErr(error, @"No foo for you."))
{
     bar();
}
```

Alternatively, if ezErrReturn finds an error it will return the method after logging and notifying
```
[DatabaseAPI getThingyFromDodad: myDodad callback:^(Thingy * myThingy, NSError *error) {
 
// Idiomatic
 if (!myThing){
    NSLog(@"Couldn't get thing from dodad: %@. Got error: %@", myDodad, [error localizedDescription]);
    return;
 }
 
// ezErr
 if (!myThing){
    ezErrReturn(error, [NSString stringWithFormat:@"Couldn't get thing from dodad: %@",myDodad);
     }
}
```


//You can use ezErr(error, @"Details") to print out the detailed error logs even without embedding it in an if-statement. It will only log if there is a valid NSError.

// Lastly (and very importantly !), if a Cocoa method returns both a BOOL success (or object) _AND_ an NSError, you should check the value of success or the existance of the object before looking at the NSError. To quote Apple docs, "When dealing with errors passed by reference, it’s important to test the return value of the method to see whether an error occurred... Don’t just test to see whether the error pointer was set to point to an error." This is because some Cocoa methods use the NSError you pass in as temporary memory, and will not reset your NSError to nil even upon success. Though I've never seen this phenomenon in a 3rd-party API, it's good to be safe. If the method only returns an NSError, however, you are expected to check it directly.

// End of Intro
////////////////////////////////////////////////////

#ifndef ezErr_h
#define ezErr_h

#pragma mark - ezErr(error, detail);

/* ezErr(NSError *, NSString *)
 *
 * Checks if error exists.
 * Detail is an optional NSString you can pass for further context
 * If no error, passes back NO and does nothing else.
 * If error, logs error (see line 17), posts notification, and passes back YES.
**/

#define ezErr(error, detail)\
( (_as_convertForLog(error, detail )) , [error isKindOfClass:[NSError class]] && error.domain)

/* example use for ezErr

 - (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
 // Idiomatic
 if (error){
    NSLog(@"Connection failed. Error description:%@ . Error domain :%@". Error code:%@", error.localizedDescription, error.domain, error.code);
 }
 
 //ezErr
 ezErr(error, @"Connection failed);
 
 }
*/

#pragma mark - ezErrReturn(error, detail)
/* ezErrReturn (NSError *, NSString *)
 *
 * Checks if error exists.
 * Detail is an optional NSString you can pass for further context
 * If no error, does nothing.
 * If error, logs error (see line 17), posts notification, and calls return on the original function
 * Passes back void.
 **/

#define ezErrReturn(error, detail)\
if ([error isKindOfClass:[NSError class]] && error.domain){\
_as_convertForLog(error, detail);\
return;\
}

/* Example use for ezErrReturn

[DatabaseAPI getThingFromDodad: myDodad callback:^(Thing * myThing, NSError *error) {
 
// Idiomatic
 if (!myThing){
     NSLog(@"Couldn't get thing from dodad: %@. Got error: %@", myDodad, [error localizedDescription]);
     return;
 }
 
// ezErr
 if (!myThing)
 {
     ezErrReturn(error, [NSString stringWithFormat:@"Get thing from dodad: %@",myDodad);
 }
 
 }
*/


#pragma mark - ezErrBlockReturn

/* ezErrBlockReturn(NSError *, NSString *, block)
 *
 * Checks if error exists.
 * Detail is an optional NSString you can pass for further context
 * If no error, does nothing.
 * If error, logs error, posts notification, executes block, and calls return on the original function.
 * Passes back void.
 **/

#define ezErrBlockReturn(error, detail, ...)\
if ([error isKindOfClass: [NSError class]] && error.domain){\
_as_convertForLog(error, detail);\
(__VA_ARGS__);\
return;\
}

/* Example use for ezErrBlockReturn
 
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
 
        // ezErr
        ezErrBlockReturn(err, @"TumBook info from cache", authCallback(err, NO));
 
 
        //...
     }
 }
 */
