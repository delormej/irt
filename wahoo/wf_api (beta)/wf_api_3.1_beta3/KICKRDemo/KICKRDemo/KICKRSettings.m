//
//  KICKRSettings.m
//  KICKRDemo
//
//  Created by Michael Moore on 11/13/12.
//
//

#import "KICKRSettings.h"


#define DEFAULT_SEARCH_TIMEOUT       10.0

#define KEY_SEARCH_TIMEOUT           @"searchTimeout"

@interface KICKRSettings ()

- (NSString *)sandboxLibraryDirectory;
- (void)saveSettings;

@end



@implementation KICKRSettings


#pragma mark -
#pragma mark NSObject Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
	[settingsRoot release];
    [settingsDirectory release];
    
	[super dealloc];
}

//--------------------------------------------------------------------------------
- (id)init
{
	if (self = [super init])
	{
        // cache the application documents directory.
        settingsDirectory = [[[self sandboxLibraryDirectory] stringByAppendingPathComponent:@"Preferences"] retain];
        
        // build the full path to the settings file.
		NSString* filePath = [settingsDirectory stringByAppendingPathComponent:WF_KICKR_SETTINGS];
		NSFileManager* fileManager = [NSFileManager defaultManager];
        
		// load the Settings plist.
		//
		//
		// for debugging, uncomment this line to overwrite existing copy.
		// DEBUG:  overwrite existing settings file.
		//[[NSFileManager defaultManager] removeItemAtPath:filePath error:NULL];
		//
		// create the settings plist dictionary if necessary.
		if ( ![fileManager fileExistsAtPath:filePath] )
		{
            // create an empty plist dictionary.
            settingsRoot = [[NSMutableDictionary dictionaryWithCapacity:10] retain];
		}
        // otherwise, load the existing settings file.
        else
        {
            // read the plist into a dictionary instance.
            settingsRoot = [[NSMutableDictionary alloc] initWithContentsOfFile:filePath];
        }
	}
	
	return self;
}


#pragma mark - KICKRSettings Implementation

#pragma mark Private Methods

//------------------------------------------------------------------------------
- (NSString *)sandboxLibraryDirectory
{
    // get the library folder.
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    NSString* retVal = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    
    return retVal;
}

//--------------------------------------------------------------------------------
- (void)saveSettings
{
	// save the settings file to disk.
	NSString* filePath = [settingsDirectory stringByAppendingPathComponent:WF_KICKR_SETTINGS];
	[settingsRoot writeToFile:filePath atomically:YES];
}

#pragma mark Properties

//--------------------------------------------------------------------------------
- (NSTimeInterval)searchTimeout
{
    NSTimeInterval retVal = DEFAULT_SEARCH_TIMEOUT;
    
    // get the value from the settings.
    NSNumber* value = (NSNumber*)[settingsRoot objectForKey:KEY_SEARCH_TIMEOUT];
    if ( value != nil )
    {
        retVal = (NSTimeInterval)[value doubleValue];
    }
    
    return retVal;
}

//--------------------------------------------------------------------------------
- (void)setSearchTimeout:(NSTimeInterval)timeout
{
    // get the settings dictionary.
    if ( settingsRoot )
    {
        // set the value in the settings dictionary.
        [settingsRoot setObject:[NSNumber numberWithDouble:timeout] forKey:KEY_SEARCH_TIMEOUT];
        
        // save the settings.
        [self saveSettings];
    }
}


@end
