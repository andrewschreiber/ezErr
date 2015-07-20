# ezErr
Never NSLog an error again.

ezErr replaces NSError-handling boilerplate with more functionality, including *detailed logs*, *nil-safety*, and *error data export*. 
```
NSError *err = [NSError errorWithDomain:@"testDomain" 
                                   code:4 
                                  userInfo:@{NSLocalizedDescriptionKey: @"This is an example localized Description"}];
    
ezErr(err, @"Demoing ezErr")
```
Logs this into the console:

![](http://i.imgur.com/Ht7rGDa.png)



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
