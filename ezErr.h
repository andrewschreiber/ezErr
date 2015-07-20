//
//  ezErr.h
//  ezErr
//
//  Created by Andrew Schreiber on 7/19/15.
//  Copyright (c) 2015 Andrew Schreiber. All rights reserved.
//


///////////////////////////////////////////////////
// Intro
//
// ezErr helps replace boilerplate for handling NSError with code that provides more functionality, including detailed logs, nil-safety, and exportable info dictionaries. Never NSLog an error again.

/* If an NSError passed into a ezErr method is non-nil, ezErr will output a detailed set of error attributes into the console:
 
 * * * * * * * * [NSError found]
 * Detail        : NSURLConnection failed
 * Description   : The operation couldn't be completed. (Example error 42.)
 * Method name   : -[ViewController viewDidLoad]
 * File name     : ViewController.m
 * Line number   : 47
 * Main thread   : Yes
 * Error domain  : NoDomain
 * Error code    : 42
 * * * * * * * * [End of ezErr log]

*/

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

#pragma mark - Notification keys

// Observe this notification to receive error info as NSErrors are found
static NSString * const kEzErrNotification = @"kEzErrNotification";

// Keys for userInfo dictionary. Contains same info as log.
static NSString * const kEzErrDetailKey   = @"kEzErrDetailKey";
static NSString * const kEzErrFileKey     = @"kEzErrFileKey";
static NSString * const kEzErrFunctionKey = @"kEzErrFunctionKey";
static NSString * const kEzErrLineKey     = @"kEzErrLineKey";
static NSString * const kEzErrThredKey    = @"kEzErrThreadKey"; //NSNumber of 1 if on main thread, 0 otherwise.
static NSString * const kEzErrDateKey     = @"kEzErrDateKey"; //NSDate
static NSString * const kEzErrCodeKey     = @"kEzErrCodeKey"; //NSNumber
static NSString * const kEzErrDomainKey   = @"kEzErrDomainKey";


/////////////////////////////////////////////////////////////////
// Anything below this line is not intended to be used directly.

#pragma mark - Internal methods

// Gathers info about the error method and passes it to logging function
#define _as_convertForLog(error, summary)\
(_as_logErr(error, summary, [[NSString stringWithFormat:@"%s",__FILE__]lastPathComponent], [NSString stringWithFormat:@"%s",__FUNCTION__], [NSString stringWithFormat:@"%d",__LINE__], [NSThread isMainThread]))


//Performs the logging and notification sending

void _as_logErr(NSError *error,
                NSString *detail,
                NSString *file,
                NSString *function,
                NSString *line,
                BOOL onMainThread)

{
    // Check error
    if (! [ error isKindOfClass:[NSError class]] || !error.domain) return;
   
    // Protect against nil fields
    if (! detail) detail = @"No detail";
    NSString *localizedDescription = error.localizedDescription;

    NSString *domain = error.domain;
    NSString *code = [NSString stringWithFormat:@"%i", (int)error.code];


    // Generate log strings
    
    NSString *layer1 = @"\n* * * * * * * * [NSError found]";
    NSString *layer2 = [NSString stringWithFormat:@"\n* Detail        : %@",detail];
    NSString *layer3 = [NSString stringWithFormat:@"\n* Description   : %@", localizedDescription];
    NSString *layer4 = [NSString stringWithFormat:@"\n* Method name   : %@",function];
    NSString *layer5 = [NSString stringWithFormat:@"\n* File name     : %@", file];
    NSString *layer6 = [NSString stringWithFormat:@"\n* Line number   : %@", line];
    NSString *layer7 = [NSString stringWithFormat:@"\n* Main thread   : %s", onMainThread? "Yes" : "No"];
    NSString *layer8 = [NSString stringWithFormat:@"\n* Error domain  : %@",domain];
    NSString *layer9 = [NSString stringWithFormat:@"\n* Error code    : %@",code];
    NSString *layer10= [NSString stringWithFormat:@"\n* * * * * * * * [End of ezErr log]"];
    
    
    NSString *logStatement = [NSString stringWithFormat:@"%@%@%@%@%@%@%@%@%@%@", layer1, layer2, layer3, layer4, layer5, layer6, layer7, layer8, layer9, layer10];
    
    NSLog(@"%@",logStatement);
    
    // Post dictionary with error info for analytics or other use.

    NSDictionary *errorInfo = @{kEzErrDetailKey   : detail,
                                kEzErrFileKey     : file,
                                kEzErrFunctionKey : function,
                                kEzErrLineKey     : line,
                                kEzErrThredKey    : [NSNumber numberWithBool:onMainThread],
                                kEzErrDateKey     : [NSDate date],
                                kEzErrDomainKey   : domain,
                                kEzErrCodeKey     : code};
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kEzErrNotification
                                                        object:nil
                                                      userInfo:errorInfo];
}


#endif
