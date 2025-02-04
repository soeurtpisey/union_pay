//
//  WeCloudDatabasePool.h
//  WeCloudChatDemo
//
//  Created by 与梦信息的Mac on 2022/2/11.
//  Copyright © 2022 WeCloud. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class WeCloudDatabase;

/** Pool of `<WeCloudDatabase>` objects.

 ### See also
 
 - `<WeCloudDatabaseQueue>`
 - `<WeCloudDatabase>`

 @warning Before using `WeCloudDatabasePool`, please consider using `<WeCloudDatabaseQueue>` instead.

 If you really really really know what you're doing and `WeCloudDatabasePool` is what
 you really really need (ie, you're using a read only database), OK you can use
 it.  But just be careful not to deadlock!

 For an example on deadlocking, search for:
 `ONLY_USE_THE_POOL_IF_YOU_ARE_DOING_READS_OTHERWISE_YOULL_DEADLOCK_USE_WeCloudDATABASEQUEUE_INSTEAD`
 in the main.m file.
 */


@interface WeCloudDatabasePool : NSObject

/** Database path */

@property (atomic, copy, nullable) NSString *path;

/** Delegate object */

@property (atomic, assign, nullable) id delegate;

/** Maximum number of databases to create */

@property (atomic, assign) NSUInteger maximumNumberOfDatabasesToCreate;

/** Open flags */

@property (atomic, readonly) int openFlags;

/**  Custom virtual file system name */

@property (atomic, copy, nullable) NSString *vfsName;


///---------------------
/// @name Initialization
///---------------------

/** Create pool using path.
 
 @param aPath The file path of the database.
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

+ (instancetype)databasePoolWithPath:(NSString * _Nullable)aPath;

/** Create pool using file URL.
 
 @param url The file `NSURL` of the database.
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

+ (instancetype)databasePoolWithURL:(NSURL * _Nullable)url;

/** Create pool using path and specified flags
 
 @param aPath The file path of the database.
 @param openFlags Flags passed to the openWithFlags method of the database.
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

+ (instancetype)databasePoolWithPath:(NSString * _Nullable)aPath flags:(int)openFlags;

/** Create pool using file URL and specified flags
 
 @param url The file `NSURL` of the database.
 @param openFlags Flags passed to the openWithFlags method of the database.
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

+ (instancetype)databasePoolWithURL:(NSURL * _Nullable)url flags:(int)openFlags;

/** Create pool using path.
 
 @param aPath The file path of the database.
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

- (instancetype)initWithPath:(NSString * _Nullable)aPath;

/** Create pool using file URL.
 
 @param url The file `NSURL of the database.
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

- (instancetype)initWithURL:(NSURL * _Nullable)url;

/** Create pool using path and specified flags.
 
 @param aPath The file path of the database.
 @param openFlags Flags passed to the openWithFlags method of the database
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

- (instancetype)initWithPath:(NSString * _Nullable)aPath flags:(int)openFlags;

/** Create pool using file URL and specified flags.
 
 @param url The file `NSURL` of the database.
 @param openFlags Flags passed to the openWithFlags method of the database
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

- (instancetype)initWithURL:(NSURL * _Nullable)url flags:(int)openFlags;

/** Create pool using path and specified flags.
 
 @param aPath The file path of the database.
 @param openFlags Flags passed to the openWithFlags method of the database
 @param vfsName The name of a custom virtual file system
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

- (instancetype)initWithPath:(NSString * _Nullable)aPath flags:(int)openFlags vfs:(NSString * _Nullable)vfsName;

/** Create pool using file URL and specified flags.
 
 @param url The file `NSURL` of the database.
 @param openFlags Flags passed to the openWithFlags method of the database
 @param vfsName The name of a custom virtual file system
 
 @return The `WeCloudDatabasePool` object. `nil` on error.
 */

- (instancetype)initWithURL:(NSURL * _Nullable)url flags:(int)openFlags vfs:(NSString * _Nullable)vfsName;

/** Returns the Class of 'WeCloudDatabase' subclass, that will be used to instantiate database object.

 Subclasses can override this method to return specified Class of 'WeCloudDatabase' subclass.

 @return The Class of 'WeCloudDatabase' subclass, that will be used to instantiate database object.
 */

+ (Class)databaseClass;

///------------------------------------------------
/// @name Keeping track of checked in/out databases
///------------------------------------------------

/** Number of checked-in databases in pool
 */

@property (nonatomic, readonly) NSUInteger countOfCheckedInDatabases;

/** Number of checked-out databases in pool
 */

@property (nonatomic, readonly) NSUInteger countOfCheckedOutDatabases;

/** Total number of databases in pool
 */

@property (nonatomic, readonly) NSUInteger countOfOpenDatabases;

/** Release all databases in pool */

- (void)releaseAllDatabases;

///------------------------------------------
/// @name Perform database operations in pool
///------------------------------------------

/** Synchronously perform database operations in pool.

 @param block The code to be run on the `WeCloudDatabasePool` pool.
 */

- (void)inDatabase:(__attribute__((noescape)) void (^)(WeCloudDatabase *db))block;

/** Synchronously perform database operations in pool using transaction.

 @param block The code to be run on the `WeCloudDatabasePool` pool.
 */

- (void)inTransaction:(__attribute__((noescape)) void (^)(WeCloudDatabase *db, BOOL *rollback))block;

/** Synchronously perform database operations in pool using deferred transaction.

 @param block The code to be run on the `WeCloudDatabasePool` pool.
 */

- (void)inDeferredTransaction:(__attribute__((noescape)) void (^)(WeCloudDatabase *db, BOOL *rollback))block;

/** Synchronously perform database operations in pool using save point.

 @param block The code to be run on the `WeCloudDatabasePool` pool.
 
 @return `NSError` object if error; `nil` if successful.

 @warning You can not nest these, since calling it will pull another database out of the pool and you'll get a deadlock. If you need to nest, use `<[WeCloudDatabase startSavePointWithName:error:]>` instead.
*/

- (NSError * _Nullable)inSavePoint:(__attribute__((noescape)) void (^)(WeCloudDatabase *db, BOOL *rollback))block;

@end


/** WeCloudDatabasePool delegate category
 
 This is a category that defines the protocol for the WeCloudDatabasePool delegate
 */

@interface NSObject (WeCloudDatabasePoolDelegate)

/** Asks the delegate whether database should be added to the pool.
 
 @param pool     The `WeCloudDatabasePool` object.
 @param database The `WeCloudDatabase` object.
 
 @return `YES` if it should add database to pool; `NO` if not.
 
 */

- (BOOL)databasePool:(WeCloudDatabasePool*)pool shouldAddDatabaseToPool:(WeCloudDatabase*)database;

/** Tells the delegate that database was added to the pool.
 
 @param pool     The `WeCloudDatabasePool` object.
 @param database The `WeCloudDatabase` object.

 */

- (void)databasePool:(WeCloudDatabasePool*)pool didAddDatabase:(WeCloudDatabase*)database;

@end

NS_ASSUME_NONNULL_END

