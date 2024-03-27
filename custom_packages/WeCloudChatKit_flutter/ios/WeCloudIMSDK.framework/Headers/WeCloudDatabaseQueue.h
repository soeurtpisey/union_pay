//
//  WeCloudDatabaseQueue.h
//  WeCloudChatDemo
//
//  Created by 与梦信息的Mac on 2022/2/10.
//  Copyright © 2022 WeCloud. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class WeCloudDatabase;

/** To perform queries and updates on multiple threads, you'll want to use `WeCloudDatabaseQueue`.

 Using a single instance of `<WeCloudDatabase>` from multiple threads at once is a bad idea.  It has always been OK to make a `<WeCloudDatabase>` object *per thread*.  Just don't share a single instance across threads, and definitely not across multiple threads at the same time.

 Instead, use `WeCloudDatabaseQueue`. Here's how to use it:

 First, make your queue.

 WeCloudDatabaseQueue *queue = [WeCloudDatabaseQueue databaseQueueWithPath:aPath];

 Then use it like so:

    [queue inDatabase:^(WeCloudDatabase *db) {
        [db executeUpdate:@"INSERT INTO myTable VALUES (?)", [NSNumber numberWithInt:1]];
        [db executeUpdate:@"INSERT INTO myTable VALUES (?)", [NSNumber numberWithInt:2]];
        [db executeUpdate:@"INSERT INTO myTable VALUES (?)", [NSNumber numberWithInt:3]];

 WeCloudResultSet *rs = [db executeQuery:@"select * from foo"];
        while ([rs next]) {
            //…
        }
    }];

 An easy way to wrap things up in a transaction can be done like this:

    [queue inTransaction:^(WeCloudDatabase *db, BOOL *rollback) {
        [db executeUpdate:@"INSERT INTO myTable VALUES (?)", [NSNumber numberWithInt:1]];
        [db executeUpdate:@"INSERT INTO myTable VALUES (?)", [NSNumber numberWithInt:2]];
        [db executeUpdate:@"INSERT INTO myTable VALUES (?)", [NSNumber numberWithInt:3]];

        if (whoopsSomethingWrongHappened) {
            *rollback = YES;
            return;
        }
        // etc…
        [db executeUpdate:@"INSERT INTO myTable VALUES (?)", [NSNumber numberWithInt:4]];
    }];

 `WeCloudDatabaseQueue` will run the blocks on a serialized queue (hence the name of the class).  So if you call `WeCloudDatabaseQueue`'s methods from multiple threads at the same time, they will be executed in the order they are received.  This way queries and updates won't step on each other's toes, and every one is happy.

 ### See also

 - `<WeCloudDatabase>`

 @warning Do not instantiate a single `<WeCloudDatabase>` object and use it across multiple threads. Use `WeCloudDatabaseQueue` instead.
 
 @warning The calls to `WeCloudDatabaseQueue`'s methods are blocking.  So even though you are passing along blocks, they will **not** be run on another thread.

 */


@interface WeCloudDatabaseQueue : NSObject
/** Path of database */

@property (atomic, retain, nullable) NSString *path;

/** Open flags */

@property (atomic, readonly) int openFlags;

/**  Custom virtual file system name */

@property (atomic, copy, nullable) NSString *vfsName;

///----------------------------------------------------
/// @name Initialization, opening, and closing of queue
///----------------------------------------------------

/** Create queue using path.
 
 @param aPath The file path of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

+ (instancetype)databaseQueueWithPath:(NSString * _Nullable)aPath;

/** Create queue using file URL.
 
 @param url The file `NSURL` of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

+ (instancetype)databaseQueueWithURL:(NSURL * _Nullable)url;

/** Create queue using path and specified flags.
 
 @param aPath The file path of the database.
 @param openFlags Flags passed to the openWithFlags method of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */
+ (instancetype)databaseQueueWithPath:(NSString * _Nullable)aPath flags:(int)openFlags;

/** Create queue using file URL and specified flags.
 
 @param url The file `NSURL` of the database.
 @param openFlags Flags passed to the openWithFlags method of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */
+ (instancetype)databaseQueueWithURL:(NSURL * _Nullable)url flags:(int)openFlags;

/** Create queue using path.
 
 @param aPath The file path of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

- (instancetype)initWithPath:(NSString * _Nullable)aPath;

/** Create queue using file URL.
 
 @param url The file `NSURL of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

- (instancetype)initWithURL:(NSURL * _Nullable)url;

/** Create queue using path and specified flags.
 
 @param aPath The file path of the database.
 @param openFlags Flags passed to the openWithFlags method of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

- (instancetype)initWithPath:(NSString * _Nullable)aPath flags:(int)openFlags;

/** Create queue using file URL and specified flags.
 
 @param url The file path of the database.
 @param openFlags Flags passed to the openWithFlags method of the database.
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

- (instancetype)initWithURL:(NSURL * _Nullable)url flags:(int)openFlags;

/** Create queue using path and specified flags.
 
 @param aPath The file path of the database.
 @param openFlags Flags passed to the openWithFlags method of the database
 @param vfsName The name of a custom virtual file system
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

- (instancetype)initWithPath:(NSString * _Nullable)aPath flags:(int)openFlags vfs:(NSString * _Nullable)vfsName;

/** Create queue using file URL and specified flags.
 
 @param url The file `NSURL of the database.
 @param openFlags Flags passed to the openWithFlags method of the database
 @param vfsName The name of a custom virtual file system
 
 @return The `WeCloudDatabaseQueue` object. `nil` on error.
 */

- (instancetype)initWithURL:(NSURL * _Nullable)url flags:(int)openFlags vfs:(NSString * _Nullable)vfsName;

/** Returns the Class of 'WeCloudDatabase' subclass, that will be used to instantiate database object.
 
 Subclasses can override this method to return specified Class of 'WeCloudDatabase' subclass.
 
 @return The Class of 'WeCloudDatabase' subclass, that will be used to instantiate database object.
 */

+ (Class)databaseClass;

/** Close database used by queue. */

- (void)close;

/** Interupt pending database operation. */

- (void)interrupt;

///-----------------------------------------------
/// @name Dispatching database operations to queue
///-----------------------------------------------

/** Synchronously perform database operations on queue.
 
 @param block The code to be run on the queue of `WeCloudDatabaseQueue`
 */

- (void)inDatabase:(__attribute__((noescape)) void (^)(WeCloudDatabase *db))block;

/** Synchronously perform database operations on queue, using transactions.

 @param block The code to be run on the queue of `WeCloudDatabaseQueue`
 */

- (void)inTransaction:(__attribute__((noescape)) void (^)(WeCloudDatabase *db, BOOL *rollback))block;

/** Synchronously perform database operations on queue, using deferred transactions.

 @param block The code to be run on the queue of `WeCloudDatabaseQueue`
 */

- (void)inDeferredTransaction:(__attribute__((noescape)) void (^)(WeCloudDatabase *db, BOOL *rollback))block;

///-----------------------------------------------
/// @name Dispatching database operations to queue
///-----------------------------------------------

/** Synchronously perform database operations using save point.

 @param block The code to be run on the queue of `WeCloudDatabaseQueue`
 */

// NOTE: you can not nest these, since calling it will pull another database out of the pool and you'll get a deadlock.
// If you need to nest, use WeCloudDatabase's startSavePointWithName:error: instead.
- (NSError * _Nullable)inSavePoint:(__attribute__((noescape)) void (^)(WeCloudDatabase *db, BOOL *rollback))block;

@end

NS_ASSUME_NONNULL_END
